#include "BsreadReceiver.h"

#include <iostream>

using namespace std;


bsread::BsreadReceiver::BsreadReceiver(string address, int rcvhwm, int sock_type) :
    header_buffer_size_(4096),
    source_address_(address)
{
    int timeout = 1000;
    ctx_ = zmq_ctx_new();
    sock_ = zmq_socket(ctx_, sock_type);

    zmq_setsockopt(sock_, ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm));
    zmq_setsockopt(sock_, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

    if (zmq_connect(sock_, address.c_str())) {
        throw runtime_error(zmq_strerror(zmq_errno()));
    }

    header_buffer_ = malloc(header_buffer_size_);
}

bsread::BsreadReceiver::~BsreadReceiver()
{
    zmq_close(sock_);
    zmq_ctx_destroy(ctx_);
    free(header_buffer_);
}

bs_daq::MessageData bsread::BsreadReceiver::get_data()
{
    int n_bytes;
    int more;
    size_t more_size = sizeof(more);

    n_bytes = zmq_recv(sock_, header_buffer_, header_buffer_size_, 0);
    zmq_getsockopt(sock_, ZMQ_RCVMORE, &more, &more_size);

    if (n_bytes == -1) {
        int error_n = zmq_errno();
        if (error_n == EAGAIN || error_n == EINTR) {
            return bs_daq::NO_DATA_MESSAGE;
        } else {
            throw runtime_error(zmq_strerror(error_n));
        }
    }
    if ((size_t)n_bytes > header_buffer_size_) {
        throw runtime_error("The received header size exceeds the allocated buffer.");
    }
    if (!more)
        throw runtime_error("Data header expected after main header.");

    auto main_header = get_main_header(header_buffer_, n_bytes);

    n_bytes = zmq_recv(sock_, header_buffer_, header_buffer_size_, 0);
    zmq_getsockopt(sock_, ZMQ_RCVMORE, &more, &more_size);

    if (n_bytes == -1) {
        throw runtime_error(zmq_strerror(zmq_errno()));
    }
    if ((size_t)n_bytes > header_buffer_size_) {
        throw runtime_error("The received header size exceeds the allocated buffer.");
    }

    if (main_header.hash != channels_data_hash_) {
        build_data_header(header_buffer_, header_buffer_size_);
        channels_data_hash_ = main_header.hash;
    }

    size_t n_data_bytes = 0;
    for (auto& data_smart_ptr : *channels_data_) {

        if (!more) {
            throw runtime_error("Invalid message format. The multipart"
                                " message terminated prematurely.");
        }

	    auto data = data_smart_ptr.get();

        data->pulse_id_ =  main_header.pulse_id;

        data->recv_n_bytes_ = zmq_recv(sock_,
                                       data->buffer_.get(),
                                       data->buffer_n_bytes_,
                                       0);
        zmq_getsockopt(sock_, ZMQ_RCVMORE, &more, &more_size);

        if (data->recv_n_bytes_ > data->buffer_n_bytes_)
            throw runtime_error("Received more bytes than expected.");

        if (!more)
            throw runtime_error("Invalid message format. The multipart"
                                " message terminated prematurely.");

        // We omit the channel timestamp because we do not use it.
        zmq_recv(sock_, nullptr, 0, 0);
        zmq_getsockopt(sock_, ZMQ_RCVMORE, &more, &more_size);

        n_data_bytes += data->recv_n_bytes_;
    }

    if (more)
        throw runtime_error("Invalid message format. The multipart message"
                            " has too many parts. Check sender.");

    return {main_header.pulse_id, n_data_bytes, channels_data_};
}

#include "rapidjson/document.h"

bsread::main_header bsread::BsreadReceiver::get_main_header(
        void* data, size_t data_len)
{
    rapidjson::Document root;
    root.Parse(static_cast<char*>(data), data_len);
    
    return {root["pulse_id"].GetInt64(),
            root["hash"].GetString()};
}

void bsread::BsreadReceiver::build_data_header(
        void* data, size_t data_len)
{
    rapidjson::Document root;
    root.Parse(static_cast<char*>(data), data_len);

    channels_data_ = make_shared<bs_daq::Channels>();

    for(auto const& channel : root["channels"].GetArray()) {

        size_t n_data_points = 1;
        std::vector<uint32_t> shape(2);

        if (channel.HasMember("shape")) {
            for (auto const& dimension : channel["shape"].GetArray()) {
                auto dimension_len = dimension.GetUint();

                n_data_points *= dimension_len;
                shape.push_back(dimension_len);
            }
        } else {
            shape.push_back(1);
        }

        string type = "float64";
        if (channel.HasMember("type")) {
            type = channel["type"].GetString();
        }

        size_t buffer_n_bytes = n_data_points * bs_type_n_bytes.at(type);

// TODO: Stupid. Needs bsread protocol fix first.
        if (type == "string") {
            buffer_n_bytes *= 1024;
        }

        // Target partition is 200MB.
        int64_t pulse_id_div = (200 * 1024 * 1024) / (buffer_n_bytes * 100);

        string encoding = "little";
        if (channel.HasMember("encoding")) {
            encoding = channel["encoding"].GetString();
        }

        string compression = "none";
        if (channel.HasMember("compression")) {
            compression = channel["compression"].GetString();
        }

        channels_data_->push_back(
                make_unique<bs_daq::ChannelData>(
                        channel["name"].GetString(),
                        type,
                        shape,
                        encoding,
                        compression,
                        buffer_n_bytes,
                        pulse_id_div)
        );
    }
}
