#ifndef DAQ_PIPELINE_IDATASTORE_H
#define DAQ_PIPELINE_IDATASTORE_H

#include "ChannelValue.h"
#include <vector>

namespace bs_daq {

    class IDataStore {
    public:
        virtual ~IDataStore() = 0;
        virtual void save_data(const std::vector<ChannelValue>& data) = 0;
    };

}

#endif //DAQ_PIPELINE_IDATASTORE_H
