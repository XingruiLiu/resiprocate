#ifndef RESIP_StatisticsManager_hxx
#define RESIP_StatisticsManager_hxx

#include "resiprocate/os/Timer.hxx"
#include "resiprocate/os/Data.hxx"
#include "resiprocate/StatisticsMessage.hxx"

// !dlb! part of the build script
#define RESIP_STATISTICS(_x) _x
//#define RESIP_STATISTICS(_x)

namespace resip
{
class SipStack;
class SipMessage;
class TransactionController;

class StatisticsManager : public StatisticsMessage::Payload
{
   public:
      // not implemented
      typedef enum
      {
         TransportFifoSize,
         TUFifoSize,
         ActiveTimers,
         OpenTcpConnections,
         ActiveClientTransactions,
         ActiveServerTransactions,
         PendingDnsQueries,
         StatsMemUsed
      } Measurement;
      
      StatisticsManager(SipStack& stack, unsigned long intervalSecs=60);

      void process();
      // not stricly thread-safe; needs to be called through the fifo somehow
      void setInterval(unsigned long intvSecs);

   private:
      friend class TransactionState;
      bool sent(SipMessage* msg, bool retrans);
      bool received(SipMessage* msg);

      void poll(); // force an update

      SipStack& mStack;
      UInt64 mInterval;
      UInt64 mNextPoll;
};

}

#endif

