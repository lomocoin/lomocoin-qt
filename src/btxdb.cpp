// Copyright (c) 2016 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "btxdb.h"
#include "checkpoints.h"
#include "util.h"
#include "kernel.h"
#include "db.h"
using namespace std;

static CBlockTxDB *pCommBlockTxDB = NULL;

bool InitBlockTxDB(CKVDBEngine * engine)
{
    pCommBlockTxDB = new CBlockTxDB(engine);
    return (pCommBlockTxDB != NULL);
}

void DestroyBlockTxDB()
{
    delete pCommBlockTxDB;
    pCommBlockTxDB = NULL;
}

CBlockTxDB::CBlockTxDB(CKVDBEngine * engine)
: CKVDB(engine)
{
}

CBlockTxDB::~CBlockTxDB()
{
}

bool CBlockTxDB::ReadTxIndex(uint256 hash, CTxIndex& txindex)
{
    return Read(make_pair(string("tx"), hash), txindex);
}

bool CBlockTxDB::UpdateTxIndex(uint256 hash, const CTxIndex& txindex)
{
    return Write(make_pair(string("tx"), hash), txindex);
}

bool CBlockTxDB::EraseTxIndex(uint256 hash)
{
    return Erase(make_pair(string("tx"), hash));
}

bool CBlockTxDB::WriteBlockIndex(const CDiskBlockIndex& blockindex)
{
    return Write(make_pair(string("blockindex"), blockindex.GetBlockHash()), blockindex);
}

bool CBlockTxDB::EraseBlockIndex(uint256 hash)
{
    return Erase(make_pair(string("blockindex"), hash));
}

bool CBlockTxDB::ReadHashBestChain(uint256& hashBestChain)
{
    return Read(string("hashBestChain"), hashBestChain);
}

bool CBlockTxDB::WriteHashBestChain(uint256 hashBestChain)
{
    return Write(string("hashBestChain"), hashBestChain);
}

bool CBlockTxDB::ReadBestInvalidTrust(CBigNum& bnBestInvalidTrust)
{
    return Read(string("bnBestInvalidTrust"), bnBestInvalidTrust);
}

bool CBlockTxDB::WriteBestInvalidTrust(CBigNum bnBestInvalidTrust)
{
    return Write(string("bnBestInvalidTrust"), bnBestInvalidTrust);
}

bool CBlockTxDB::ReadSyncCheckpoint(uint256& hashCheckpoint)
{
    return Read(string("hashSyncCheckpoint"), hashCheckpoint);
}

bool CBlockTxDB::WriteSyncCheckpoint(uint256 hashCheckpoint)
{
    return Write(string("hashSyncCheckpoint"), hashCheckpoint);
}

bool CBlockTxDB::ReadCheckpointPubKey(string& strPubKey)
{
    return Read(string("strCheckpointPubKey"), strPubKey);
}

bool CBlockTxDB::WriteCheckpointPubKey(const string& strPubKey)
{
    return Write(string("strCheckpointPubKey"), strPubKey);
}

bool CBlockTxDB::ReadV04UpgradeTime(unsigned int& nUpgradeTime)
{
    return Read(string("nProtocolV04UpgradeTime"), nUpgradeTime);
}

bool CBlockTxDB::WriteV04UpgradeTime(const unsigned int& nUpgradeTime)
{
    return Write(string("nProtocolV04UpgradeTime"), nUpgradeTime);
}

bool CBlockTxDB::ReadV05UpgradeTime(unsigned int& nUpgradeTime)
{
    return Read(string("nProtocolV05UpgradeTime"), nUpgradeTime);
}

bool CBlockTxDB::WriteV05UpgradeTime(const unsigned int& nUpgradeTime)
{
    return Write(string("nProtocolV05UpgradeTime"), nUpgradeTime);
}

bool CBlockTxDB::LoadDB(CKVDBWalker& Walker)
{
    return WalkThrough(Walker);
}

