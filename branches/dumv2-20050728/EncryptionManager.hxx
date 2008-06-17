#if !defined(RESIP_ENCRYPTIONMANAGER_HXX)
#define RESIP_ENCRYPTIONMANAGER_HXX

#include "resiprocate/os/Data.hxx"
#include "resiprocate/os/BaseException.hxx"
#include "resiprocate/SipMessage.hxx"
#include "resiprocate/Contents.hxx"
#include "resiprocate/Security.hxx"
#include "resiprocate/dum/DialogUsageManager.hxx"
#include "resiprocate/Helper.hxx"
#include "resiprocate/dum/CertMessage.hxx"
#include "resiprocate/dum/RemoteCertStore.hxx"
#include "resiprocate/dum/DumFeature.hxx"

namespace resip
{

class EncryptionManager : public DumFeature
{
   public:
      class Exception : public BaseException
      {
         public:
            Exception(const Data& msg, const Data& file, const int line);
            const char* name() const { return "EncryptionManagerException"; }
      };

      EncryptionManager(DialogUsageManager& dum);
      virtual ~EncryptionManager();
      void setRemoteCertStore(std::auto_ptr<RemoteCertStore> store);
      DumFeature::ProcessingResult process(Message* msg);

   private:

      typedef enum
      {
         Pending,
         Complete
      } Result;

      EncryptionManager::Result processCertMessage(const CertMessage& cert);
      Contents* sign(const SipMessage& msg, const Data& senderAor);
      Contents* encrypt(const SipMessage& msg, const Data& recipientAor);
      Contents* signAndEncrypt(const SipMessage& mg, const Data& senderAor, const Data& recipientAor);
      bool decrypt(SipMessage& msg);

      class Request
      {
         public:
            Request(DialogUsageManager& dum, RemoteCertStore* store, const SipMessage& msg);
            virtual ~Request();
            virtual Result received(bool success, MessageId::Type type, const Data& aor, const Data& data) = 0;
            Data getId() const { return mMsg.getTransactionId(); }

         protected:
            DialogUsageManager& mDum;
            RemoteCertStore* mStore;
            SipMessage mMsg; // initial message.
            int mPendingRequests;

            void response415();
      };

      class Sign : public Request
      {
         public:
            Sign(DialogUsageManager& dum, RemoteCertStore* store, const SipMessage& msg, const Data& senderAor);
            ~Sign();
            Result received(bool success, MessageId::Type type, const Data& aor, const Data& data);
            bool sign(Contents**);

         protected:
            Data mSenderAor;
      };

      class Encrypt : public Request
      {
         public:
            Encrypt(DialogUsageManager& dum, RemoteCertStore* store, const SipMessage& msg, const Data& recipientAor);
            ~Encrypt();
            Result received(bool success, MessageId::Type type, const Data& aor, const Data& data);
            bool encrypt(Contents**);

         protected:
            Data mRecipientAor;
      };

      class SignAndEncrypt : public Request
      {
         public:
            SignAndEncrypt(DialogUsageManager& dum, RemoteCertStore* store, const SipMessage& msg,  const Data& senderAor, const Data& recipientAor);
            ~SignAndEncrypt();
            Result received(bool success, MessageId::Type type, const Data& aor, const Data& data);
            bool signAndEncrypt(Contents**);

         protected:
            Data mSenderAor;
            Data mRecipientAor;

         private:
            Contents* doWork();
      };

      class Decrypt : public Request
      {
         public:
            Decrypt(DialogUsageManager& dum, RemoteCertStore* store, const SipMessage& msg);
            ~Decrypt();
            Result received(bool success, MessageId::Type type, const Data& aor, const Data& data);
            bool decrypt(Helper::ContentsSecAttrs& csa);

         private:
            std::auto_ptr<Contents> mContents;
            bool isEncrypted();
            bool isSigned();
            bool isEncryptedRecurse(Contents*);
            bool isSignedRecurse(Contents*, const Data& decryptorAor);
            Helper::ContentsSecAttrs getContents(const SipMessage& msg, Security& security, bool noDecryptionKey);
            Contents* getContentsRecurse(Contents*, Security&, bool, SecurityAttributes* attr);
            Data mDecryptor;
            Data mSigner;
      };

      //DialogUsageManager* mDum;
      UInt32 mCounter;
      std::auto_ptr<RemoteCertStore> mRemoteCertStore;

      typedef std::list<Request*> RequestList;
      RequestList mRequests;
};

}

#endif

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */