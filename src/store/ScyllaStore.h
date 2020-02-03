#ifndef DAQ_PIPELINE_SCYLLASTORE_H
#define DAQ_PIPELINE_SCYLLASTORE_H


#include <cassandra.h>
#include "../IDataStore.h"

namespace scylla {

    static const std::string INSERT_STATEMENT(
        "INSERT INTO bsread_data.channel_data "
        "(channel_name, pulse_id_mod, pulse_id, "
        "data, type, shape, encoding, compression) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");

    template<class T> using cass_ptr = std::unique_ptr<T, void (*)(T *)>;

    class ScyllaStore : public bs_daq::IDataStore {

    protected:
        cass_ptr<CassCluster> cluster_ = {NULL, NULL};
        cass_ptr<CassSession> session_ = {NULL, NULL};
        cass_ptr<const CassPrepared> prepared_insert_ = {NULL, NULL};

        std::atomic<int> n_pending_inserts_;

    public:
        ScyllaStore(const std::string& node_addresses);
        void save_data(const bs_daq::MessageData message_data);
    };
}

#endif //DAQ_PIPELINE_SCYLLASTORE_H
