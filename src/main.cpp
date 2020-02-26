#include <iostream>
#include <csignal>
#include <chrono>
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
    auto stats = debug::ConsoleStats(std::string(stream_host).substr(6), 100);

    // Default value to make first iteration statistics nicer.
    f_sec time_iteration = f_sec(0.01);

    std::cout << "[INFO] Start writing loop..." << std::endl;

    while (true) {
        auto start_iteration = hres_clock::now();

        auto message_data = receiver.get_data();
        if (message_data.channels_ == nullptr) {
            continue;
        }
        f_sec time_get_data = hres_clock::now() - start_iteration;

        store.save_data(message_data);

        stats.add_stats(message_data.pulse_id_, {
                time_iteration.count(),
                time_get_data.count(),
                message_data.n_data_bytes_,
                store.get_n_pending_inserts(),
		1 // n_pulses
        });

        time_iteration = hres_clock::now() - start_iteration;
    }

    return 0;
}
