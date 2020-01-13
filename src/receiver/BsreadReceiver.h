#ifndef BSREAD_RECEIVER_H
#define BSREAD_RECEIVER_H

#include <zmq.hpp>
#include <utility>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "../external/json.h"
#include "../IDataReceiver.h"

namespace bsread {

    struct timestamp {
        const uint64_t sec; // seconds past UNIX epoch (1/1/1970)
        const uint64_t nsec;  // nanosecond offset since last full second

        timestamp(uint64_t sec, uint64_t nsec) : sec(sec), nsec(nsec) {};
    };

    struct main_header {
        std::string htype;
        uint64_t pulse_id;
        timestamp global_timestamp;
        std::string hash;
        std::string dh_compression;
    };

    static const std::unordered_map<std::string, size_t> bs_type_n_bytes = {
            {"string",  1},
            {"bool",    1},
            {"float64", 8},
            {"float32", 4},
            {"int8",    1},
            {"uint8",   1},
            {"int16",   2},
            {"uint16",  2},
            {"int32",   4},
            {"uint32",  4},
            {"int64",   8},
            {"uint64",  8},
    };

    class BsreadReceiver : public bs_daq::IDataReceiver {

        zmq::context_t ctx_;
        zmq::socket_t sock_;
        std::string source_address_;

        Json::Reader json_reader_;

    public:
        BsreadReceiver(std::string address, int rcvhwm=10, int sock_typ=ZMQ_PULL);
        StreamMessage get_data();

    private:
        main_header get_main_header(void* data, size_t data_len);
        bs_daq::MessageData get_data_header(void* data, size_t data_len);

        bs_daq::MessageData channels_data_;
        std::string channels_data_hash_;
    };
}

#endif //BSREAD_RECEIVER_H
