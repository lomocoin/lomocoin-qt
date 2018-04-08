// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2012 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "walletdb.h"
#include "wallet.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;


extern CCriticalSection cs_db;
extern map<string, int> mapFileUseCount;
extern void CloseDb(const string& strFile);

//
// CWalletDB
//

bool CWalletDB::WriteName(const string& strAddress, const string& strName)
{
    nWalletDBUpdated++;
    return Write(make_pair(string("name"), strAddress), strName);
}

bool CWalletDB::EraseName(const string& strAddress)
{
    // This should only be used for sending addresses, never for receiving addresses,
    // receiving addresses must always have an address book entry if they're not change return.
    nWalletDBUpdated++;
    return Erase(make_pair(string("name"), strAddress));
}

bool CWalletDB::ReadAccount(const string& strAccount, CAccount& account)
{
    account.SetNull();
    return Read(make_pair(string("acc"), strAccount), account);
}

bool CWalletDB::WriteAccount(const string& strAccount, const CAccount& account)
{
    return Write(make_pair(string("acc"), strAccount), account);
}

bool CWalletDB::EraseAccount(const string& strAccount)
{
    return Erase(make_pair(string("acc"), strAccount));
}

int CWalletDB::LoadWallet(CWallet* pwallet)
{
    pwallet->vchDefaultKey = CPubKey();
    int nFileVersion = 0;
    vector<uint256> vWalletUpgrade;
    bool fIsEncrypted = false;

    //// todo: shouldn't we catch exceptions and try to recover and continue?
    {
        LOCK(pwallet->cs_wallet);
        int nMinVersion = 0;
        if (Read((string)"minversion", nMinVersion))
        {
            if (nMinVersion > CLIENT_VERSION)
                return DB_TOO_NEW;
            pwallet->LoadMinVersion(nMinVersion);
        }

        // Get cursor
        Dbc* pcursor = GetCursor();
        if (!pcursor)
        {
            printf("Error getting wallet database cursor\n");
            return DB_CORRUPT;
        }

        LOOP
        {
            // Read next record
            CDataStream ssKey(SER_DISK, CLIENT_VERSION);
            CDataStream ssValue(SER_DISK, CLIENT_VERSION);
            int ret = ReadAtCursor(pcursor, ssKey, ssValue);
            if (ret == DB_NOTFOUND)
                break;
            else if (ret != 0)
            {
                printf("Error reading next record from wallet database\n");
                return DB_CORRUPT;
            }

            // Unserialize
            // Taking advantage of the fact that pair serialization
            // is just the two items serialized one after the other
            string strType;
            ssKey >> strType;
            if (strType == "name")
            {
                string strAddress;
                ssKey >> strAddress;
                ssValue >> pwallet->mapAddressBook[CBitcoinAddress(strAddress).Get()];
            }

            else if (strType == "key" || strType == "wkey")
            {
                vector<unsigned char> vchPubKey;
                ssKey >> vchPubKey;
                CKey key;
                if (strType == "key")
                {
                    CPrivKey pkey;
                    ssValue >> pkey;
                    key.SetPubKey(vchPubKey);
                    key.SetPrivKey(pkey);
                    if (key.GetPubKey() != vchPubKey)
                    {
                        printf("Error reading wallet database: CPrivKey pubkey inconsistency\n");
                        return DB_CORRUPT;
                    }
                    if (!key.IsValid())
                    {
                        printf("Error reading wallet database: invalid CPrivKey\n");
                        return DB_CORRUPT;
                    }
                }
                else
                {
                    CWalletKey wkey;
                    ssValue >> wkey;
                    key.SetPubKey(vchPubKey);
                    key.SetPrivKey(wkey.vchPrivKey);
                    if (key.GetPubKey() != vchPubKey)
                    {
                        printf("Error reading wallet database: CWalletKey pubkey inconsistency\n");
                        return DB_CORRUPT;
                    }
                    if (!key.IsValid())
                    {
                        printf("Error reading wallet database: invalid CWalletKey\n");
                        return DB_CORRUPT;
                    }
                }
                if (!pwallet->LoadKey(key))
                {
                    printf("Error reading wallet database: LoadKey failed\n");
                    return DB_CORRUPT;
                }
            }
            else if (strType == "mkey")
            {
                unsigned int nID;
                ssKey >> nID;
                CMasterKey kMasterKey;
                ssValue >> kMasterKey;
                if(pwallet->mapMasterKeys.count(nID) != 0)
                {
                    printf("Error reading wallet database: duplicate CMasterKey id %u\n", nID);
                    return DB_CORRUPT;
                }
                pwallet->mapMasterKeys[nID] = kMasterKey;
                if (pwallet->nMasterKeyMaxID < nID)
                    pwallet->nMasterKeyMaxID = nID;
            }
            else if (strType == "ckey")
            {
                vector<unsigned char> vchPubKey;
                ssKey >> vchPubKey;
                vector<unsigned char> vchPrivKey;
                ssValue >> vchPrivKey;
                if (!pwallet->LoadCryptedKey(vchPubKey, vchPrivKey))
                {
                    printf("Error reading wallet database: LoadCryptedKey failed\n");
                    return DB_CORRUPT;
                }
                fIsEncrypted = true;
            }
            else if (strType == "defaultkey")
            {
                ssValue >> pwallet->vchDefaultKey;
            }
            else if (strType == "pool")
            {
                int64 nIndex;
                ssKey >> nIndex;
                pwallet->setKeyPool.insert(nIndex);
            }
            else if (strType == "version")
            {
                ssValue >> nFileVersion;
                if (nFileVersion == 10300)
                    nFileVersion = 300;
            }
            else if (strType == "cscript")
            {
                uint160 hash;
                ssKey >> hash;
                CScript script;
                ssValue >> script;
                if (!pwallet->LoadCScript(script))
                {
                    printf("Error reading wallet database: LoadCScript failed\n");
                    return DB_CORRUPT;
                }
            }
        }
        pcursor->close();
    }

    BOOST_FOREACH(uint256 hash, vWalletUpgrade)
        WriteTx(hash, pwallet->mapWallet[hash]);

    printf("nFileVersion = %d\n", nFileVersion);


    // Rewrite encrypted wallets of versions 0.4.0 and 0.5.0rc:
    if (fIsEncrypted && (nFileVersion == 40000 || nFileVersion == 50000))
        return DB_NEED_REWRITE;

    if (nFileVersion < CLIENT_VERSION) // Update
        WriteVersion(CLIENT_VERSION);

    return DB_LOAD_OK;
}

