// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2011-2013 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "db.h"
#include "net.h"
#include "checkpoints.h"
#include "util.h"
#include "main.h"
#include "kernel.h"
#include <boost/version.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#ifndef WIN32
#include "sys/stat.h"
#endif

using namespace std;
using namespace boost;


unsigned int nWalletDBUpdated;



//
// CDB
//

CCriticalSection cs_db;
static bool fDbEnvInit = false;
bool fDetachDB = false;
DbEnv dbenv(0);
map<string, int> mapFileUseCount;
static map<string, Db*> mapDb;

static void EnvShutdown()
{
    if (!fDbEnvInit)
        return;

    fDbEnvInit = false;
    try
    {
        dbenv.close(0);
    }
    catch (const DbException& e)
    {
        printf("EnvShutdown exception: %s (%d)\n", e.what(), e.get_errno());
    }
    DbEnv(0).remove(GetDataDir().string().c_str(), 0);
}

class CDBInit
{
public:
    CDBInit()
    {
    }
    ~CDBInit()
    {
        EnvShutdown();
    }
}
instance_of_cdbinit;


CDB::CDB(const char *pszFile, const char* pszMode) : pdb(NULL)
{
    int ret;
    if (pszFile == NULL)
        return;

    fReadOnly = (!strchr(pszMode, '+') && !strchr(pszMode, 'w'));
    bool fCreate = strchr(pszMode, 'c');
    unsigned int nFlags = DB_THREAD;
    if (fCreate)
        nFlags |= DB_CREATE;

    {
        LOCK(cs_db);
        if (!fDbEnvInit)
        {
            if (fShutdown)
                return;
            filesystem::path pathDataDir = GetDataDir();
            filesystem::path pathLogDir = pathDataDir / "database";
            filesystem::create_directory(pathLogDir);
            filesystem::path pathErrorFile = pathDataDir / "db.log";
            printf("dbenv.open LogDir=%s ErrorFile=%s\n", pathLogDir.string().c_str(), pathErrorFile.string().c_str());

            int nDbCache = GetArg("-dbcache", 25);
            dbenv.set_lg_dir(pathLogDir.string().c_str());
            dbenv.set_cachesize(nDbCache / 1024, (nDbCache % 1024)*1048576, 1);
            dbenv.set_lg_bsize(1048576);
            dbenv.set_lg_max(10485760);
            dbenv.set_lk_max_locks(10000);
            dbenv.set_lk_max_objects(10000);
            dbenv.set_errfile(fopen(pathErrorFile.string().c_str(), "a")); /// debug
            dbenv.set_flags(DB_TXN_WRITE_NOSYNC, 1);
            dbenv.set_flags(DB_AUTO_COMMIT, 1);
            dbenv.log_set_config(DB_LOG_AUTO_REMOVE, 1);
            ret = dbenv.open(pathDataDir.string().c_str(),
                             DB_CREATE     |
                             DB_INIT_LOCK  |
                             DB_INIT_LOG   |
                             DB_INIT_MPOOL |
                             DB_INIT_TXN   |
                             DB_THREAD     |
                             DB_RECOVER,
                             S_IRUSR | S_IWUSR);
            if (ret > 0)
                throw runtime_error(strprintf("CDB() : error %d opening database environment", ret));
            fDbEnvInit = true;
        }

        strFile = pszFile;
        ++mapFileUseCount[strFile];
        pdb = mapDb[strFile];
        if (pdb == NULL)
        {
            pdb = new Db(&dbenv, 0);

            ret = pdb->open(NULL,      // Txn pointer
                            pszFile,   // Filename
                            "main",    // Logical db name
                            DB_BTREE,  // Database type
                            nFlags,    // Flags
                            0);

            if (ret > 0)
            {
                delete pdb;
                pdb = NULL;
                {
                     LOCK(cs_db);
                    --mapFileUseCount[strFile];
                }
                strFile = "";
                throw runtime_error(strprintf("CDB() : can't open database file %s, error %d", pszFile, ret));
            }

            if (fCreate && !Exists(string("version")))
            {
                bool fTmp = fReadOnly;
                fReadOnly = false;
                WriteVersion(CLIENT_VERSION);
                fReadOnly = fTmp;
            }

            mapDb[strFile] = pdb;
        }
    }
}

