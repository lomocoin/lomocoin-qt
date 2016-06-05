// Copyright (c) 2016 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "leveldbeng.h"
#include "leveldb/filter_policy.h"
#include "leveldb/cache.h"
#include <boost/filesystem/path.hpp>

CLevelDBArguments::CLevelDBArguments()
{
    cache = 32 << 20;
    syncwrite = true;
    files = 4096;
}

CLevelDBArguments::~CLevelDBArguments()
{
}

CLevelDBEngine::CLevelDBEngine(CLevelDBArguments& arguments)
{
    boost::filesystem::path pathLevelDB = GetDataDir() / arguments.name;

    path = pathLevelDB.string();

    options.block_cache = leveldb::NewLRUCache(arguments.cache / 2);    
    options.write_buffer_size = arguments.cache / 4;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    options.create_if_missing = true;
    options.compression = leveldb::kNoCompression;
    options.max_open_files = arguments.files;
        
    pdb = NULL;
    piter = NULL;

    readoptions.verify_checksums = true;

    writeoptions.sync = arguments.syncwrite;    
}

CLevelDBEngine::~CLevelDBEngine()
{
    delete piter;
    piter = NULL;
    delete pdb;
    pdb = NULL;
    delete options.filter_policy;
    options.filter_policy = NULL;
    delete options.block_cache;
    options.block_cache = NULL;
}

bool CLevelDBEngine::Open()
{
    leveldb::Status status = leveldb::DB::Open(options, path, &pdb);
    if (!status.ok())
    {
        printf("Failed to open / create database : %s\n",path.c_str());
        return false;
    }

    return true;
}

void CLevelDBEngine::Close()
{
    delete piter;
    piter = NULL;
    delete pdb;
    pdb = NULL;
}

bool CLevelDBEngine::Get(const CDataStream& ssKey,CDataStream& ssValue)
{    
    leveldb::Slice slKey(&ssKey[0], ssKey.size());
    std::string strValue;       
    leveldb::Status status = pdb->Get(readoptions, slKey, &strValue);
    if (status.ok())
    {
        ssValue.write(strValue.data(), strValue.size());
        return true;
    }
    return false;
}

bool CLevelDBEngine::Put(const CDataStream& ssKey,const CDataStream& ssValue, bool fOverwrite)
{
    leveldb::Slice slKey(&ssKey[0], ssKey.size());
    if (!fOverwrite)
    { 
        std::string strValue;
        leveldb::Status status = pdb->Get(readoptions, slKey, &strValue);
        if (status.ok() || !status.IsNotFound())
        {
            return false;
        }
    }

    leveldb::Slice slValue(&ssValue[0], ssValue.size());
    leveldb::Status status = pdb->Put(writeoptions, slKey, slValue);
    
    return status.ok();
}

bool CLevelDBEngine::Remove(const CDataStream& ssKey)
{
    leveldb::Slice slKey(&ssKey[0], ssKey.size());
    leveldb::Status status = pdb->Delete(writeoptions, slKey);

    return status.ok();
}

bool CLevelDBEngine::MoveFirst()
{
    if (piter == NULL)
    {
        if ((piter = pdb->NewIterator(readoptions)) == NULL)
            return false;
    }
    
    piter->SeekToFirst();

    return true;
}

bool CLevelDBEngine::MoveNext(CDataStream& ssKey,CDataStream& ssValue)
{
    if (!piter->Valid())
        return false;

    leveldb::Slice slKey = piter->key();
    leveldb::Slice slValue = piter->value();

    ssKey.write(slKey.data(), slKey.size());
    ssValue.write(slValue.data(), slValue.size());

    piter->Next();

    return true;
}

