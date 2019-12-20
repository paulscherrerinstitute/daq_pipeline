#ifndef BSREAD_CONSTANTS_H
#define BSREAD_CONSTANTS_H

#include <map>
#include <string>

namespace bsread {

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

    static const std::map<std::string, size_t> bsdata_type_n_bytes = {
            {"string",  1},
            {"bool",    1},
            {"float64", 8},
            {"float32", 4},
            {"int8",    1},
            {"uint8",   1},
            {"int16",   2},
            {"uint16",  2},
            {"int32",   4},
            {"uint32",  4},
            {"int64",   8},
            {"uint64",  8},
    };
}


#endif //BSREAD_CONSTANTS_H
