#include "device_provider.h"

#include <algorithm>
#include <fstream>
#include <regex>
#include <iostream>

namespace coding_challenge
{
    class DeviceProviderImpl : public IDeviceProvider
    {
    public:
        DeviceProviderImpl(const std::string &source) : file_stream_(source, std::ifstream::in), extract_regex_("^(.*) ([0-9]*)$")
        {
        }

        std::string GetDeviceIdFromName(const std::string &name) override
        {
            std::string sanitized;
            std::copy_if(name.begin(), name.end(), std::back_inserter(sanitized), [](char cur) { return cur != '(' && cur != ')'; });
            file_stream_.seekg(0, std::ios::beg);
            std::string line;
            while (std::getline(file_stream_, line))
            {
                line.pop_back();
                auto split = getNameAndDeviceId(line);
                if (sanitized == split.first)
                {
                    return split.second;
                }
            }

            return "";
        }

    private:
        std::ifstream file_stream_;
        std::regex extract_regex_;
        std::pair<std::string, std::string> getNameAndDeviceId(const std::string& src)
        {
            std::smatch match;
            if (std::regex_match(src, match, extract_regex_))
            {
                return { match[1].str(), match[2].str() };
            }

            return {};
        }
    };

    DeviceProvider::DeviceProvider(const std::string &source) : impl_(std::make_unique<DeviceProviderImpl>(source))
    {
    }

    std::string DeviceProvider::GetDeviceIdFromName(const std::string &name)
    {
        return impl_->GetDeviceIdFromName(name);
    }
} // namespace coding_challenge