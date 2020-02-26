#ifndef DAQ_PIPELINE_DATA_H
#define DAQ_PIPELINE_DATA_H

#include <string>
#include <vector>
#include <memory>

namespace bs_daq {

    struct ChannelData {

        // Attributes set on data header change.
        const std::string channel_name_;
        const std::string type_;
        const std::vector<uint32_t> shape_;
        const std::string encoding_;
        const std::string compression_;
        const size_t buffer_n_bytes_;
        char* const buffer_;
        const int64_t pulse_id_div_;

        // Attributes set for each received message.
        int64_t pulse_id_ = 0;
        size_t recv_n_bytes_ = 0;

        ChannelData(
                std::string channel_name,
                std::string type,
                const std::vector<uint32_t>& shape,
                std::string encoding,
                std::string compression,
                size_t buffer_n_bytes,
                int64_t pulse_id_mod)
                : channel_name_(channel_name),
                  type_(type),
                  shape_(shape),
                  encoding_(encoding),
                  compression_(compression),
                  buffer_n_bytes_(buffer_n_bytes),
                  buffer_(new char[buffer_n_bytes]),
                  pulse_id_div_(pulse_id_mod) {}
    };

    struct MessageData {
        int64_t pulse_id_;
        size_t n_data_bytes_;
        std::vector<bs_daq::ChannelData>* channels_;
    };

    static const MessageData NO_DATA_MESSAGE =
        {-1, 0, nullptr};
}

#endif //DAQ_PIPELINE_DATA_H
