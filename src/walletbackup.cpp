// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2016-2018 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "walletbackup.h"
#include "wallet.h"
#include "base58.h"
#include <openssl/rand.h>
using namespace std;

///////////////////////////////
// CWalletBackup

bool CWalletBackup::Import(const SecureString& strWalletPassphrase,CDataStream& ssBuffer)
{
    if (!LoadBuffer(ssBuffer))
    {
        return false;
    }
    if (IsLocked() && !Unlock(strWalletPassphrase))
    {
        return false;
    }
    return true;
}

bool CWalletBackup::Export(const SecureString& strWalletPassphrase,CDataStream& ssBuffer)
{
    if (!IsCrypted() && !Encrypt(strWalletPassphrase))
    {
        return false;
    }

    if (!IsLocked())
    {
        Lock();
    }

    if (!Unlock(strWalletPassphrase))
    {
        return false;
    }

    return SaveBuffer(ssBuffer);
}

bool CWalletBackup::LoadBuffer(CDataStream& ssBuffer)
{
    try
    {
        while(!ssBuffer.empty())
        {
            string strType;
            ssBuffer >> strType;
            if (strType == "key")
            {
                vector<unsigned char> vchPubKey;
                CPrivKey pkey;
                ssBuffer >> vchPubKey >> pkey;
                CKey key;
                key.SetPubKey(vchPubKey);
                key.SetPrivKey(pkey);
                if (key.GetPubKey() != vchPubKey || !key.IsValid()
                    || !AddKey(key))
                {
                    return false;
                }
            }
            else if (strType == "wkey")
            {
                vector<unsigned char> vchPubKey;
                CWalletKey wkey;
                ssBuffer >> vchPubKey >> wkey;
                CKey key;
                key.SetPubKey(vchPubKey);
                key.SetPrivKey(wkey.vchPrivKey);
                if (key.GetPubKey() != vchPubKey || !key.IsValid()
                    || !AddKey(key))
                {
                    return false;
                }
            }
            else if (strType == "ckey")
            {
                vector<unsigned char> vchPubKey;
                vector<unsigned char> vchCryptedSecret;
                ssBuffer >> vchPubKey >> vchCryptedSecret;
                if (!AddCryptedKey(vchPubKey,vchCryptedSecret))
                {
                    return false;
                }
            }
            else if (strType == "mkey")
            {
                ssBuffer >> kMasterKey;
            }
            else if (strType == "cscript")
            {
                uint160 hash;
                CScript script;
                ssBuffer >> hash >> script;
                if (!AddCScript(script))
                {
                    return false;
                }
            }
            else if (strType == "name")
            {
                string strAddress,strName;
                ssBuffer >> strAddress >> strName;
                mapAddressBook.insert(make_pair(strName,strAddress));
            }
            else if (strType == "acc")
            {
                string strAccount;
                CAccount account;
                ssBuffer >> strAccount >> account;
                CBitcoinAddress address(account.vchPubKey.GetID());
                if (address.IsValid() && !strAccount.empty())
                {
                    mapAccount.insert(make_pair(address.ToString(),strAccount));
                }
            }
            else
            {
                return false;
            }
        }
        for (map<string,string>::iterator it = mapAccount.begin();it != mapAccount.end();)
        {
            CBitcoinAddress address((*it).first);
            
            if (address.IsScript())
            {
                CScriptID scriptID;
                address.GetScriptID(scriptID);
                if (!HaveCScript(scriptID))
                {
                    mapAccount.erase(it++);
                    continue;
                }
            }
            else
            {
                CKeyID keyID;
                address.GetKeyID(keyID);
                if (!HaveKey(keyID))
                {
                    mapAccount.erase(it++);
                    continue;
                }
            }
            ++it;
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CWalletBackup::SaveBuffer(CDataStream& ssBuffer)
{
    if (!IsCrypted())
    {
        return false;
    }

    ssBuffer << string("mkey") << kMasterKey;
    set<CKeyID> setKeys;
    GetKeys(setKeys);
    BOOST_FOREACH(const CKeyID& keyID,setKeys)
    {
        CPubKey vchPubKey;
        vector<unsigned char> vchCryptedSecret;
        if (!GetPubKey(keyID,vchPubKey) || ! GetCryptedSecret(keyID,vchCryptedSecret))
        {
            return false;
        }
        ssBuffer << string("ckey") << vchPubKey.Raw() << vchCryptedSecret;
    }

    set<CScriptID> setCScripts;
    GetCScripts(setCScripts);
    BOOST_FOREACH(const CScriptID& scriptID,setCScripts)
    {
        CScript script;
        if (!GetCScript(scriptID,script))
        {
            return false;
        }
        ssBuffer << string("cscript") << (uint160)scriptID << script;
    }

    BOOST_FOREACH(const PAIRTYPE(string,string)& p,mapAddressBook)
    {
        ssBuffer << string("label") << p;
    }

    BOOST_FOREACH(const PAIRTYPE(string,string)& p,mapAccount)
    {
        ssBuffer << string("acc") << p;
    }
    return true;
}

bool CWalletBackup::Encrypt(const SecureString& strWalletPassphrase)
{
    if (IsCrypted())
    {
        return false;
    }

    CKeyingMaterial vMasterKey;
    RandAddSeedPerfmon();

    vMasterKey.resize(WALLET_CRYPTO_KEY_SIZE);
    RAND_bytes(&vMasterKey[0], WALLET_CRYPTO_KEY_SIZE);

    RandAddSeedPerfmon();
    kMasterKey.vchSalt.resize(WALLET_CRYPTO_SALT_SIZE);
    RAND_bytes(&kMasterKey.vchSalt[0], WALLET_CRYPTO_SALT_SIZE);

    CCrypter crypter;
    kMasterKey.nDeriveIterations = 25000;
    int64 nStartTime = GetTimeMillis();
    crypter.SetKeyFromPassphrase(strWalletPassphrase, kMasterKey);

    kMasterKey.nDeriveIterations = 2500000 / ((double)(GetTimeMillis() - nStartTime));
    nStartTime = GetTimeMillis();
    crypter.SetKeyFromPassphrase(strWalletPassphrase, kMasterKey);

    kMasterKey.nDeriveIterations = (kMasterKey.nDeriveIterations + kMasterKey.nDeriveIterations * 100 / ((double)(GetTimeMillis() - nStartTime))) / 2;
    if (kMasterKey.nDeriveIterations < 25000)
    {
        kMasterKey.nDeriveIterations = 25000;
    }

    if (!crypter.SetKeyFromPassphrase(strWalletPassphrase, kMasterKey)
        || !crypter.Encrypt(vMasterKey, kMasterKey.vchCryptedKey))
    {
        return false;
    }

    return EncryptKeys(vMasterKey);
}

bool CWalletBackup::Unlock(const SecureString& strWalletPassphrase)
{
    if (!IsLocked())
    {
        return false;
    }

    CCrypter crypter;
    CKeyingMaterial vMasterKey;

    if (!crypter.SetKeyFromPassphrase(strWalletPassphrase, kMasterKey)
        || !crypter.Decrypt(kMasterKey.vchCryptedKey, vMasterKey)
        || !CCryptoKeyStore::Unlock(vMasterKey))
    {
        return false;
    }

    return true;
}

