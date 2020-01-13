#ifndef DAQ_PIPELINE_CONSOLESTATS_H
#define DAQ_PIPELINE_CONSOLESTATS_H

#include "../IDataStats.h"

namespace debug {
    class ConsoleStats : public bs_daq::IDataStats {
    public:
        void add_stats(uint64_t pulse_id, const bs_daq::StatsMap& stats);
    };
}

#endif //DAQ_PIPELINE_CONSOLESTATS_H
