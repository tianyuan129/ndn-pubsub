
#include "subscriber.hpp"
#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces can be used to prevent/limit name conflicts
namespace pubsub {

class Consumer : noncopyable
{
public:
  Consumer() : m_subscriber(m_face) {}

  void
  run();

private:
  void
  onData(const Data& data)
  {
    std::cout << data << std::endl;
  }

  void
  onNack(const Interest& interest, const lp::Nack& nack)
  {
    std::cout << "received Nack with reason " << nack.getReason()
              << " for interest " << interest << std::endl;
  }

  void
  onTimeout(const Interest& interest)
  {
    std::cout << "Timeout " << interest << std::endl;
  }

private:
  Face m_face;
  Subscriber m_subscriber;
};

void
Consumer::run()
{
    m_subscriber.addSubscription(Name("/example/testApp"), bind(&Consumer::onData, this, _1));
    m_subscriber.sendSubInterest();

    // processEvents will block until the requested data received or timeout occurs
    m_face.processEvents();
  }

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  ndn::pubsub::Consumer consumer;
  try {
    consumer.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
