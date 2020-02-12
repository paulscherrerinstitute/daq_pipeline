#include <iostream>
#include "receiver/BsreadReceiver.h"
#include "stats/ConsoleStats.h"
#include "store/ScyllaStore.h"

typedef std::chrono::high_resolution_clock hres_clock;
typedef std::chrono::duration<float> f_sec;

int main() {

    auto receiver = bsread::BsreadReceiver("tcp://127.0.0.1:10100");
    scylla::ScyllaStore store("127.0.0.1");
    auto stats = debug::ConsoleStats();

    auto stats_map = bs_daq::StatsMap();

    // Default value to make first iteration statistics nicer.
    f_sec time_iteration = f_sec(0.003);
    f_sec time_add_stats = f_sec(0.003);

    std::cout << "[INFO] Start writing loop..." << std::endl;

    while (true) {
        auto start_iteration = hres_clock::now();

        auto message_data = receiver.get_data();
        f_sec time_get_data = hres_clock::now() - start_iteration;

        auto start_save_data = hres_clock::now();
        store.save_data(message_data);
        f_sec time_save_data = hres_clock::now() - start_save_data;

        auto start_add_stats = hres_clock::now();
        stats.add_stats(message_data.pulse_id_, {
                {"iteration", time_iteration.count()},
                {"get_data", time_get_data.count()},
                {"save_data", time_save_data.count()},
                {"add_stats", time_add_stats.count()},
                {"n_data_bytes", float(message_data.n_data_bytes_)},
                {"n_pending_inserts", store.get_n_pending_inserts()}
        });

        auto end_iteration = hres_clock::now();
        time_add_stats = end_iteration - start_add_stats;
        time_iteration = end_iteration - start_iteration;
    }

    return 0;
}