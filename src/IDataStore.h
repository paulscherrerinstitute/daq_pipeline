#ifndef DAQ_PIPELINE_IDATASTORE_H
#define DAQ_PIPELINE_IDATASTORE_H

#include "Data.h"
#include <vector>

namespace bs_daq {

    class IDataStore {
    public:
        virtual ~IDataStore() = default;
        virtual void save_data(MessageData message_data) = 0;
    };

}

#endif //DAQ_PIPELINE_IDATASTORE_H
