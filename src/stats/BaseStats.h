#ifndef DAQ_PIPELINE_BASESTATS_H
#define DAQ_PIPELINE_BASESTATS_H
#include "../IDataStats.h"
#include <memory>

namespace bs_daq {
    class BaseStats : public bs_daq::IDataStats {
    protected:
        std::unique_ptr<bs_daq::StatsMap> stats_ = NULL;

    public:
        BaseStats();
        void add_stats(uint64_t pulse_id, const bs_daq::StatsMap& pulse_stats);
    };
}

#endif //DAQ_PIPELINE_BASESTATS_H
