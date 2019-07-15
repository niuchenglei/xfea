#include <gtest/gtest.h>

#include "util/bisheng_hash.h"
using xfea::bisheng::HashTool;

TEST(HashTool, create_sign64_beg) {
    uint32_t sig1 = 0;
    uint32_t sig2 = 0;
    HashTool::create_sign64_beg(sig1, sig2, NULL);
    ASSERT_EQ(0xffffffff, sig1); 
    ASSERT_EQ(0xffffffff, sig2); 

    HashTool::create_sign64_beg(sig1, sig2, "");
    ASSERT_EQ(0, sig1);
    ASSERT_EQ(0, sig2);

    HashTool::create_sign64_beg(sig1, sig2, "1");
    ASSERT_EQ(3211264, sig1);
    ASSERT_EQ(380296497, sig2);

    HashTool::create_sign64_beg(sig1, sig2, "11");
    ASSERT_EQ(3211411, sig1);
    ASSERT_EQ(730435111, sig2);
}

TEST(HashTool, create_sign64) {
    uint32_t sig1 = 0;
    uint32_t sig2 = 0;
    HashTool::create_sign64(sig1, sig2, 0, 0, 0);
    ASSERT_EQ(0, sig1);
    ASSERT_EQ(0, sig2);

    HashTool::create_sign64(sig1, sig2, 1, 1, 1);
    ASSERT_EQ(65795, sig1);
    ASSERT_EQ(30731316, sig2);

    HashTool::create_sign64(sig1, sig2, 10, 702804, 297347274);
    ASSERT_EQ(4001334366U, sig1);
    ASSERT_EQ(4280976850U, sig2);
}

TEST(HashTool, hash64) {
    ASSERT_EQ(18446463685360484486UL, HashTool::hash64(NULL, 1));
    ASSERT_EQ(18446464784887936739UL, HashTool::hash64(NULL, 2));
    ASSERT_EQ(12104348807840040571UL, HashTool::hash64("Belta0", 1));
}

TEST(HashTool, hash64_with_int32_value) {
    ASSERT_EQ(static_cast<uint64_t>(1) << 32, HashTool::hash64_with_int32_value(0, 1));
    ASSERT_EQ(static_cast<uint64_t>(15) << 32 | 2000, HashTool::hash64_with_int32_value(2000, 15));
    ASSERT_EQ(64424511440UL, HashTool::hash64_with_int32_value(2000, 15));
}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
