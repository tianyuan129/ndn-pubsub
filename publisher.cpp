// 2019 Tianyuan Yu

#include "publisher.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/security/validator-null.hpp>

#include <boost/algorithm/string.hpp>

namespace ndn {

NDN_LOG_INIT(ndn.Publisher);

Publisher::Publisher(ndn::Face& face)
 : m_face(face)
{}

void
Publisher::registerPrefix(const ndn::Name& pubPrefix)
{
   auto prefixId = m_face.setInterestFilter(ndn::InterestFilter(pubPrefix),
                                            bind(&Publisher::onPubInterest, this, _2), nullptr);  
   NDN_LOG_DEBUG("register prefix: " << pubPrefix); 
}

bool
Publisher::addPublication(const ndn::Name& pubPrefix, const ndn::Data& Data)
{
  auto it = m_pubList.insert(std::pair<ndn::Name, ndn::Data>(pubPrefix, Data));
  if (!it.second) {
    return false;
  }

  // registe prefix
  registerPrefix(pubPrefix);
  return true;
}

void
Publisher::onPubInterest(const ndn::Interest& interest)
{
  NDN_LOG_DEBUG("Pub Interest: " << interest);

    auto iter = m_pubList.find(interest.getName());
    if (iter != m_pubList.end()) {
        NDN_LOG_DEBUG("Triggered Callback: " << iter->first);
        m_face.put(iter->second);
        NDN_LOG_DEBUG("Pub Data: " << iter->second);
    }
}


} //namespace ndn