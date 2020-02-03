#ifndef DAQ_PIPELINE_IDATASTATS_H
#define DAQ_PIPELINE_IDATASTATS_H

#include <cstdint>
#include <unordered_map>
#include <string>

namespace bs_daq {

    typedef std::unordered_map<std::string, float> StatsMap;

    class IDataStats {
    public:
        virtual ~IDataStats() = default;
        virtual void add_stats(uint64_t pulse_id, const StatsMap& pulse_stats) = 0;
    };

}

#endif //DAQ_PIPELINE_IDATASTATS_H
