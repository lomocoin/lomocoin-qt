// Copyright (c) 2016 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef  LOMOCOIN_WTXDB_H
#define  LOMOCOIN_WTXDB_H

#include "kvdb.h"

class CWallet;
class CWalletTx;

class CWalletTxDB : public CKVDB
{
public:
    CWalletTxDB(CKVDBEngine * engine = NULL);
    ~CWalletTxDB();
    bool WriteTx(const uint256 hash, const CWalletTx& wtx);
    bool EraseTx(const uint256 hash);
    bool LoadAll(CWallet* pwallet);
};

#endif //LOMOCOIN_WTXDB_H