void CDB::Close()
{
    if (!pdb)
        return;
    if (!vTxn.empty())
        vTxn.front()->abort();
    vTxn.clear();
    pdb = NULL;

    // Flush database activity from memory pool to disk log
    unsigned int nMinutes = 0;
    if (fReadOnly)
        nMinutes = 1;
    if (strFile == "addr.dat")
        nMinutes = 2;
    if (strFile == "blkindex.dat")
        nMinutes = 2;
    if (strFile == "blkindex.dat" && IsInitialBlockDownload())
        nMinutes = 5;

    dbenv.txn_checkpoint(nMinutes ? GetArg("-dblogsize", 100)*1024 : 0, nMinutes, 0);

    {
        LOCK(cs_db);
        --mapFileUseCount[strFile];
    }
}

void CloseDb(const string& strFile)
{
    {
        LOCK(cs_db);
        if (mapDb[strFile] != NULL)
        {
            // Close the database handle
            Db* pdb = mapDb[strFile];
            pdb->close(0);
            delete pdb;
            mapDb[strFile] = NULL;
        }
    }
}

bool CDB::Rewrite(const string& strFile, const char* pszSkip)
{
    while (!fShutdown)
    {
        {
            LOCK(cs_db);
            if (!mapFileUseCount.count(strFile) || mapFileUseCount[strFile] == 0)
            {
                // Flush log data to the dat file
                CloseDb(strFile);
                dbenv.txn_checkpoint(0, 0, 0);
                dbenv.lsn_reset(strFile.c_str(), 0);
                mapFileUseCount.erase(strFile);

                bool fSuccess = true;
                printf("Rewriting %s...\n", strFile.c_str());
                string strFileRes = strFile + ".rewrite";
                { // surround usage of db with extra {}
                    CDB db(strFile.c_str(), "r");
                    Db* pdbCopy = new Db(&dbenv, 0);
    
                    int ret = pdbCopy->open(NULL,                 // Txn pointer
                                            strFileRes.c_str(),   // Filename
                                            "main",    // Logical db name
                                            DB_BTREE,  // Database type
                                            DB_CREATE,    // Flags
                                            0);
                    if (ret > 0)
                    {
                        printf("Cannot create database file %s\n", strFileRes.c_str());
                        fSuccess = false;
                    }
    
                    Dbc* pcursor = db.GetCursor();
                    if (pcursor)
                        while (fSuccess)
                        {
                            CDataStream ssKey(SER_DISK, CLIENT_VERSION);
                            CDataStream ssValue(SER_DISK, CLIENT_VERSION);
                            int ret = db.ReadAtCursor(pcursor, ssKey, ssValue, DB_NEXT);
                            if (ret == DB_NOTFOUND)
                            {
                                pcursor->close();
                                break;
                            }
                            else if (ret != 0)
                            {
                                pcursor->close();
                                fSuccess = false;
                                break;
                            }
                            if (pszSkip &&
                                strncmp(&ssKey[0], pszSkip, std::min(ssKey.size(), strlen(pszSkip))) == 0)
                                continue;
                            if (strncmp(&ssKey[0], "\x07version", 8) == 0)
                            {
                                // Update version:
                                ssValue.clear();
                                ssValue << CLIENT_VERSION;
                            }
                            Dbt datKey(&ssKey[0], ssKey.size());
                            Dbt datValue(&ssValue[0], ssValue.size());
                            int ret2 = pdbCopy->put(NULL, &datKey, &datValue, DB_NOOVERWRITE);
                            if (ret2 > 0)
                                fSuccess = false;
                        }
                    if (fSuccess)
                    {
                        db.Close();
                        CloseDb(strFile);
                        if (pdbCopy->close(0))
                            fSuccess = false;
                        delete pdbCopy;
                    }
                }
                if (fSuccess)
                {
                    Db dbA(&dbenv, 0);
                    if (dbA.remove(strFile.c_str(), NULL, 0))
                        fSuccess = false;
                    Db dbB(&dbenv, 0);
                    if (dbB.rename(strFileRes.c_str(), NULL, strFile.c_str(), 0))
                        fSuccess = false;
                }
                if (!fSuccess)
                    printf("Rewriting of %s FAILED!\n", strFileRes.c_str());
                return fSuccess;
            }
        }
        Sleep(100);
    }
    return false;
}