CTxDB::CTxDB(const char* pszMode)
: pBlockTxDB(pCommBlockTxDB),fTxn(false)
{
}

void CTxDB::ResetTxn()
{
    mapUpdatedTxIndex.clear();
    mapUpdatedBlockIndex.clear();
    setDeletedTxIndex.clear();
    setDeletedBlockIndex.clear();
    hashBestChainUpdated = uint256(0);
    bnBestInvalidTrustUpdated = CBigNum(0); 
}

bool CTxDB::TxnBegin()
{
    LOCK(cs_DBCache);
    if (fTxn)
        return false;
    ResetTxn();
    fTxn = true;
    return true;
}

bool CTxDB::TxnCommit()
{
    LOCK(cs_DBCache);
    for (map<uint256,CTxIndex>::iterator it = mapUpdatedTxIndex.begin();
         it != mapUpdatedTxIndex.end();++it)
    {
        pBlockTxDB->UpdateTxIndex((*it).first,(*it).second);
    }
    for (map<uint256,CDiskBlockIndex>::iterator it = mapUpdatedBlockIndex.begin();
         it != mapUpdatedBlockIndex.end();++it)
    {
        pBlockTxDB->WriteBlockIndex((*it).second);
    }
    BOOST_FOREACH(const uint256& hash,setDeletedTxIndex)
    {
        pBlockTxDB->EraseTxIndex(hash);
    }
    BOOST_FOREACH(const uint256& hash,setDeletedBlockIndex)
    {
        pBlockTxDB->EraseBlockIndex(hash);
    }
    if (hashBestChainUpdated != 0)
    {
        pBlockTxDB->WriteHashBestChain(hashBestChainUpdated);
    }
    if (bnBestInvalidTrustUpdated != 0)
    {
        pBlockTxDB->WriteBestInvalidTrust(bnBestInvalidTrustUpdated);
    }
    ResetTxn();
    fTxn = false;
    return true;
}

bool CTxDB::TxnAbort()
{
    LOCK(cs_DBCache);
    ResetTxn();
    fTxn = false;
    return true;
}

bool CTxDB::ReadTxIndex(uint256 hash, CTxIndex& txindex)
{
    txindex.SetNull();
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            if (setDeletedTxIndex.count(hash))
            {
                return false;
            }
            map<uint256,CTxIndex>::iterator it = mapUpdatedTxIndex.find(hash); 
            if (it != mapUpdatedTxIndex.end())
            {
                txindex = (*it).second;
                return true;
            }
        }
    }
    return pBlockTxDB->ReadTxIndex(hash,txindex);
}

bool CTxDB::UpdateTxIndex(uint256 hash, const CTxIndex& txindex)
{
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            mapUpdatedTxIndex[hash] = txindex;
            setDeletedTxIndex.erase(hash);
            return true; 
        }
    }
    return pBlockTxDB->UpdateTxIndex(hash,txindex);
}

bool CTxDB::EraseTxIndex(const CTransaction& tx)
{
    uint256 hash = tx.GetHash();
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            mapUpdatedTxIndex.erase(hash);
            setDeletedTxIndex.insert(hash);
            return true; 
        }
    }

    return pBlockTxDB->EraseTxIndex(hash);
}

bool CTxDB::ContainsTx(uint256 hash)
{
    CTxIndex txindex;
    return ReadTxIndex(hash,txindex);
}

bool CTxDB::ReadDiskTx(uint256 hash, CTransaction& tx, CTxIndex& txindex)
{
    tx.SetNull();
    if (!ReadTxIndex(hash, txindex))
        return false;
    return (tx.ReadFromDisk(txindex.pos));
}

bool CTxDB::ReadDiskTx(uint256 hash, CTransaction& tx)
{
    CTxIndex txindex;
    return ReadDiskTx(hash, tx, txindex);
}

