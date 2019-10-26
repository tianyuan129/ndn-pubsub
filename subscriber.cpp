// 2019 Tianyuan Yu

#include "subscriber.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/security/validator-null.hpp>

#include <boost/algorithm/string.hpp>

namespace ndnd {

NDN_LOG_INIT(ndnd.Subscriber);

Subscriber::Subscriber(const ndn::Name& subPrefix,
                      ndn::Face& face,
                      const ReceiveDataCallback& onReceiveData,
                      unsigned int count,
                      ndn::time::milliseconds subInterestLifetime = SUB_INTEREST_LIFETIME)
 : m_face(face)
 , m_scheduler(m_face.getIoService())
 , m_subPrefix(subPrefix)
 , m_subInterestPrefix(ndn::Name(m_subPrefix).append("hello")) //TODO:
 , m_onReceiveData(onReceiveData)
 , m_subInterestLifetime(subInterestLifetime)
 , m_rng(ndn::random::getRandomNumberEngine())
 , m_rangeUniformRandom(100, 500)
{
}

bool
Subscriber::addSubscription(const ndn::Name& prefix, const ReceiveDataCallback& onReceiveData)
{
  auto it = m_subCallbacks.insert(std::pair<ndn::Name, ReceiveDataCallback>(prefix, onReceiveData));
  if (!it.second) {
    return false;
  }
  return true;
}

void
Subscriber::stop()
{
  m_scheduler.cancelAllEvents();

  if (m_subFetcher) {
    m_subFetcher->stop();
    m_subFetcher.reset();
  }
}

void
Subscriber::sendSubInterest()
{
  ndn::Name subInterestName(m_subInterestPrefix);


  ndn::Interest subInterest(subInterestName);

  NDN_LOG_DEBUG("sendSubInterest, nonce: " << subInterest.getNonce() <<
                " hash: " << std::hash<std::string>{}(subInterest.getName().toUri()));

  m_face.expressInterest(subInterest, bind(&Subscriber::onSubData, this, _1, _2),
                                      bind(&Subscriber::onSubNack, this, _1, _2),
                                      bind(&Subscriber::onSubTimeout, this, _1));
}

void
Subscriber::onSubData(const ndn::Interest& subInterest, const ndn::Data& Data)
{
    NDN_LOG_DEBUG("Subscribed Data: " << Data);

    auto iter = m_subCallbacks.find(subInterest.getName());
    if (iter != m_subCallbacks.end()) {
      NDN_LOG_DEBUG("Triggered Callback: " << iter->first);
      iter->second(Data);
    }

  //sendSubInterest();
}

void 
Subscriber::onSubNack(const ndn::Interest& subInterest, const ndn::lp::Nack& Nack)
{
  NDN_LOG_DEBUG("received Nack with reason " << Nack.getReason()
                 << " for interest " << subInterest << std::endl);
}

void
Subscriber::onSubTimeout(const ndn::Interest& subInterest)
{
   NDN_LOG_DEBUG("Timeout " << subInterest << std::endl);
}


} //namespace ndnd