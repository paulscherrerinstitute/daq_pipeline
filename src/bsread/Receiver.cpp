#include "Receiver.h"
#include "json.h"

#include <iostream>

using namespace std;

Receiver::Receiver(string address, int rcvhwm, int sock_type) :
        m_ctx(1),
        m_sock(m_ctx, sock_type),
        m_address(address)
{
    m_sock.setsockopt(ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm));
    m_sock.connect(address.c_str());
}

const bs_daq::MessageData& Receiver::get_data()
{
    zmq::message_t msg;
    int more;
    size_t more_size = sizeof(more);

    m_sock.recv(&msg);
    m_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);

    if (!more)
        throw runtime_error("Data header expected after main header.");

    auto main_header = get_main_header(msg.data(), msg.size());

    if (main_header.htype != "bsr_m-1.1")
        throw runtime_error("Wrong protocol for this receiver.");
    if (main_header.dh_compression != "none")
        throw runtime_error("Data header compression not supported.");

    m_sock.recv(&msg);
    m_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);

    if (main_header->hash != current_data_header_hash_) {
        channels_data_ = get_data_header(msg.data(), msg.size());
        channels_data_hash_ = main_header.hash;
    }

    for (auto data : channels_data_) {

        if (!more)
            throw runtime_error("Invalid message format. The multipart"
                                " message terminated prematurely.");

        data.recv_n_bytes_ = m_sock.recv(data.buffer_.get(),
                                         data.buffer_n_bytes_);
        m_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);

        if (data.recv_n_bytes > data.buffer_n_bytes_)
            throw runtime_error("Received more bytes than expected.")

        if (!more)
            throw runtime_error("Invalid message format. The multipart"
                                " message terminated prematurely.");

        // We omit the channel timestamp because we do not use it.
        m_sock.recv(nullptr, 0);
        m_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);
    }

    if (more)
        throw runtime_error("Invalid message format. The multipart message"
                            " has too many parts. Check sender.");

    return channels_data_;
}

main_header Receiver::get_main_header(void* data, size_t data_len)
{
    Json::Value root;
    auto json_string = string(static_cast<char*>(data), data_len);
    json_reader.parse(json_string, root);

    return {root["htype"].asString(),
            root["pulse_id"].asUInt64(),
            timestamp(root["global_timestamp"]["sec"].asUInt64(),
                      root["global_timestamp"]["ns"].asUInt64()))
            root["hash"].asString(),
            root["dh_compression"].asString()};
}

bs_daq::MessageData Receiver::get_data_header(void* data, size_t data_len) {

    Json::Value root;
    auto json_string = string(static_cast<char*>(data), data_len);
    json_reader.parse(json_string, root);

    bs_daq::MessageData message_data(root["channels"].size());

    for(Json::Value& channel : root["channels"]) {

        size_t n_data_points = 1;
        std::vector<uint32_t> shape(2);
        for(Json::Value& dimension : channel.get("shape", {1})) {
            auto dimension_len = dimension.asUInt();

            n_data_points *= dimension_len;
            shape.push_back(dimension_len);
        }

        auto type = channel.get("type", "float64").asString()
        size_t buffer_n_bytes = n_data_points * bsdata_type_n_bytes.at(type);

        message_data.push_back(
                make_unique<ChannelData>(
                    channel["name"].asString(),
                    type,
                    shape,
                    channel.get("encoding", "little").asString(),
                    channel.get("compression", "none").asString()
                    buffer_n_bytes)
        );
    }

    return message_data;
}
