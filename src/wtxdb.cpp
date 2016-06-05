// Copyright (c) 2016 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "wtxdb.h"
#include "wallet.h"

using namespace std;

class CWalletTxDBLoadTx : public CKVDBWalker
{
public:
    CWallet *pWallet;

    CWalletTxDBLoadTx(CWallet *p)
    {
        pWallet = p;
    }

    bool Handle(CDataStream& ssKey, CDataStream& ssValue)
    {
        if (pWallet == NULL)
            return false;
        
        uint256 hash;
        ssKey >> hash;

        CWalletTx& wtx = pWallet->mapWallet[hash];
        ssValue >> wtx;

        wtx.BindWallet(pWallet);

        return true; 
    }
};

CWalletTxDB::CWalletTxDB(CKVDBEngine * engine)
: CKVDB(engine)
{
}

CWalletTxDB::~CWalletTxDB()
{
}

bool CWalletTxDB::WriteTx(const uint256 hash, const CWalletTx& wtx)
{
    return Write(hash,wtx);
}

bool CWalletTxDB::EraseTx(const uint256 hash)
{
    return Erase(hash);
}

bool CWalletTxDB::LoadAll(CWallet* pwallet)
{
    CWalletTxDBLoadTx loader(pwallet);
    return WalkThrough(loader);    
}


