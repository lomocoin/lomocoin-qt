// Copyright (c) 2016 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef  LOMOCOIN_LEVELDBENG_H
#define  LOMOCOIN_LEVELDBENG_H
#include "kvdb.h"
#include "util.h"
#include <leveldb/db.h>
#include <boost/filesystem/path.hpp>

class CLevelDBArguments
{
public:
    CLevelDBArguments();
    ~CLevelDBArguments();
public:
    std::string name;
    size_t cache;
    bool syncwrite;
    int files;    
};

class CLevelDBEngine : public CKVDBEngine
{
public:
    CLevelDBEngine(CLevelDBArguments& arguments);
    ~CLevelDBEngine();

    bool Open();
    void Close();

    bool Get(const CDataStream& ssKey,CDataStream& ssValue);
    bool Put(const CDataStream& ssKey,const CDataStream& ssValue, bool fOverwrite);
    bool Remove(const CDataStream& ssKey);
  
    bool MoveFirst();
    bool MoveNext(CDataStream& ssKey,CDataStream& ssValue);
protected:
    std::string path;
    leveldb::DB* pdb;
    leveldb::Iterator* piter;
    leveldb::Options options;
    leveldb::ReadOptions readoptions;
    leveldb::WriteOptions writeoptions;
};

#endif //LOMOCOIN_LEVELDBENG_H

