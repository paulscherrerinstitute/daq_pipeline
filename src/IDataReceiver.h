#ifndef DAQ_PIPELINE_DATARECEIVER_H
#define DAQ_PIPELINE_DATARECEIVER_H

#include <vector>
#include "ChannelData.h"

namespace bs_daq {

    typedef std::vector<std::unique_ptr<ChannelData>> MessageData;

    class IDataReceiver {
    public:
        virtual ~IDataReceiver() = 0;
        virtual const MessageData& get_data();
    };

}

#endif //DAQ_PIPELINE_DATARECEIVER_H
