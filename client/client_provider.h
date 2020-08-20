#ifndef CODING_CHALLENGE_CLIENT_PROVIDER
#define CODING_CHALLENGE_CLIENT_PROVIDER

#include <string>
#include <memory>

namespace coding_challenge
{
    struct ClientInfo 
    {
        std::string ClientName;
        int Timestamp;
    };

    class IClientProvider 
    {
        public:
            virtual ~IClientProvider() = default;
            virtual ClientInfo GetNextClient() = 0;
    };

    class ClientProvider : public IClientProvider 
    {
        public:
            ClientProvider(const std::string& source);
            ClientInfo GetNextClient() override;

        private:
            std::unique_ptr<IClientProvider> impl_;
    };
}

#endif