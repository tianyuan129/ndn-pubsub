
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include "publisher.hpp"
#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces can be used to prevent/limit name conflicts
namespace examples {

class Producer : noncopyable
{
public:
  Producer() : m_publisher(m_face) {}

  void
  run()
  {
    Name dataName("/example/testApp/randomData");
    dataName
      .append("yeeees") // add "testApp" component to Interest name
      .appendVersion();  // add "version" component (current UNIX timestamp in milliseconds)

    static const std::string content = "HELLO KITTY";

    // Create Data packet
    shared_ptr<Data> data = make_shared<Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(10_s); // 10 seconds
    data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());

    // Sign Data packet with default identity
    m_keyChain.sign(*data);

    // Return Data packet to the requester 
    m_publisher.addPublication(Name("/example/testApp"), *data);
    m_face.processEvents();
  }

private:

  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix \""
              << prefix << "\" in local hub's daemon (" << reason << ")"
              << std::endl;
    m_face.shutdown();
  }

private:
  Face m_face;
  Publisher m_publisher;
  KeyChain m_keyChain;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  ndn::examples::Producer producer;
  try {
    producer.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
