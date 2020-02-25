#include "BaseStats.h"

bs_daq::BaseStats::BaseStats() : stats_({})
{}

void bs_daq::BaseStats::add_stats(uint64_t pulse_id, const bs_daq::StatsData& pulse_stats)
{
    stats_.iteration_ += pulse_stats.iteration_;
    stats_.get_data_ += pulse_stats.get_data_;
    stats_.n_data_bytes_ += pulse_stats.n_data_bytes_;
    stats_.n_pending_inserts_ += pulse_stats.n_pending_inserts_;
    stats_.n_pulses_ += pulse_stats.n_pulses_;
}


