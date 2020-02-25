#include "ScyllaStore.h"

using namespace std;

scylla::ScyllaStore::ScyllaStore(const std::string& node_addresses) :
        n_pending_inserts_(0)
{
    session_ = cass_session_new();
    cluster_ = cass_cluster_new();

    cass_cluster_set_contact_points(cluster_, node_addresses.c_str());
    cass_cluster_set_token_aware_routing(cluster_, cass_true);
    cass_cluster_set_num_threads_io(cluster_, 1);
    cass_cluster_set_queue_size_io(cluster_, 32000);

    cass_ptr<CassFuture> connect_future({
        cass_session_connect(session_, cluster_),
        [](CassFuture *p){ cass_future_free(p); }
    });
    auto connection_status = cass_future_error_code(connect_future.get());

    if (connection_status != CASS_OK) {
        throw runtime_error("Cannot connect to cluster. "
                            "Please check network and provided hosts.");
    }

    cass_ptr<CassFuture> prepared_future = {
            cass_session_prepare(session_, INSERT_STATEMENT.c_str()),
            [](CassFuture *p){ cass_future_free(p); }
    };
    auto prepared_status = cass_future_error_code(prepared_future.get());

    if (prepared_status != CASS_OK) {
        throw runtime_error("Cannot prepare statement on cluster.");
    }

    prepared_insert_ = cass_future_get_prepared(prepared_future.get());
}

scylla::ScyllaStore::~ScyllaStore()
{
    cass_prepared_free(prepared_insert_);
    cass_session_free(session_);
    cass_cluster_free(cluster_);
}

void scylla::ScyllaStore::save_data(const bs_daq::MessageData message_data)
{
    for (auto& channel_data : *message_data.channels_){

	auto data = channel_data.get();
	
	auto statement = cass_prepared_bind(prepared_insert_);
        
	cass_statement_bind_string_by_name(statement,
                "channel_name", data->channel_name_.c_str());

        cass_statement_bind_int64_by_name(statement,
                "pulse_id_mod", (int64_t)(data->pulse_id_ / data->pulse_id_div_));

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

	    auto insert_future = cass_session_execute(session_, statement);

        cass_future_set_callback(
                insert_future,
                [](__attribute__((unused)) CassFuture* future, void* data) {
                    // TODO: Read future result and log eventual error.
                    (*((std::atomic<uint32_t>*)data))--;
                    },
                &n_pending_inserts_);

        cass_future_free(insert_future);
        cass_statement_free(statement);
    }

    n_pending_inserts_ += message_data.channels_->size();
}

uint32_t scylla::ScyllaStore::get_n_pending_inserts()
{
    return n_pending_inserts_;
}
