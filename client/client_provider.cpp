#include "client_provider.h"

#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iomanip>

namespace coding_challenge
{
    class ClientProviderImpl : public IClientProvider
    {
        public:
            ClientProviderImpl(const std::string& source) : file_stream_(source, std::ifstream::in)
            {
            }

            ClientInfo GetNextClient() override
            {
                auto line = nextLine();
                auto split = extractNameAndTime(line);
                std::istringstream timeStream(split.second);
                std::tm t = {};
                timeStream >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
                auto timestamp = static_cast<int>(mktime(&t));
                return { split.first, timestamp };
            }

        private:
            std::ifstream file_stream_;

            std::string nextLine()
            {
                std::string result;
                std::getline(file_stream_, result);
                result.pop_back();
                return result;
            }

            std::pair<std::string, std::string> extractNameAndTime(const std::string& src) 
            {
                const size_t TimeLength = 21;
                auto pivot = src.size() - TimeLength;
                auto name = src.substr(0, pivot + 1);
                auto timestamp = src.substr(pivot + 1);
                return { name, timestamp };
            }
    };

    ClientProvider::ClientProvider(const std::string& source) : impl_(std::make_unique<ClientProviderImpl>(source))
    {
    }

    ClientInfo ClientProvider::GetNextClient()
    {
        return impl_->GetNextClient();
    }
}