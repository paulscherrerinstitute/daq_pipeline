#ifndef DAQ_PIPELINE_DATARECEIVER_H
#define DAQ_PIPELINE_DATARECEIVER_H

#include "ChannelValue.h"
#include <vector>

namespace bs_daq {

    class IDataReceiver {
    public:
        virtual ~IDataReceiver() = 0;
        virtual std::vector<ChannelValue> get_data() = 0;
    };

}

#endif //DAQ_PIPELINE_DATARECEIVER_H
