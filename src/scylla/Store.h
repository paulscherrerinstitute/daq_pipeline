#ifndef DAQ_PIPELINE_STORE_H
#define DAQ_PIPELINE_STORE_H


#include <cassandra.h>
#include "../IDataStore.h"

namespace scylla {

    static const std::string INSERT_STATEMENT(
        "INSERT INTO bsread_data.channel_data "
        "(channel_name, pulse_id_mod, pulse_id, "
        "data, type, shape, encoding, compression) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");

    template<class T> using cass_ptr = std::unique_ptr<T, void (*)(T *)>;

    class Store : public bs_daq::IDataStore {

    protected:
        cass_ptr<CassCluster> cluster_ = {NULL, NULL};
        cass_ptr<CassSession> session_ = {NULL, NULL};
        cass_ptr<const CassPrepared> prepared_insert_ = {NULL, NULL};

        void free_class_member_cass_objects();

    public:
        Store(const std::string node_addresses);
        ~Store();

        void save_data(const std::vector<bs_daq::ChannelData>& data);
    };
}

#endif //DAQ_PIPELINE_STORE_H
