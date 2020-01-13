#include "gtest/gtest.h"
#include "../src/bsread/BsreadReceiver.cpp"

TEST(blaTest, test1) {
    bsread::BsreadReceiver receiver("tcp://localhost:10000");
    auto data = receiver.get_data()
    EXPECT_EQ (0,  0);
    EXPECT_EQ (20, 20);
    EXPECT_EQ (100, 100);
}