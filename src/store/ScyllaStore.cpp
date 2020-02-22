#include <iostream>
#include "ScyllaStore.h"

using namespace std;

scylla::ScyllaStore::ScyllaStore(const std::string& node_addresses) :
        n_pending_inserts_(0)
{
    session_ = {
            cass_session_new(),
            [](CassSession *p){ cass_session_free(p); }
    };

    cluster_ = {
            cass_cluster_new(),
            [](CassCluster *p){ cass_cluster_free(p); }
    };

    cass_cluster_set_contact_points(cluster_.get(), node_addresses.c_str());
    cass_cluster_set_token_aware_routing(cluster_.get(), cass_true);
    cass_cluster_set_num_threads_io(cluster_.get(), 1);
    cass_cluster_set_queue_size_io(cluster_.get(), 32000);

    cass_ptr<CassFuture> connect_future({
        cass_session_connect(session_.get(), cluster_.get()),
        [](CassFuture *p){ cass_future_free(p); }
    });
    auto connection_status = cass_future_error_code(connect_future.get());

    if (connection_status != CASS_OK) {

// TODO: In case this happens often, extract the error message with cass_future_error_message.
        throw runtime_error("Cannot connect to cluster. "
                            "Please check network and provided hosts.");
    }

    cass_ptr<CassFuture> prepared_future = {
            cass_session_prepare(session_.get(), INSERT_STATEMENT.c_str()),
            [](CassFuture *p){ cass_future_free(p); }
    };
    auto prepared_status = cass_future_error_code(prepared_future.get());

    if (prepared_status != CASS_OK) {
// TODO: Maybe a more detailed error message?
        throw runtime_error("Cannot prepare statement on cluster.");
    }

    prepared_insert_ = {
            cass_future_get_prepared(prepared_future.get()),
            [](const CassPrepared *p){ cass_prepared_free(p); }
    };
}

void scylla::ScyllaStore::save_data(const bs_daq::MessageData message_data)
{

    for (auto& channel_data : *message_data.channels_){

	auto data = channel_data.get();
	
	auto statement = cass_prepared_bind(prepared_insert_.get());
        
	cass_statement_bind_string_by_name(statement,
                "channel_name", data->channel_name_.c_str());

        cass_statement_bind_int64_by_name(statement,
                "pulse_id_mod", data->pulse_id_mod_);

        cass_statement_bind_int64_by_name(statement,
                "pulse_id", data->pulse_id_);

        cass_statement_bind_bytes_by_name(statement,
                 "data",
                 reinterpret_cast<uint8_t*>(data->buffer_.get()),
                 channel_data->buffer_n_bytes_);

        cass_statement_bind_string_by_name(statement,
                "type", data->type_.c_str());

        auto cass_shape = cass_collection_new(CASS_COLLECTION_TYPE_LIST,
                                              data->shape_.size());

        for (auto shape_element : data->shape_) {
            cass_collection_append_uint32(cass_shape, shape_element);
        }

        cass_statement_bind_collection_by_name(statement,
                "shape", cass_shape);

	cass_collection_free(cass_shape);
       
	cass_statement_bind_string_by_name(statement,
                "encoding", data->encoding_.c_str());

        cass_statement_bind_string_by_name(statement,
                "compression", data->compression_.c_str());

        n_pending_inserts_++;
 
	auto insert_future = cass_session_execute(session_.get(), statement);

        cass_future_set_callback(
                insert_future,
                [](CassFuture* future, void* data) {
                    // TODO: Read future result and log eventual error.
                    static_cast<scylla::ScyllaStore*>(data)->n_pending_inserts_--;
                    },
                this);

	cass_future_free(insert_future);
	cass_statement_free(statement);
    }
}

uint32_t scylla::ScyllaStore::get_n_pending_inserts()
{
    return static_cast<uint32_t>(n_pending_inserts_);
}
