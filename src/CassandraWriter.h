#ifndef DAQ_PIPELINE_CASSANDRAWRITER_H
#define DAQ_PIPELINE_CASSANDRAWRITER_H

#include <memory>
#include <string>
#include <vector>
#include "cassandra.h"

struct CassandraChannelRow {
    std::string device_name;
    int64_t pulse_id_mod;
    std::string channel_name;
    int64_t pulse_id;

    char* blob;
    std::string type;
    std::vector<uint32_t> shape;
    std::string encoding;
    std::string compression;
};

class CassandraWriter {

public:
    CassandraWriter(std::shared_ptr<const std::vector<std::string>> cluster_ips);

};


#endif