#include "Store.h"

using namespace std;

scylla::Store::Store(const std::string node_addresses)
{
    session_ = cass_session_new();

    cluster_ = cass_cluster_new();
    cass_cluster_set_contact_points(cluster_, node_addresses.c_str());

    CassFuture* connect_future = cass_session_connect(session_, cluster_);
    auto connection_status = cass_future_error_code(connect_future);
    cass_future_free(connect_future);

    if (connection_status != CASS_OK) {
        cass_cluster_free(cluster_);
        cass_session_free(session_);

// TODO: In case this happens often, extract the error message with cass_future_error_message.
        throw std::runtime_error("Cannot connect to cluster. Please check network and provided hosts.");
    }

}

scylla::Store::~Store()
{
    cass_cluster_free(cluster_);
    cass_session_free(session_);
}

void scylla::Store::save_data(const std::vector<bs_daq::ChannelData>& data)
{
    return;
}