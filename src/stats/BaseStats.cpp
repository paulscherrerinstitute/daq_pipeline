#include "BaseStats.h"

bs_daq::BaseStats::BaseStats() : stats_(std::make_unique<bs_daq::StatsMap>())
{}

void bs_daq::BaseStats::add_stats(uint64_t pulse_id, const bs_daq::StatsMap& pulse_stats) {

    (*stats_)["n_pulses"] += 1;

    for (auto& stat: pulse_stats) {
        auto stat_name = stat.first;
        auto stat_value = stat.second;

        (*stats_)[stat_name] += stat_value;
    }
}


