#include <iostream>
#include <csignal>
#include "receiver/BsreadReceiver.h"
#include "stats/ConsoleStats.h"
#include "store/ScyllaStore.h"

typedef std::chrono::high_resolution_clock hres_clock;
typedef std::chrono::duration<float> f_sec;

int main() {

    signal(SIGTERM, [](int signum) {
        std::cout << "Interrupted by SIGTERM." << std::endl;
        exit(signum);
    });

    auto scylla_hosts = std::getenv("SCYLLA_HOSTS");
    if (!scylla_hosts) {
        throw std::runtime_error("SCYLLA_HOSTS env variable not set.");
    }

    auto stream_host = std::getenv("STREAM_HOST");
    if (!stream_host) {
        throw std::runtime_error("STREAM_HOST env variable not set.");
    }

    std::cout << "Starting daq_pipeline with scylla_hosts=" << scylla_hosts;
    std::cout << " and stream_host=" << stream_host << std::endl;

    auto receiver = bsread::BsreadReceiver(stream_host);
    scylla::ScyllaStore store(scylla_hosts);
    auto stats = debug::ConsoleStats();

    auto stats_map = bs_daq::StatsMap();

    // Default value to make first iteration statistics nicer.
    f_sec time_iteration = f_sec(0.003);
    f_sec time_add_stats = f_sec(0.003);

    std::cout << "[INFO] Start writing loop..." << std::endl;

    while (true) {
        auto start_iteration = hres_clock::now();

        auto message_data = receiver.get_data();
        if (message_data.pulse_id_ == bs_daq::NO_DATA_MESSAGE.pulse_id_) {
            continue;
        }
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