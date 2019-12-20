#ifndef BSREAD_RECEIVER_H
#define BSREAD_RECEIVER_H

#include <zmq.hpp>
#include <utility>
#include <string>
#include <memory>
#include <vector>
#include "json.h"

#include "constants.h"

namespace bsread {

    struct main_header {
        std::string htype;
        uint64_t pulse_id;
        timestamp global_timestamp;
        std::string hash;
        std::string dh_compression;
    };

    class Receiver {

        zmq::context_t m_ctx;
        zmq::socket_t m_sock;
        std::string m_address;

        Json::Reader json_reader;

    public:
        Receiver(std::string address, int rcvhwm=10, int sock_typ=ZMQ_PULL);
        bsread::bsread_message receive();
        virtual ~Receiver() = default;

    private:
        main_header get_main_header(void* data, size_t data_len);
        data_header get_data_header(void* data, size_t data_len, compression_type compression);
        data_channel_value get_channel_data(void* data, size_t data_len, compression_type compression);
        std::shared_ptr<timestamp> get_channel_timestamp(void* data, size_t data_len);

        std::string channels_data_hash_;
        std::vector<std::unique_ptr<ChannelData>> channels_data_;
    };
}

#endif //BSREAD_RECEIVER_H
