// Copyright (c) 2016 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef  LOMOCOIN_KVDB_H
#define  LOMOCOIN_KVDB_H

#include "util.h"
#include <boost/thread.hpp>

class CKVDBEngine
{
public:    
    virtual ~CKVDBEngine() {}  

    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual bool Get(const CDataStream& ssKey,CDataStream& ssValue) = 0;
    virtual bool Put(const CDataStream& ssKey,const CDataStream& ssValue, bool fOverwrite) = 0;
    virtual bool Remove(const CDataStream& ssKey) = 0;
    virtual bool MoveFirst() = 0;
    virtual bool MoveNext(CDataStream& ssKey,CDataStream& ssValue) = 0;
};

class CKVDBWalker
{
public:
    virtual bool Handle(CDataStream& ssKey, CDataStream& ssValue) = 0;
};

class CKVDB
{
public:
    CKVDB(CKVDBEngine * engine = NULL) : dbEngine(engine)
    {
        LOCK(cs);
        if (dbEngine != NULL)
        {
            if (!dbEngine->Open())
            {
                delete dbEngine;
                dbEngine = NULL;
            }
        }
    }

    virtual ~CKVDB() 
    {
        LOCK(cs);
        if (dbEngine != NULL)
        {
            dbEngine->Close();
            delete dbEngine;
        }
    }

    bool IsValid() const 
    {
        return (dbEngine != NULL);
    }

    bool Import(const CDataStream& ssKey,const CDataStream& ssValue)
    {
        try
        {
            LOCK(cs);

            if (dbEngine == NULL)
                return false;
            return dbEngine->Put(ssKey,ssValue,true);
        }
        catch (...)
        {
        }

        return false;
    }
protected:
    template<typename K, typename T>
    bool Read(const K& key, T& value) 
    {
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        ssKey.reserve(ssKey.GetSerializeSize(key));
        ssKey << key;

        CDataStream ssValue(SER_DISK, CLIENT_VERSION);
        try
        {
            LOCK(cs);

            if (dbEngine == NULL)
                return false;

            if (dbEngine->Get(ssKey,ssValue))
            {
                ssValue >> value;
                return true;
            }
        }
        catch (const boost::thread_interrupted&) 
        {
            throw;
        }
        catch (...)
        {
        }

        return false;
    }

    template<typename K, typename T>
    bool Write(const K& key, const T& value, bool fOverwrite=true) 
    {
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        ssKey.reserve(ssKey.GetSerializeSize(key));
        ssKey << key;

        CDataStream ssValue(SER_DISK, CLIENT_VERSION);
        ssValue.reserve(ssValue.GetSerializeSize(value));
        ssValue << value;
        
        try
        {
            LOCK(cs);

            if (dbEngine == NULL)
                return false;
            return dbEngine->Put(ssKey,ssValue,fOverwrite);
        }
        catch (const boost::thread_interrupted&) 
        {
            throw;
        }
        catch (...)
        {
        }

        return false;
    }

    template<typename K>
    bool Erase(const K& key) 
    {
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        ssKey.reserve(ssKey.GetSerializeSize(key));
        ssKey << key;
        
        try
        {
            LOCK(cs);

            if (dbEngine == NULL)
                return false;
            return dbEngine->Remove(ssKey);
        }
        catch (const boost::thread_interrupted&) 
        {
            throw;
        }
        catch (...)
        {
        }

        return false;
    }
   
    bool WalkThrough(CKVDBWalker& Walker)
    {
        try
        {
            LOCK(cs);

            if (dbEngine == NULL)
                return false;

            if (!dbEngine->MoveFirst())
                return false;
            
            for (;;)
            { 
                CDataStream ssKey(SER_DISK, CLIENT_VERSION);
                CDataStream ssValue(SER_DISK, CLIENT_VERSION);
                if (!dbEngine->MoveNext(ssKey,ssValue))
                    break;

                if (!Walker.Handle(ssKey,ssValue))
                    break;                               
            }
            return true;
        }
        catch (const boost::thread_interrupted&) 
        {
            throw;
        }
        catch (...)
        {
        }

        return false;
    }  

protected:
    CCriticalSection cs;
    CKVDBEngine * dbEngine;
};

#endif //LOMOCOIN_KVDB_H