bool CWalletDB::ExportDB(CDataStream& ssBuffer)
{
    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor)
    {
        printf("Error getting wallet database cursor\n");
        return false;
    }

    
    CMasterKey kMasterKey;
    unsigned int nMaxMasterKeyID = 0;
    LOOP
    {
        // Read next record
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        CDataStream ssValue(SER_DISK, CLIENT_VERSION);
        int ret = ReadAtCursor(pcursor, ssKey, ssValue);
        if (ret == DB_NOTFOUND)
            break;
        else if (ret != 0)
        {
            printf("Error reading next record from wallet database\n");
            return false;
        }
        string strType;
        ssKey >> strType;
        if (strType == "name")
        {
            string strAddress;
            ssKey >> strAddress;
        }
        else if ((strType == "key" || strType == "wkey" || strType == "ckey" || strType == "cscript"
                 || strType == "name" || strType == "acc") && !ssKey.empty())
        {
            ssBuffer << ssKey << ssValue;
        }
        else if (strType == "mkey")
        {
            unsigned int nID;
            ssKey >> nID;
            if (nID > nMaxMasterKeyID)
            {
                nMaxMasterKeyID = nID;
                ssValue >> kMasterKey;
            }
        }
    }
    if (nMaxMasterKeyID > 0)
    {
        ssBuffer << string("mkey") << kMasterKey;
    }
    pcursor->close();
    return true;
}

