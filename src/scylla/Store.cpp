#include "Store.h"

using namespace std;

scylla::Store::Store(const std::string node_addresses)
{
    session_ = cass_session_new();

    cluster_ = cass_cluster_new();
    cass_cluster_set_contact_points(cluster_, node_addresses.c_str());
    cass_cluster_set_token_aware_routing(cluster_, cass_true);

    CassFuture* connect_future = cass_session_connect(session_, cluster_);
    auto connection_status = cass_future_error_code(connect_future);
    cass_future_free(connect_future);

    if (connection_status != CASS_OK) {
        free_class_member_cass_objects();
// TODO: In case this happens often, extract the error message with cass_future_error_message.
        throw std::runtime_error("Cannot connect to cluster. Please check network and provided hosts.");
    }


}

scylla::Store::~Store()
{
    free_class_member_cass_objects();
}

void scylla::Store::free_class_member_cass_objects()
{
    if (prepared_insert_ != NULL) {
        cass_prepared_free(prepared_insert_);
    }

    if (cluster_ != NULL) {
        cass_cluster_free(cluster_);
    }

    if (session_ != NULL) {
        cass_session_free(session_);
    }
}

void scylla::Store::save_data(const std::vector<bs_daq::ChannelData>& data)
{
    return;
}