bool CTxDB::ReadDiskTx(COutPoint outpoint, CTransaction& tx, CTxIndex& txindex)
{
    return ReadDiskTx(outpoint.hash, tx, txindex);
}

bool CTxDB::ReadDiskTx(COutPoint outpoint, CTransaction& tx)
{
    CTxIndex txindex;
    return ReadDiskTx(outpoint.hash, tx, txindex);
}

bool CTxDB::WriteBlockIndex(const CDiskBlockIndex& blockindex)
{
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            uint256 hash = blockindex.GetBlockHash();
            mapUpdatedBlockIndex[hash] = blockindex;
            setDeletedBlockIndex.erase(hash);
            return true; 
        }
    }
    return pBlockTxDB->WriteBlockIndex(blockindex);
}

bool CTxDB::EraseBlockIndex(uint256 hash)
{
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            mapUpdatedBlockIndex.erase(hash);
            setDeletedBlockIndex.insert(hash);
            return true; 
        }
    }
    return pBlockTxDB->EraseBlockIndex(hash);
}

bool CTxDB::ReadHashBestChain(uint256& hashBestChain)
{
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            if (hashBestChainUpdated != 0)
            {
                hashBestChain = hashBestChainUpdated;
                return true;
            }
        }
    }
    return pBlockTxDB->ReadHashBestChain(hashBestChain);
}

bool CTxDB::WriteHashBestChain(uint256 hashBestChain)
{
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            hashBestChainUpdated = hashBestChain;
            return true;
        }
    }
    return pBlockTxDB->WriteHashBestChain(hashBestChain);
}

bool CTxDB::ReadBestInvalidTrust(CBigNum& bnBestInvalidTrust)
{
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            if (bnBestInvalidTrustUpdated != 0)
            {
                bnBestInvalidTrust = bnBestInvalidTrustUpdated;
                return true;
            }
        }
    }
    return pBlockTxDB->ReadBestInvalidTrust(bnBestInvalidTrust);
}

bool CTxDB::WriteBestInvalidTrust(CBigNum bnBestInvalidTrust)
{
    {
        LOCK(cs_DBCache);
        if (fTxn)
        {
            bnBestInvalidTrustUpdated = bnBestInvalidTrust;
            return true;
        }
    }
    return pBlockTxDB->WriteBestInvalidTrust(bnBestInvalidTrust);
}

bool CTxDB::ReadSyncCheckpoint(uint256& hashCheckpoint)
{
    return pBlockTxDB->ReadSyncCheckpoint(hashCheckpoint);
}

bool CTxDB::WriteSyncCheckpoint(uint256 hashCheckpoint)
{
    return pBlockTxDB->WriteSyncCheckpoint(hashCheckpoint);
}

bool CTxDB::ReadCheckpointPubKey(string& strPubKey)
{
    return pBlockTxDB->ReadCheckpointPubKey(strPubKey);
}

bool CTxDB::WriteCheckpointPubKey(const string& strPubKey)
{
    return pBlockTxDB->WriteCheckpointPubKey(strPubKey);
}

bool CTxDB::ReadV04UpgradeTime(unsigned int& nUpgradeTime)
{
    return pBlockTxDB->ReadV04UpgradeTime(nUpgradeTime);
}

bool CTxDB::WriteV04UpgradeTime(const unsigned int& nUpgradeTime)
{
    return pBlockTxDB->WriteV04UpgradeTime(nUpgradeTime);
}

bool CTxDB::ReadV05UpgradeTime(unsigned int& nUpgradeTime)
{
    return pBlockTxDB->ReadV05UpgradeTime(nUpgradeTime);
}

bool CTxDB::WriteV05UpgradeTime(const unsigned int& nUpgradeTime)
{
    return pBlockTxDB->WriteV05UpgradeTime(nUpgradeTime);
}