void ThreadFlushWalletDB(void* parg)
{
    const string& strFile = ((const string*)parg)[0];
    static bool fOneThread;
    bool fCompactWallet = GetBoolArg("-compactwallet", false);
    if (fOneThread)
        return;
    fOneThread = true;
    if (!GetBoolArg("-flushwallet", true))
        return;

    unsigned int nLastSeen = nWalletDBUpdated;
    unsigned int nLastFlushed = nWalletDBUpdated;
    int64 nLastWalletUpdate = GetTime();
    while (!fShutdown)
    {
        Sleep(500);

        if (nLastSeen != nWalletDBUpdated)
        {
            nLastSeen = nWalletDBUpdated;
            nLastWalletUpdate = GetTime();
        }

        if (nLastFlushed != nWalletDBUpdated && GetTime() - nLastWalletUpdate >= 2)
        {
            TRY_LOCK(cs_db,lockDb);
            if (lockDb)
            {
                // Don't do this if any databases are in use
                int nRefCount = 0;
                map<string, int>::iterator mi = mapFileUseCount.begin();
                while (mi != mapFileUseCount.end())
                {
                    nRefCount += (*mi).second;
                    mi++;
                }

                if (nRefCount == 0 && !fShutdown)
                {
                    map<string, int>::iterator mi = mapFileUseCount.find(strFile);
                    if (mi != mapFileUseCount.end())
                    {
                        printf("%s ", DateTimeStrFormat(GetTime()).c_str());
                        printf("Flushing wallet.dat\n");
                        nLastFlushed = nWalletDBUpdated;
                        int64 nStart = GetTimeMillis();

                        if (fCompactWallet)
                        {
                            printf("Compacting wallet.dat\n");
                            CWalletDB(strFile).Compact();
                        }

                        // Flush wallet.dat so it's self contained
                        CloseDb(strFile);
                        dbenv.txn_checkpoint(0, 0, 0);
                        dbenv.lsn_reset(strFile.c_str(), 0);

                        mapFileUseCount.erase(mi++);
                        printf("Flushed wallet.dat %" PRI64d "ms\n", GetTimeMillis() - nStart);
                    }
                }
            }
        }
    }
}

bool BackupWallet(const CWallet& wallet, const string& strDest)
{
    if (!wallet.fFileBacked)
        return false;
    while (!fShutdown)
    {
        {
            LOCK(cs_db);
            if (!mapFileUseCount.count(wallet.strWalletFile) || mapFileUseCount[wallet.strWalletFile] == 0)
            {
                // Flush log data to the dat file
                CloseDb(wallet.strWalletFile);
                dbenv.txn_checkpoint(0, 0, 0);
                dbenv.lsn_reset(wallet.strWalletFile.c_str(), 0);
                mapFileUseCount.erase(wallet.strWalletFile);

                // Copy wallet.dat
                filesystem::path pathSrc = GetDataDir() / wallet.strWalletFile;
                filesystem::path pathDest(strDest);
                if (filesystem::is_directory(pathDest))
                    pathDest /= wallet.strWalletFile;

                try {
#if BOOST_VERSION >= 104000
                    filesystem::copy_file(pathSrc, pathDest, filesystem::copy_option::overwrite_if_exists);
#else
                    filesystem::copy_file(pathSrc, pathDest);
#endif
                    printf("copied wallet.dat to %s\n", pathDest.string().c_str());
                    return true;
                } catch(const filesystem::filesystem_error &e) {
                    printf("error copying wallet.dat to %s - %s\n", pathDest.string().c_str(), e.what());
                    return false;
                }
            }
        }
        Sleep(100);
    }
    return false;
}
