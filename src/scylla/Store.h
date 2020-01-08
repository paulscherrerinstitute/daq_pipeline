#ifndef DAQ_PIPELINE_STORE_H
#define DAQ_PIPELINE_STORE_H


#include <cassandra.h>
#include "../IDataStore.h"

namespace scylla {
    class Store : public bs_daq::IDataStore {

    protected:
        CassCluster* cluster_ = NULL;
        CassSession* session_ = NULL;

    public:
        Store(const std::string node_addresses);
        ~Store();

        void save_data(const std::vector<bs_daq::ChannelData>& data);
    };
}

#endif //DAQ_PIPELINE_STORE_H