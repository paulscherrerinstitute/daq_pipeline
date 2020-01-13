#include "Store.h"

using namespace std;

scylla::Store::Store(const std::string& node_addresses)
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

void scylla::Store::save_data(const std::vector<bs_daq::ChannelData>& data)
{
    for (auto& channel_data : data){
        cass_ptr<CassStatement> statement = {
                cass_prepared_bind(prepared_insert_.get()),
                [](CassStatement *p){ cass_statement_free(p); }
        };

        cass_statement_bind_string_by_name(statement.get(),
                "channel_name", channel_data.channel_name_.c_str());

        cass_statement_bind_int64_by_name(statement.get(),
                "pulse_id_mod", channel_data.pulse_id_mod_);

        cass_statement_bind_int64_by_name(statement.get(),
                "pulse_id", channel_data.pulse_id_);

        cass_statement_bind_bytes_by_name(statement.get(),
                 "data",
                 reinterpret_cast<uint8_t*>(channel_data.buffer_.get()),
                 channel_data.buffer_n_bytes_);

        cass_statement_bind_string_by_name(statement.get(),
                "type", channel_data.type_.c_str());

// TODO: Construct the actual shape.
        cass_statement_bind_collection_by_name(statement.get(),
                "shape", NULL);

        cass_statement_bind_string_by_name(statement.get(),
                "encoding", channel_data.encoding_.c_str());

        cass_statement_bind_string_by_name(statement.get(),
                "compression", channel_data.compression_.c_str());
    }
}