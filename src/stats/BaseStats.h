#ifndef DAQ_PIPELINE_BASESTATS_H
#define DAQ_PIPELINE_BASESTATS_H
#include "../IDataStats.h"
#include <memory>

namespace bs_daq {
    class BaseStats : public bs_daq::IDataStats {
    protected:
        StatsData stats_{};

    public:
        BaseStats();
        void add_stats(uint64_t pulse_id, const StatsData& pulse_stats);
    };
}

#endif //DAQ_PIPELINE_BASESTATS_H
