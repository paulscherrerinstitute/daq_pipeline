#ifndef DAQ_PIPELINE_ISTATSPROVIDER_H
#define DAQ_PIPELINE_ISTATSPROVIDER_H

#include <cstdint>
#include <unordered_map>

typedef std::unordered_map<std::string, float> StatsMap;

namespace bs_daq {

    class IStatsProvider {
        virtual ~IStatsProvider() = 0;
        virtual void add_stats(uint64_t pulse_id, const StatsMap& stats) = 0;
    };

}

#endif //DAQ_PIPELINE_ISTATSPROVIDER_H
