#ifndef DAQ_PIPELINE_DATARECEIVER_H
#define DAQ_PIPELINE_DATARECEIVER_H

#include <vector>
#include "ChannelData.h"

namespace bs_daq {

    class IDataReceiver {
    public:
        virtual ~IDataReceiver() = default;
        virtual const MessageData& get_data() = 0;
    };

}

#endif //DAQ_PIPELINE_DATARECEIVER_H
