// 2019 Tianyuan Yu

#ifndef ndn_PUBLISHER_HPP
#define ndn_PUBLISHER_HPP

#include <map>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/time.hpp>

namespace ndn {

class Publisher
{
public:

  Publisher(ndn::Face& face);

  void
  publish(const ndn::Name& pubPrefix, ndn::Data& Data);

  bool
  addPublication(const ndn::Name& pubPrefix, const ndn::Data& Data);

private:

  void
  registerPrefix(const ndn::Name& pubprefix);

private:

  void
  onPubInterest(const ndn::Interest& interest);


private:
  ndn::Face& m_face;
  
  // Store sequence number for the prefix.
  std::map<ndn::Name, ndn::Data> m_pubList;
};

} // namespace psync

#endif // PSYNC_PARTIAL_PRODUCER_HPP
