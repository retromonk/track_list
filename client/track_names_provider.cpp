#include "track_names_provider.h"

#include <fstream>
#include <algorithm>

namespace coding_challenge
{
    class TrackNamesProviderImpl : public ITrackNamesProvider
    {
    public:
        TrackNamesProviderImpl(const std::string &source) : file_stream_(source, std::ifstream::in)
        {
            track_count_ = std::count(std::istreambuf_iterator<char>(file_stream_),
                                      std::istreambuf_iterator<char>(), '\n');
        }

        std::string GetTrack(int index) override
        {
            file_stream_.seekg(0, std::ios::beg);
            std::string result;
            for (auto i = 0; i < index; ++i) 
            {
                std::getline(file_stream_, result);
            }

            result.pop_back();
            return result;
        }

        int Count() const override
        {
            return track_count_;
        }

    private:
        std::ifstream file_stream_;
        int track_count_;
    };

    TrackNamesProvider::TrackNamesProvider(const std::string &source) : impl_(std::make_unique<TrackNamesProviderImpl>(source))
    {
    }

    std::string TrackNamesProvider::GetTrack(int index)
    {
        return impl_->GetTrack(index);
    }

    int TrackNamesProvider::Count() const
    {
        return impl_->Count();
    }
} // namespace coding_challenge