void DBFlush(bool fShutdown)
{
    // Flush log data to the actual data file
    //  on all files that are not in use
    printf("DBFlush(%s)%s\n", fShutdown ? "true" : "false", fDbEnvInit ? "" : " db not started");
    if (!fDbEnvInit)
        return;
    {
        LOCK(cs_db);
        map<string, int>::iterator mi = mapFileUseCount.begin();
        while (mi != mapFileUseCount.end())
        {
            string strFile = (*mi).first;
            int nRefCount = (*mi).second;
            printf("%s refcount=%d\n", strFile.c_str(), nRefCount);
            if (nRefCount == 0)
            {
                // Move log data to the dat file
                CloseDb(strFile);
                printf("%s checkpoint\n", strFile.c_str());
                dbenv.txn_checkpoint(0, 0, 0);
                if ((strFile != "blkindex.dat" && strFile != "addr.dat") || fDetachDB) {
                    printf("%s detach\n", strFile.c_str());
                    dbenv.lsn_reset(strFile.c_str(), 0);
                }
                printf("%s closed\n", strFile.c_str());
                mapFileUseCount.erase(mi++);
            }
            else
                mi++;
        }
        if (fShutdown)
        {
            char** listp;
            if (mapFileUseCount.empty())
            {
                dbenv.log_archive(&listp, DB_ARCH_REMOVE);
                EnvShutdown();
            }
        }
    }
}






//
// CTxDB_BDB
//

bool CTxDB_BDB::ImportTo(CBlockTxDB& txdb)
{
    Dbc* pcursor = GetCursor();
    if (!pcursor)
    {
        printf("Error getting bdb tx database cursor\n");
        return false;
    }
    printf("Start import blockindex from bdb...\n");
    int nCount = 0;
    qtloop
    {
        // Read next record
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        CDataStream ssValue(SER_DISK, CLIENT_VERSION);
        int ret = ReadAtCursor(pcursor, ssKey, ssValue);
        if (ret == DB_NOTFOUND)
            break;
        else if (ret != 0)
        {
            printf("Error reading next record from bdb tx database\n");
                return false;
        }
        if (!txdb.Import(ssKey,ssValue))
        {
            printf("Error import record to tx database\n");
                return false;
        }
        nCount++;
    }
    pcursor->close();
    printf("Import blockindex item count : %d\n",nCount); 
    return true;
}



//
// CAddrDB
//

bool CAddrDB::WriteAddrman(const CAddrMan& addrman)
{
    return Write(string("addrman"), addrman);
}

bool CAddrDB::LoadAddresses()
{
    if (Read(string("addrman"), addrman))
    {
        printf("Loaded %i addresses\n", addrman.size());
        return true;
    }
    
    // Read pre-0.6 addr records

    vector<CAddress> vAddr;
    vector<vector<unsigned char> > vDelete;

    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor)
        return false;

    qtloop
    {
        // Read next record
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        CDataStream ssValue(SER_DISK, CLIENT_VERSION);
        int ret = ReadAtCursor(pcursor, ssKey, ssValue);
        if (ret == DB_NOTFOUND)
            break;
        else if (ret != 0)
            return false;

        // Unserialize
        string strType;
        ssKey >> strType;
        if (strType == "addr")
        {
            CAddress addr;
            ssValue >> addr;
            vAddr.push_back(addr);
        }
    }
    pcursor->close();

    addrman.Add(vAddr, CNetAddr("0.0.0.0"));
    printf("Loaded %i addresses\n", addrman.size());

    // Note: old records left; we ran into hangs-on-startup
    // bugs for some users who (we think) were running after
    // an unclean shutdown.

    return true;
}

bool LoadAddresses()
{
    return CAddrDB("cr+").LoadAddresses();
}