bool CTxDB::LoadBlockIndex()
{
    // Check empty database
    uint256 hashBestChainTest = 0;
    if (!ReadHashBestChain(hashBestChainTest) || hashBestChainTest == 0)
    {
        ImportDB();
    }
    
    if (!pBlockTxDB->LoadDB(*this) || fRequestShutdown)
    {
        return false;
    }

    // Calculate bnChainTrust
    vector<pair<int, CBlockIndex*> > vSortedByHeight;
    vSortedByHeight.reserve(mapBlockIndex.size());
    BOOST_FOREACH(const PAIRTYPE(uint256, CBlockIndex*)& item, mapBlockIndex)
    {
        CBlockIndex* pindex = item.second;
        vSortedByHeight.push_back(make_pair(pindex->nHeight, pindex));
    }
    sort(vSortedByHeight.begin(), vSortedByHeight.end());
    BOOST_FOREACH(const PAIRTYPE(int, CBlockIndex*)& item, vSortedByHeight)
    {
        CBlockIndex* pindex = item.second;
        pindex->bnChainTrust = (pindex->pprev ? pindex->pprev->bnChainTrust : 0) + pindex->GetBlockTrust();
        // lomocoin: calculate stake modifier checksum
        pindex->nStakeModifierChecksum = GetStakeModifierChecksum(pindex);
        if (!CheckStakeModifierCheckpoints(pindex->nHeight, pindex->nStakeModifierChecksum))
            return error("CTxDB::LoadBlockIndex() : Failed stake modifier checkpoint height=%d, modifier=0x%016"PRI64x, pindex->nHeight, pindex->nStakeModifier);
    }

    // Load hashBestChain pointer to end of best chain
    if (!ReadHashBestChain(hashBestChain))
    {
        if (pindexGenesisBlock == NULL)
            return true;
        return error("CTxDB::LoadBlockIndex() : hashBestChain not loaded");
    }

    if (!mapBlockIndex.count(hashBestChain))
        return error("CTxDB::LoadBlockIndex() : hashBestChain not found in the block index");
    pindexBest = mapBlockIndex[hashBestChain];
    nBestHeight = pindexBest->nHeight;
    bnBestChainTrust = pindexBest->bnChainTrust;
    printf("LoadBlockIndex(): hashBestChain=%s  height=%d  trust=%s\n", hashBestChain.ToString().substr(0,20).c_str(), nBestHeight, bnBestChainTrust.ToString().c_str());

    // lomocoin: load hashSyncCheckpoint
    if (!ReadSyncCheckpoint(Checkpoints::hashSyncCheckpoint))
        return error("CTxDB::LoadBlockIndex() : hashSyncCheckpoint not loaded");
    printf("LoadBlockIndex(): synchronized checkpoint %s\n", Checkpoints::hashSyncCheckpoint.ToString().c_str());

    // Load bnBestInvalidTrust, OK if it doesn't exist
    ReadBestInvalidTrust(bnBestInvalidTrust);

    // Verify blocks in the best chain
    int nCheckLevel = GetArg("-checklevel", 1);
    int nCheckDepth = GetArg( "-checkblocks", 2500);
    if (nCheckDepth == 0)
        nCheckDepth = 1000000000; // suffices until the year 19000
    if (nCheckDepth > nBestHeight)
        nCheckDepth = nBestHeight;
    printf("Verifying last %i blocks at level %i\n", nCheckDepth, nCheckLevel);
    CBlockIndex* pindexFork = NULL;
    map<pair<unsigned int, unsigned int>, CBlockIndex*> mapBlockPos;
    for (CBlockIndex* pindex = pindexBest; pindex && pindex->pprev; pindex = pindex->pprev)
    {
        if (pindex->nHeight < nBestHeight-nCheckDepth)
            break;
        CBlock block;
        if (!block.ReadFromDisk(pindex))
            return error("LoadBlockIndex() : block.ReadFromDisk failed");
        // check level 1: verify block validity
        if (nCheckLevel>0 && !block.CheckBlock())
        {
            printf("LoadBlockIndex() : *** found bad block at %d, hash=%s\n", pindex->nHeight, pindex->GetBlockHash().ToString().c_str());
            pindexFork = pindex->pprev;
        }
        // check level 2: verify transaction index validity
        if (nCheckLevel>1)
        {
            pair<unsigned int, unsigned int> pos = make_pair(pindex->nFile, pindex->nBlockPos);
            mapBlockPos[pos] = pindex;
            BOOST_FOREACH(const CTransaction &tx, block.vtx)
            {
                uint256 hashTx = tx.GetHash();
                CTxIndex txindex;
                if (ReadTxIndex(hashTx, txindex))
                {
                    // check level 3: checker transaction hashes
                    if (nCheckLevel>2 || pindex->nFile != txindex.pos.nFile || pindex->nBlockPos != txindex.pos.nBlockPos)
                    {
                        // either an error or a duplicate transaction
                        CTransaction txFound;
                        if (!txFound.ReadFromDisk(txindex.pos))
                        {
                            printf("LoadBlockIndex() : *** cannot read mislocated transaction %s\n", hashTx.ToString().c_str());
                            pindexFork = pindex->pprev;
                        }
                        else
                            if (txFound.GetHash() != hashTx) // not a duplicate tx
                            {
                                printf("LoadBlockIndex(): *** invalid tx position for %s\n", hashTx.ToString().c_str());
                                pindexFork = pindex->pprev;
                            }
                    }
                    // check level 4: check whether spent txouts were spent within the main chain
                    unsigned int nOutput = 0;
                    if (nCheckLevel>3)
                    {
                        BOOST_FOREACH(const CDiskTxPos &txpos, txindex.vSpent)
                        {
                            if (!txpos.IsNull())
                            {
                                pair<unsigned int, unsigned int> posFind = make_pair(txpos.nFile, txpos.nBlockPos);
                                if (!mapBlockPos.count(posFind))
                                {
                                    printf("LoadBlockIndex(): *** found bad spend at %d, hashBlock=%s, hashTx=%s\n", pindex->nHeight, pindex->GetBlockHash().ToString().c_str(), hashTx.ToString().c_str());
                                    pindexFork = pindex->pprev;
                                }
                                // check level 6: check whether spent txouts were spent by a valid transaction that consume them
                                if (nCheckLevel>5)
                                {
                                    CTransaction txSpend;
                                    if (!txSpend.ReadFromDisk(txpos))
                                    {
                                        printf("LoadBlockIndex(): *** cannot read spending transaction of %s:%i from disk\n", hashTx.ToString().c_str(), nOutput);
                                        pindexFork = pindex->pprev;
                                    }
                                    else if (!txSpend.CheckTransaction())
                                    {
                                        printf("LoadBlockIndex(): *** spending transaction of %s:%i is invalid\n", hashTx.ToString().c_str(), nOutput);
                                        pindexFork = pindex->pprev;
                                    }
                                    else
                                    {
                                        bool fFound = false;
                                        BOOST_FOREACH(const CTxIn &txin, txSpend.vin)
                                            if (txin.prevout.hash == hashTx && txin.prevout.n == nOutput)
                                                fFound = true;
                                        if (!fFound)
                                        {
                                            printf("LoadBlockIndex(): *** spending transaction of %s:%i does not spend it\n", hashTx.ToString().c_str(), nOutput);
                                            pindexFork = pindex->pprev;
                                        }
                                    }
                                }
                            }
                            nOutput++;
                        }
                    }
                }
                // check level 5: check whether all prevouts are marked spent
                if (nCheckLevel>4)
                {
                     BOOST_FOREACH(const CTxIn &txin, tx.vin)
                     {
                          CTxIndex txindex;
                          if (ReadTxIndex(txin.prevout.hash, txindex))
                              if (txindex.vSpent.size()-1 < txin.prevout.n || txindex.vSpent[txin.prevout.n].IsNull())
                              {
                                  printf("LoadBlockIndex(): *** found unspent prevout %s:%i in %s\n", txin.prevout.hash.ToString().c_str(), txin.prevout.n, hashTx.ToString().c_str());
                                  pindexFork = pindex->pprev;
                              }
                     }
                }
            }
        }
    }
    if (pindexFork)
    {
        // Reorg back to the fork
        printf("LoadBlockIndex() : *** moving best chain pointer back to block %d\n", pindexFork->nHeight);
        CBlock block;
        if (!block.ReadFromDisk(pindexFork))
            return error("LoadBlockIndex() : block.ReadFromDisk failed");
        CTxDB txdb;
        block.SetBestChain(txdb, pindexFork);
    }

    return true;
}

