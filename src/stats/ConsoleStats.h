#ifndef DAQ_PIPELINE_CONSOLESTATS_H
#define DAQ_PIPELINE_CONSOLESTATS_H

#include "../IDataStats.h"
#include "BaseStats.h"

namespace debug {
    class ConsoleStats : public bs_daq::BaseStats {
    public:
        void add_stats(uint64_t pulse_id, const bs_daq::StatsData& pulse_stats);
    };
}

#endif //DAQ_PIPELINE_CONSOLESTATS_H
