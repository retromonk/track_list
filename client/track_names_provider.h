#ifndef CODING_CHALLENGE_TRACK_NAMES_PROVIDER
#define CODING_CHALLENGE_TRACK_NAMES_PROVIDER
#include <memory>
#include <string>

namespace coding_challenge 
{
    class ITrackNamesProvider 
    {
        public:
            virtual ~ITrackNamesProvider() = default;
            virtual std::string GetTrack(int index) = 0;
            virtual int Count() const = 0;
    };

    class TrackNamesProvider : public ITrackNamesProvider
    {
        public:
            TrackNamesProvider(const std::string& source);
            std::string GetTrack(int index) override;
            int Count() const override;

        private:
            std::unique_ptr<ITrackNamesProvider> impl_;
    };
}

#endif