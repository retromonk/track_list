#include "udp_client.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <vector>

namespace coding_challenge
{
    class UDPClientImpl : public IUDPClient
    {
    public:
        UDPClientImpl(std::string hostname, int port) : hostname_(std::move(hostname)), port_(port)
        {
        }

        ~UDPClientImpl() override
        {
            if (fd_ > 0)
            {
                close(fd_);
            }
        }

        bool Connect() override
        {
            if (fd_ > 0)
            {
                return true;
            }

            if ((fd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                perror("In socket");
                return false;
            }

            struct sockaddr_in address;
            int addrlen = sizeof(address);
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port_);
            memset(address.sin_zero, '\0', sizeof(address.sin_zero));
            if (inet_pton(AF_INET, hostname_.c_str(), &address.sin_addr) <= 0)
            {
                perror("Invalid address");
                return false;
            }

            if (connect(fd_, (struct sockaddr *)&address, sizeof(address)) < 0)
            {
                perror("Failed to connect to server");
                return false;
            }

            return true;
        }

        bool SendString(const std::string &output)
        {
            if (fd_ <= 0) 
            {
                return false;
            }
            
            send(fd_, output.c_str(), output.size(), 0);
            return true;
        }

    private:
        std::string hostname_;
        int port_;
        int fd_ = -1;
    };

    UDPClient::UDPClient(const std::string &hostname, int port) : impl_(std::make_unique<UDPClientImpl>(hostname, port))
    {
    }

    bool UDPClient::Connect()
    {
        return impl_->Connect();
    }

    bool UDPClient::SendString(const std::string &output)
    {
        return impl_->SendString(output);
    }
} // namespace coding_challenge