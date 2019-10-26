// 2019 Tianyuan Yu

#include "subscriber.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/security/validator-null.hpp>

#include <boost/algorithm/string.hpp>

namespace ndn {

NDN_LOG_INIT(ndn.Subscriber);

Subscriber::Subscriber(ndn::Face& face,
                       ndn::time::milliseconds subInterestLifetime)
 : m_face(face)
 , m_scheduler(m_face.getIoService())
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
Subscriber::sendSubInterest()
{
  auto iter = m_subCallbacks.begin();
  while (iter != m_subCallbacks.end()) {
    ndn::Interest subInterest(iter->first);
    m_face.expressInterest(subInterest, bind(&Subscriber::onSubData, this, _1, _2),
                                    bind(&Subscriber::onSubNack, this, _1, _2),
                                    bind(&Subscriber::onSubTimeout, this, _1));
    NDN_LOG_DEBUG("sendSubInterest, nonce: " << subInterest.getNonce() <<
                " hash: " << std::hash<std::string>{}(subInterest.getName().toUri()));
  }

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


} //namespace ndn