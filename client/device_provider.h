#ifndef CODING_CHALLENGE_DEVICE_PROVIDER
#define CODING_CHALLENGE_DEVICE_PROVIDER

#include <string>
#include <memory>

namespace coding_challenge
{
    class IDeviceProvider
    {
    public:
        virtual ~IDeviceProvider() = default;
        virtual std::string GetDeviceIdFromName(const std::string& name) = 0;
    };

    class DeviceProvider : public IDeviceProvider
    {
    public:
        DeviceProvider(const std::string &source);
        std::string GetDeviceIdFromName(const std::string& name) override;

    private:
        std::unique_ptr<IDeviceProvider> impl_;
    };

} // namespace coding_challenge

#endif