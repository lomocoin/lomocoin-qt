// Copyright (c) 2016 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef  LOMOCOIN_BLOCKTXDB_H
#define  LOMOCOIN_BLOCKTXDB_H

#include "main.h"
#include "kvdb.h"

class CBlockTxDB : public CKVDB
{
public:
    CBlockTxDB(CKVDBEngine * engine = NULL);
    ~CBlockTxDB();
public:
    bool ReadTxIndex(uint256 hash, CTxIndex& txindex);
    bool UpdateTxIndex(uint256 hash, const CTxIndex& txindex);
    bool EraseTxIndex(uint256 hash);
    bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
    bool EraseBlockIndex(uint256 hash);
    bool ReadHashBestChain(uint256& hashBestChain);
    bool WriteHashBestChain(uint256 hashBestChain);
    bool ReadBestInvalidTrust(CBigNum& bnBestInvalidTrust);
    bool WriteBestInvalidTrust(CBigNum bnBestInvalidTrust);
    bool ReadSyncCheckpoint(uint256& hashCheckpoint);
    bool WriteSyncCheckpoint(uint256 hashCheckpoint);
    bool ReadCheckpointPubKey(std::string& strPubKey);
    bool WriteCheckpointPubKey(const std::string& strPubKey);
    bool ReadV04UpgradeTime(unsigned int& nUpgradeTime);
    bool WriteV04UpgradeTime(const unsigned int& nUpgradeTime);
    bool ReadV05UpgradeTime(unsigned int& nUpgradeTime);
    bool WriteV05UpgradeTime(const unsigned int& nUpgradeTime);
    bool LoadDB(CKVDBWalker& Walker);
};

class CTxDB : public CKVDBWalker
{
public:
    CTxDB(const char* pszMode="r+"); 
public:
    void Close() {}
    bool TxnBegin();
    bool TxnCommit();
    bool TxnAbort();
    bool ReadTxIndex(uint256 hash, CTxIndex& txindex);
    bool UpdateTxIndex(uint256 hash, const CTxIndex& txindex);
    bool EraseTxIndex(const CTransaction& tx);
    bool ContainsTx(uint256 hash);
    bool ReadDiskTx(uint256 hash, CTransaction& tx, CTxIndex& txindex);
    bool ReadDiskTx(uint256 hash, CTransaction& tx);
    bool ReadDiskTx(COutPoint outpoint, CTransaction& tx, CTxIndex& txindex);
    bool ReadDiskTx(COutPoint outpoint, CTransaction& tx);
    bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
    bool EraseBlockIndex(uint256 hash);
    bool ReadHashBestChain(uint256& hashBestChain);
    bool WriteHashBestChain(uint256 hashBestChain);
    bool ReadBestInvalidTrust(CBigNum& bnBestInvalidTrust);
    bool WriteBestInvalidTrust(CBigNum bnBestInvalidTrust);
    bool ReadSyncCheckpoint(uint256& hashCheckpoint);
    bool WriteSyncCheckpoint(uint256 hashCheckpoint);
    bool ReadCheckpointPubKey(std::string& strPubKey);
    bool WriteCheckpointPubKey(const std::string& strPubKey);
    bool ReadV04UpgradeTime(unsigned int& nUpgradeTime);
    bool WriteV04UpgradeTime(const unsigned int& nUpgradeTime);
    bool ReadV05UpgradeTime(unsigned int& nUpgradeTime);
    bool WriteV05UpgradeTime(const unsigned int& nUpgradeTime);
    bool LoadBlockIndex();
    bool Handle(CDataStream& ssKey, CDataStream& ssValue);
protected:
    void ResetTxn();
    bool ImportDB();
protected:
    CCriticalSection cs_DBCache;
    CBlockTxDB* pBlockTxDB;
    std::map<uint256,CTxIndex> mapUpdatedTxIndex;
    std::map<uint256,CDiskBlockIndex> mapUpdatedBlockIndex;
    std::set<uint256> setDeletedTxIndex;
    std::set<uint256> setDeletedBlockIndex;
    uint256 hashBestChainUpdated;
    CBigNum bnBestInvalidTrustUpdated;
    bool fTxn;
};


bool InitBlockTxDB(CKVDBEngine * engine);
void DestroyBlockTxDB();

#endif //LOMOCOIN_BLOCKTXDB_H


