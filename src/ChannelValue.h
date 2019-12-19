#ifndef DAQ_PIPELINE_CHANNELVALUE_H
#define DAQ_PIPELINE_CHANNELVALUE_H

#include <string>
#include <vector>
#include <memory>

namespace bs_daq {

    class ChannelValue {

    private:
        size_t data_n_bytes_;

    public:

        const std::string channel_name_;
        const int64_t pulse_id_mod_;
        const int64_t pulse_id_;
        const std::string type_;
        const std::vector<uint32_t> shape_;
        const std::string encoding_;
        const std::string compression_;
        const size_t max_data_n_bytes_;
        const std::unique_ptr<char*> buffer_;

        ChannelValue(std::string channel_name,
                     int64_t pulse_id_mod,
                     int64_t pulse_id,
                     std::string type,
                     const std::vector<uint32_t>& shape,
                     std::string encoding,
                     std::string compression,
                     size_t max_data_n_bytes);

        void set_data_n_bytes(size_t data_n_bytes);
        size_t get_data_n_bytes();

    };

}

#endif //DAQ_PIPELINE_CHANNELVALUE_H
