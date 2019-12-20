#ifndef DAQ_PIPELINE_DATARECEIVER_H
#define DAQ_PIPELINE_DATARECEIVER_H

#include <vector>
#include "ChannelData.h"

namespace bs_daq {

    class IDataReceiver {
    public:
        virtual ~IDataReceiver() = 0;
        virtual const std::vector<std::unique_ptr<ChannelData>>& get_data();
    };

}

#endif //DAQ_PIPELINE_DATARECEIVER_H
