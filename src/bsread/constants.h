#ifndef BSREAD_CONSTANTS_H
#define BSREAD_CONSTANTS_H

#include <map>
#include <string>

namespace bsread {

    // 100 characters for 6 attributes
    static const size_t MAX_HEADER_LEN = 100 * 6;

    // 200 characters for 200 channels.
    static const size_t MAX_DATA_HEADER_LEN = 200 * 200;

    struct timestamp{

        timestamp(void): sec(0), nsec(0) {};
        timestamp(uint64_t sec, uint64_t nsec) : sec(sec), nsec(nsec) {};

        /**
         * @brief sec seconds past UNIX epoch (1/1/1970)
         */
        uint64_t sec;

        /**
         * @brief ns nanosecond offset since last full second
         */
        uint64_t nsec;
    };

    enum bsdata_type {
            BSDATA_STRING,
            BSDATA_BOOl,
            BSDATA_FLOAT64,
            BSDATA_FLOAT32,
            BSDATA_INT8,
            BSDATA_UINT8,
            BSDATA_INT16,
            BSDATA_UINT16,
            BSDATA_INT32,
            BSDATA_UINT32,
            BSDATA_INT64,
            BSDATA_UINT64};

    static const size_t bsdata_type_size[] = {
            1,
            1,
            8,
            4,
            1,
            1,
            2,
            2,
            4,
            4,
            8,
            8
    };

    static const char* const bsdata_type_name[] = {
            "string",
            "bool",
            "float64",
            "float32",
            "int8",
            "uint8",
            "int16",
            "uint16",
            "int32",
            "uint32",
            "int64",
            "uint64"};

    static const std::map<std::string, bsdata_type> bsdata_type_mapping = {
            {"string", BSDATA_STRING},
            {"bool", BSDATA_BOOl},
            {"float64",BSDATA_FLOAT64},
            {"float32",BSDATA_FLOAT32},
            {"int8", BSDATA_INT8},
            {"uint8", BSDATA_UINT8},
            {"int16", BSDATA_INT16},
            {"uint16", BSDATA_UINT16},
            {"int32", BSDATA_INT32},
            {"uint32", BSDATA_UINT32},
            {"int64", BSDATA_INT64},
            {"uint64", BSDATA_UINT64},
    };
}


#endif //BSREAD_CONSTANTS_H