CBlockIndex static * InsertBlockIndex(uint256 hash)
{
    if (hash == 0)
        return NULL;

    // Return existing
    map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
    if (mi != mapBlockIndex.end())
        return (*mi).second;

    // Create new
    CBlockIndex* pindexNew = new CBlockIndex();
    if (!pindexNew)
        throw runtime_error("LoadBlockIndex() : new CBlockIndex failed");
    mi = mapBlockIndex.insert(make_pair(hash, pindexNew)).first;
    pindexNew->phashBlock = &((*mi).first);

    return pindexNew;
}

bool CTxDB::Handle(CDataStream& ssKey, CDataStream& ssValue)
{
    if (fRequestShutdown)
    {
        return false;
    }

    try
    {
        string strType;
        ssKey >> strType;
        if (strType == "blockindex")
        {
            CDiskBlockIndex diskindex;
            ssValue >> diskindex;

            // Construct block index object
            CBlockIndex* pindexNew = InsertBlockIndex(diskindex.GetBlockHash());
            pindexNew->pprev          = InsertBlockIndex(diskindex.hashPrev);
            pindexNew->pnext          = InsertBlockIndex(diskindex.hashNext);
            pindexNew->nFile          = diskindex.nFile;
            pindexNew->nBlockPos      = diskindex.nBlockPos;
            pindexNew->nHeight        = diskindex.nHeight;
            pindexNew->nMint          = diskindex.nMint;
            pindexNew->nMoneySupply   = diskindex.nMoneySupply;
            pindexNew->nFlags         = diskindex.nFlags;
            pindexNew->nStakeModifier = diskindex.nStakeModifier;
            pindexNew->prevoutStake   = diskindex.prevoutStake;
            pindexNew->nStakeTime     = diskindex.nStakeTime;
            pindexNew->hashProofOfStake = diskindex.hashProofOfStake;
            pindexNew->nVersion       = diskindex.nVersion;
            pindexNew->hashMerkleRoot = diskindex.hashMerkleRoot;
            pindexNew->nTime          = diskindex.nTime;
            pindexNew->nBits          = diskindex.nBits;
            pindexNew->nNonce         = diskindex.nNonce;

            // Watch for genesis block
            if (pindexGenesisBlock == NULL && diskindex.GetBlockHash() == hashGenesisBlock)
                pindexGenesisBlock = pindexNew;

            if (!pindexNew->CheckIndex())
                return error("LoadBlockIndex() : CheckIndex failed at %d", pindexNew->nHeight);

            // lomocoin: build setStakeSeen
            if (pindexNew->IsProofOfStake())
                setStakeSeen.insert(make_pair(pindexNew->prevoutStake, pindexNew->nStakeTime));
        }
    }
    catch (std::exception &e) 
    {
        return error("%s() : deserialize error", __PRETTY_FUNCTION__);
    }

    return true;
}

bool CTxDB::ImportDB()
{
    try
    {
        boost::filesystem::path path = GetDataDir() / "blkindex.dat";
        if (boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path))
        {
            CTxDB_BDB bdb;
            return bdb.ImportTo(*pBlockTxDB);
        }
    }
    catch (...)
    {
    }
    return false;
}

