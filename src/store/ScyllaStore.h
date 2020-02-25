#ifndef DAQ_PIPELINE_SCYLLASTORE_H
#define DAQ_PIPELINE_SCYLLASTORE_H


#include <cassandra.h>
#include <atomic>
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
        CassCluster* cluster_ = nullptr;
        CassSession* session_ = nullptr;
        const CassPrepared* prepared_insert_ = nullptr;

        std::atomic<uint32_t> n_pending_inserts_;

    public:
        ScyllaStore(const std::string& node_addresses);
        ~ScyllaStore();
        void save_data(bs_daq::MessageData message_data);
        uint32_t get_n_pending_inserts();
    };
}

#endif //DAQ_PIPELINE_SCYLLASTORE_H

