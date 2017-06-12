// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2016-2018 The LoMoCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef  LOMOCOIN_WALLETBACKUP_H
#define  LOMOCOIN_WALLETBACKUP_H

#include "keystore.h"
#include "serialize.h"
#include "script.h"

class CWalletBackup : public CCryptoKeyStore
{
public:
    bool Import(const SecureString& strWalletPassphrase,CDataStream& ssBuffer);
    bool Export(const SecureString& strWalletPassphrase,CDataStream& ssBuffer);
protected:
    virtual bool LoadBuffer(CDataStream& ssBuffer);
    bool SaveBuffer(CDataStream& ssBuffer);
    bool Encrypt(const SecureString& strWalletPassphrase);
    bool Unlock(const SecureString& strWalletPassphrase);
public:
    std::map<std::string,std::string> mapAddressBook;
    std::map<std::string,std::string> mapAccount;
protected:
    CMasterKey kMasterKey;
};

#endif //LOMOCOIN_WALLETBACKUP_H
