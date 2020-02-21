#include <iostream>
#include <iomanip>
#include "ConsoleStats.h"

void debug::ConsoleStats::add_stats(uint64_t pulse_id, const bs_daq::StatsData& pulse_stats) {
    BaseStats::add_stats(pulse_id, pulse_stats);

    if (stats_.n_pulses_ == 100) {
        std::cout << "pulse_id="<< pulse_id << " {";
	
	std::cout << "\"iteration\": " << stats_.iteration_ << ", ";
	std::cout << "\"get_data\": " << stats_.get_data_ << ", ";
	std::cout << "\"save_data\": " << stats_.save_data_ << ", ";
	std::cout << "\"add_stats\": " << stats_.add_stats_ << ", ";
	std::cout << "\"n_data_bytes\": " << stats_.n_data_bytes_ << ", ";
	std::cout << "\"n_pending_inserts\": " << stats_.n_pending_inserts_ << ", ";
	std::cout << "\"n_pulses\": " << stats_.n_pulses_;
        
	std::cout << "}" << std::endl;

        stats_ = bs_daq::StatsData();
    }
}
