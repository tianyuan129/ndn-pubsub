// 2019 Tianyuan Yu

#ifndef ndn_SUBSCRIBER_HPP
#define ndn_SUBSCRIBER_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/segment-fetcher.hpp>
#include <ndn-cxx/util/time.hpp>

#include <map>
#include <vector>

namespace ndn {

using namespace ndn::literals::time_literals;

typedef std::function<void(const ndn::Data&)> ReceiveDataCallback;

const ndn::time::milliseconds SUB_INTEREST_LIFETIME = 1_s;

class Subscriber
{
public:
  
  Subscriber(ndn::Face& face, ndn::time::milliseconds subInterestLifetime = SUB_INTEREST_LIFETIME);
  
  void
  sendSubInterest();

  bool
  addSubscription(const ndn::Name& subPrefix, const ReceiveDataCallback& onReceiveData);

/*
  ndn::optional<uint64_t>
  getSeqNo(const ndn::Name& prefix) const
  {
    auto it = m_prefixes.find(prefix);
    if (it == m_prefixes.end()) {
      return ndn::nullopt;
    }
    return it->second;
  }
*/

  void
  stop();

private:

  void
  onSubData(const ndn::Interest& subInterest, const ndn::Data& Data);

  void 
  onSubNack(const ndn::Interest& subInterest, const ndn::lp::Nack& Nack);

  void 
  onSubTimeout(const ndn::Interest& subInterest);

private:
  ndn::Face& m_face;
  ndn::Scheduler m_scheduler;


  ndn::time::milliseconds m_subInterestLifetime;

  // Store sequence number for the prefix.
  std::map<ndn::Name, ReceiveDataCallback> m_subCallbacks;

  ndn::random::RandomNumberEngine& m_rng;
  std::uniform_int_distribution<> m_rangeUniformRandom;
};

} // namespace ndn

#endif // ndn_SUBSCRIBER_HPP
