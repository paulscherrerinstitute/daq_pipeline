#include <iostream>
#include <iomanip>
#include "ConsoleStats.h"

void debug::ConsoleStats::add_stats(uint64_t pulse_id, const bs_daq::StatsMap& pulse_stats) {
    BaseStats::add_stats(pulse_id, pulse_stats);

    auto n_pulses = (*stats_)["n_pulses"];

    if (n_pulses == 100) {
        std::cout << "pulse_id="<< pulse_id << " {";

        for (auto& stat: *stats_) {
            auto stat_name = stat.first;
            auto stat_value = stat.second / n_pulses;

            std::cout << "\"" << stat_name << "\": ";
            std::cout << stat_value << ", ";
        }
        std::cout << "}" << std::endl;

        stats_->clear();
    }
}
