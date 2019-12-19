#include "ChannelValue.h"

bs_daq::ChannelValue::ChannelValue(
        std::string channel_name,
        int64_t pulse_id_mod,
        int64_t pulse_id,
        std::string type,
        const std::vector<uint32_t>& shape,
        std::string encoding,
        std::string compression,
        size_t data_n_bytes)
            : channel_name_(channel_name),
              pulse_id_mod_(pulse_id_mod),
              pulse_id_(pulse_id),
              type_(type),
              shape_(shape),
              encoding_(encoding),
              compression_(compression),
              data_n_bytes_(data_n_bytes),
              buffer_(std::make_unique<char>(data_n_bytes)) {}
