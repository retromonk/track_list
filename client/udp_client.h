#ifndef CODING_CHALLENGE_UDP_CLIENT
#define CODING_CHALLENGE_UDP_CLIENT

#include <string>
#include <memory>

namespace coding_challenge
{
    class IUDPClient {
        public:
            virtual ~IUDPClient() = default;
            virtual bool Connect() = 0;
            virtual bool SendString(const std::string& output) = 0;
    };

    class UDPClient : public IUDPClient {
        public:
            UDPClient(const std::string& ipAddress, int port);
            bool Connect() override;
            bool SendString(const std::string& output) override;

        private:
            std::unique_ptr<IUDPClient> impl_;
    };
}

#endif