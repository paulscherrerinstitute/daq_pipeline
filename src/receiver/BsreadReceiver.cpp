#include "BsreadReceiver.h"
#include "json.h"

#include <iostream>

using namespace std;


bsread::BsreadReceiver::BsreadReceiver(string address, int rcvhwm, int sock_type) :
        ctx_(1),
        sock_(ctx_, sock_type),
        source_address_(address)
{
    int timeout = 1000;
    sock_.setsockopt(ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm));
    sock_.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    sock_.connect(address.c_str());
}

bs_daq::MessageData bsread::BsreadReceiver::get_data()
{
    zmq::message_t msg;
    int more;
    size_t more_size = sizeof(more);

    if (!sock_.recv(msg)) {
        return bs_daq::NO_DATA_MESSAGE;
    }
    sock_.getsockopt(ZMQ_RCVMORE, &more, &more_size);

    if (!more)
        throw runtime_error("Data header expected after main header.");

    auto main_header = get_main_header(msg.data(), msg.size());

    if (main_header.htype != "bsr_m-1.1")
        throw runtime_error("Wrong protocol for this receiver.");
    if (main_header.dh_compression != "")
        throw runtime_error("Data header compression not supported.");

    sock_.recv(msg);
    sock_.getsockopt(ZMQ_RCVMORE, &more, &more_size);

    if (main_header.hash != channels_data_hash_) {
        build_data_header(msg.data(), msg.size());
        channels_data_hash_ = main_header.hash;
    }

    size_t n_data_bytes = 0;

    for (auto& data : *channels_data_) {

        if (!more)
            throw runtime_error("Invalid message format. The multipart"
                                " message terminated prematurely.");

        data->pulse_id_ =  main_header.pulse_id;

        data->recv_n_bytes_ = sock_.recv(data->buffer_.get(),
                                         data->buffer_n_bytes_);
        sock_.getsockopt(ZMQ_RCVMORE, &more, &more_size);

        if (data->recv_n_bytes_ > data->buffer_n_bytes_)
            throw runtime_error("Received more bytes than expected.");

        if (!more)
            throw runtime_error("Invalid message format. The multipart"
                                " message terminated prematurely.");

        // We omit the channel timestamp because we do not use it.
        sock_.recv(zmq::mutable_buffer());
        sock_.getsockopt(ZMQ_RCVMORE, &more, &more_size);

        n_data_bytes += data->recv_n_bytes_;
    }

    if (more)
        throw runtime_error("Invalid message format. The multipart message"
                            " has too many parts. Check sender.");

    return {main_header.pulse_id, n_data_bytes, channels_data_};
}

bsread::main_header bsread::BsreadReceiver::get_main_header(
        void* data, size_t data_len)
{
    Json::Value root;
    auto json_string = string(static_cast<char*>(data), data_len);
    json_reader_.parse(json_string, root);

    return {root["htype"].asString(),
            root["pulse_id"].asInt64(),
            timestamp(root["global_timestamp"]["sec"].asUInt64(),
                      root["global_timestamp"]["ns"].asUInt64()),
            root["hash"].asString(),
            root["dh_compression"].asString()};
}

void bsread::BsreadReceiver::build_data_header(
        void* data, size_t data_len)
{
    Json::Value root;
    auto json_string = string(static_cast<char*>(data), data_len);
    json_reader_.parse(json_string, root);

    channels_data_ = make_shared<bs_daq::Channels>();

    for(Json::Value& channel : root["channels"]) {

        size_t n_data_points = 1;
        std::vector<uint32_t> shape(2);
        for(Json::Value& dimension : channel.get("shape", {1})) {
            auto dimension_len = dimension.asUInt();

            n_data_points *= dimension_len;
            shape.push_back(dimension_len);
        }

        auto type = channel.get("type", "float64").asString();
        size_t buffer_n_bytes = n_data_points * bs_type_n_bytes.at(type);

// TODO: This is stupid. Strings should also be able to tell the needed buffer size in advance.
        if (type == "string") {
            buffer_n_bytes *= 1024;
        }

// TODO: Implement pulse_id_mod calculation.
        int64_t pulse_id_mod = 0;

        channels_data_->push_back(
                make_unique<bs_daq::ChannelData>(
                    channel["name"].asString(),
                    type,
                    shape,
                    channel.get("encoding", "little").asString(),
                    channel.get("compression", "none").asString(),
                    buffer_n_bytes,
                    pulse_id_mod)
        );
    }
}
