#ifndef DAQ_PIPELINE_COMMONS_H
#define DAQ_PIPELINE_COMMONS_H

#include <string>
#include <vector>
#include <memory>

namespace bs_daq {
    struct ChannelMetadata {
        const std::string channel_name_;
        const std::string type_;
        const std::vector<uint32_t> shape_;
        const std::string encoding_;
        const std::string compression_;

        const int64_t pulse_id_mod_;

        ChannelMetadata(
                const std::string channel_name,
                const std::string type,
                const std::vector<uint32_t>& shape,
                const std::string encoding,
                const std::string compression) :
                    channel_name_(channel_name),
                    type_(type),
                    shape_(shape),
                    encoding_(encoding),
                    compression_(compression),
                    pulse_id_mod_(0) {}
    };

    struct ChannelData {
        size_t data_n_bytes_;
        const std::unique_ptr<char> buffer_;

        ChannelData()
    };


}

#endif //DAQ_PIPELINE_COMMONS_H
