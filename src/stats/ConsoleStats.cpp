#include <iostream>
#include <iomanip>
#include "ConsoleStats.h"

debug::ConsoleStats::ConsoleStats(const std::string& device_name, size_t n_pulses_output):
    device_name_(device_name),
    n_pulses_output_(n_pulses_output)
{}

void debug::ConsoleStats::add_stats(uint64_t pulse_id, const bs_daq::StatsData& pulse_stats) {
    BaseStats::add_stats(pulse_id, pulse_stats);

    std::cout << std::fixed << std::setprecision( 2 );

    if (stats_.n_pulses_ == n_pulses_output_) {
        std::cout << device_name_ << " ";
        std::cout << "i: " << stats_.iteration_/n_pulses_output_*1000 << " ";
        std::cout << "d: " << stats_.get_data_/n_pulses_output_*1000<< " ";
        std::cout << "b: " << stats_.n_data_bytes_/n_pulses_output_<< " ";
        std::cout << "np: " << stats_.n_pending_inserts_/n_pulses_output_;
        std::cout << std::endl;

        stats_ = bs_daq::StatsData();
    }
}
