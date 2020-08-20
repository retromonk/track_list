#include "udp_client.h"
#include "client_provider.h"
#include "track_names_provider.h"
#include "device_provider.h"

#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <random>
#include <sstream>
#include <iostream>

static const int16_t PORT = 5000;
const char *CLIENTFILEPATH = "../../ClientNames.txt";
const char *TRACKFILEPATH = "../../TrackNames.txt";
const char *DEVICEFILEPATH = "../../DeviceIDs.txt";

using namespace coding_challenge;

struct Data
{
    ClientInfo Client;
    std::vector<std::string> TrackList;
};

std::ostream &operator<<(std::ostream &os, const Data &data)
{
    os << "{";
    os << "\n";
    os << "\"";
    os << "client";
    os << "\"";
    os << ": ";
    os << "\"";
    os << data.Client.ClientName;
    os << "\"";
    os << ",";
    os << "\n";
    os << "\"";
    os << "timestamp";
    os << "\"";
    os << ": ";
    os << data.Client.Timestamp;
    os << ", ";
    os << "\n";
    os << "\"";
    os << "tracks";
    os << "\"";
    os << ": ";
    os << "[";
    for (auto i = 0UL; i < data.TrackList.size(); ++i)
    {
        if (i > 0)
        {
            os << ", ";
        }

        os << "\"";
        os << data.TrackList.at(i);
        os << "\"";
    }
    os << "]";
    os << "\n";
    os << "}";
    os << "\n";

    return os;
}

std::mutex mutex;
std::condition_variable conditionVariable;
Data data = {};
auto hasNewData = false;
auto isComplete = false;

void reader_thread()
{
    ClientProvider clientProvider(CLIENTFILEPATH);
    TrackNamesProvider trackProvider(TRACKFILEPATH);

    std::random_device randomDevice;
    std::default_random_engine randomEngine(randomDevice());
    std::uniform_int_distribution<> clientCountDistribution(1, 10);
    std::uniform_int_distribution<> trackCountDistribution(3, 6);
    std::uniform_int_distribution<> randomTrackIndexDistribution(1, trackProvider.Count());
    auto clientCount = clientCountDistribution(randomEngine);
    for (auto i = 0; i < clientCount; ++i)
    {
        // read a client name + timestamp from file #1
        auto info = clientProvider.GetNextClient();

        // read a random number (3 - 6) tracknames from file #2
        std::vector<std::string> tracks;
        auto trackCount = trackCountDistribution(randomEngine);
        for (auto j = 0; j < trackCount; ++j)
        {
            auto nextTrackIndex = randomTrackIndexDistribution(randomEngine);
            auto nextTrack = trackProvider.GetTrack(nextTrackIndex);
            tracks.emplace_back(nextTrack);
        }

        // send resulting info to worker thread
        std::unique_lock<std::mutex> l(mutex);
        conditionVariable.wait(l, [] { return !hasNewData; });
        data = {info, tracks};
        hasNewData = true;
        l.unlock();
        conditionVariable.notify_one();
    } // repeat random number of times (1 - 10)

    // send completed indication to worker thread
    std::unique_lock<std::mutex> l(mutex);
    conditionVariable.wait(l, [] { return !hasNewData; });
    isComplete = true;
    l.unlock();
    conditionVariable.notify_one();
}

void worker_thread()
{
    DeviceProvider deviceProvider(DEVICEFILEPATH);
    std::vector<Data> dataCollection;
    while (true)
    {
        std::unique_lock<std::mutex> l(mutex);
        conditionVariable.wait(l, [] { return hasNewData || isComplete; });
        if (hasNewData)
        {
            // The Worker Thread, upon receiving a set of data consisting of "Client Name" + "Timestamp" + <variable list of track names>, will perform a lookup of that client name in file # 3, the "DeviceIDs.txt" file
            auto deviceId = deviceProvider.GetDeviceIdFromName(data.Client.ClientName);
            // The Worker Thread will concatenate the Client Name and Device ID into an overall identifier of "<client name> + <device ID>"
            std::stringstream oss;
            oss << data.Client.ClientName << deviceId;
            data.Client.ClientName = oss.str();
            // The Worker Thread will retain every entry it receives until it receives a "completed" indication from the Reader Thread
            dataCollection.emplace_back(data);
            hasNewData = false;
        }
        else if (isComplete)
        {
            // Once a completed indication is received, the Worker Thread will post its results (i.e. the 1-10 sets of data its received from the Reader Thread) in JSON format to the localhost provided
            UDPClient client("127.0.0.1", PORT);
            if (!client.Connect())
            {
                exit(EXIT_FAILURE);
            }

            for (const auto &data : dataCollection)
            {
                std::stringstream oss;
                oss << data;
                client.SendString(oss.str());
            }

            break;
        }

        l.unlock();
        conditionVariable.notify_one();
    }
}

int main(int argc, char const *argv[])
{
    std::thread readerThread(reader_thread);
    std::thread workerThread(worker_thread);

    workerThread.join();
    readerThread.join();

    return 0;
}