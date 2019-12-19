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

    struct data_channel_value {

        data_channel_value() : data(nullptr), data_len(0) {}

        data_channel_value(char* data, size_t data_len) :
                data(data, std::default_delete<char[]>()),
                data_len(data_len)
        {}

        std::shared_ptr<char> data;
        size_t data_len;
        std::shared_ptr<timestamp> timestamp;
    };

    struct data_channel_header {
        std::string name;
        bsdata_type type;
        std::vector<uint32_t> shape;
        compression_type compression;
        endianess endianess;
        uint32_t modulo;
        uint32_t offset;
    };

    struct data_header {
        std::string htype;
        std::vector<data_channel_header> channels;
    };

    struct main_header {
        compression_type dh_compression;
        timestamp global_timestamp;
        std::string hash;
        std::string htype;
        uint64_t pulse_id;
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
        std::shared_ptr<main_header> get_main_header(void* data, size_t data_len);
        data_header get_data_header(void* data, size_t data_len, compression_type compression);
        data_channel_value get_channel_data(void* data, size_t data_len, compression_type compression);
        std::shared_ptr<timestamp> get_channel_timestamp(void* data, size_t data_len);

        std::shared_ptr<data_header> data_header_;
    };
}

#endif //BSREAD_RECEIVER_H
