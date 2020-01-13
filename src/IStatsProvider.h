#ifndef DAQ_PIPELINE_ISTATSPROVIDER_H
#define DAQ_PIPELINE_ISTATSPROVIDER_H

#include <cstdint>
#include <unordered_map>

namespace bs_daq {

    typedef std::unordered_map<std::string, float> StatsMap;

    class IStatsProvider {
        virtual ~IStatsProvider() = default;
        virtual void add_stats(uint64_t pulse_id, const StatsMap& stats) = 0;
    };

}

#endif //DAQ_PIPELINE_ISTATSPROVIDER_H
