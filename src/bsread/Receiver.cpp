#include "Receiver.h"

#include <iostream>

using namespace std;
using namespace bsread;

Receiver::Receiver(string address, int rcvhwm, int sock_type) :
        m_ctx(1),
        m_sock(m_ctx, sock_type),
        m_address(address)
{
    m_sock.setsockopt(ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm));
    m_sock.connect(address.c_str());
}

bsread_message Receiver::receive()
{
    zmq::message_t msg;
    int more;
    size_t more_size = sizeof(more);

    m_sock.recv(&msg);
    m_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);

    if (!more)
        throw runtime_error("Data header expected after main header.");

    auto main_header = get_main_header(msg.data(), msg.size());

    if (main_header.htype != "bsread-1.0")
        throw runtime_error("Wrong protocol for this receiver.");
    if (main_header.dh_compression != "none")
        throw runtime_error("Data header compression not supported.");

    m_sock.recv(&msg);
    m_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);

    if (main_header->hash != current_data_header_hash_) {
        channels_data_ = get_data_header(msg.data(), msg.size());
        channels_data_hash_ = main_header.hash;
    }

    for (auto channel_data : channels_data_) {

        if (!more)
            throw runtime_error("Invalid message format. The multipart"
                                " message terminated prematurely.");

        size_t recv_n_bytes = m_sock.recv(channel_data.buffer_.get(),
                                          channel_data.data_n_bytes_);
        m_sock.getsockopt(ZMQ_RCVMORE, &more, &more_size);

        if

        if (recv_n_bytes > channel_data.data_n_bytes_)
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

    return bsread_message(main_header, data_header, channels_value);
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

data_header Receiver::get_data_header(void* data, size_t data_len) {

    Json::Value root;
    auto json_string = string(static_cast<char*>(data), data_len);
    json_reader.parse(json_string, root);

    auto data_header = make_shared<bsread::data_header>();
    data_header->htype = root["htype"].asString();

    for(Json::Value& channel : root["channels"]) {

        data_channel_header channel_definition;
        channel_definition.name = channel["name"].asString();
        channel_definition.type = bsdata_type_mapping.at(channel.get("type", "float64").asString());

        for(Json::Value& dimension : channel.get("shape", {1})) {
            channel_definition.shape.push_back(dimension.asUInt());
        }

        channel_definition.compression = compression_type_mapping.at(channel.get("compression", "none").asString());
        channel_definition.endianess = endianess_mapping.at(channel.get("encoding", "little").asString());
        channel_definition.modulo = channel.get("modulo", 1).asUInt();
        channel_definition.offset = channel.get("offset", 0).asUInt();

        data_header->channels.push_back(channel_definition);
    }

    return data_header;
}


shared_ptr<timestamp> Receiver::get_channel_timestamp(void* data, size_t data_len) {
    if (data_len == 0) {
        return nullptr;
    }

    if (data_len != sizeof(timestamp)) {
        throw runtime_error("Timestamp length incorrect. Check sender.");
    }

    auto channel_timestamp = static_cast<uint64_t*>(data);

    return make_shared<timestamp>(channel_timestamp[0], channel_timestamp[1]);
}