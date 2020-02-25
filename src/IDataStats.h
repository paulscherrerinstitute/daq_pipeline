#ifndef DAQ_PIPELINE_IDATASTATS_H
#define DAQ_PIPELINE_IDATASTATS_H

#include <cstdint>
#include <unordered_map>
#include <string>

namespace bs_daq {

    struct StatsData {
	float iteration_;
	float get_data_;
	size_t n_data_bytes_;
	uint32_t n_pending_inserts_;	
	uint32_t n_pulses_;	
    };
    
    class IDataStats {
    public:
        virtual ~IDataStats() = default;
        virtual void add_stats(uint64_t pulse_id, const StatsData& pulse_stats) = 0;
    };

}

#endif //DAQ_PIPELINE_IDATASTATS_H
