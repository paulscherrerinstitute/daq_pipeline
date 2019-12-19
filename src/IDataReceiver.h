#ifndef DAQ_PIPELINE_DATARECEIVER_H
#define DAQ_PIPELINE_DATARECEIVER_H

#include "ChannelValue.h"
#include <vector>

namespace bs_daq {

    class IDataReceiver {
    public:
        virtual ~IDataReceiver() = 0;
        virtual std::shared_ptr<ChannelValue> get_data();
    };

}

#endif //DAQ_PIPELINE_DATARECEIVER_H
