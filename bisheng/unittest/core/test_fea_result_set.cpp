#include <gtest/gtest.h>
#include <cstring>

#include "core/fea_result_set.h"

using xfea::bisheng::fea_result_t;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_WARNING;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::GlobalParameter;


TEST(fea_result_t, constructor) {
    fea_result_t fea_result;
    EXPECT_TRUE('\0' == fea_result.fea_text[0]);
    EXPECT_EQ(0, fea_result.fea_slot);
    EXPECT_FALSE(fea_result.is_hash);
    EXPECT_EQ(0UL, fea_result.origin_fea_sign);
    EXPECT_EQ(0UL, fea_result.final_fea_sign);
}

TEST(FeaResultSet, constructor) {
    FeaResultSet fea_result_set;
    EXPECT_EQ(0, fea_result_set._fea_result_num);
    for (unsigned int i = 0; i < GlobalParameter::kMaxFeaResultNum; ++i) {
        EXPECT_TRUE('\0' == fea_result_set._fea_result_array[i].fea_text[0]);
        EXPECT_EQ(0, fea_result_set._fea_result_array[i].fea_slot);
        EXPECT_FALSE(fea_result_set._fea_result_array[i].is_hash);
        EXPECT_EQ(0UL, fea_result_set._fea_result_array[i].origin_fea_sign);
        EXPECT_EQ(0UL, fea_result_set._fea_result_array[i].final_fea_sign);
    }
}

TEST(FeaResultSet, reset) {
    FeaResultSet fea_result_set;
    snprintf(fea_result_set._fea_result_array[0].fea_text, GlobalParameter::kMaxFeaTextResultSize, "%s", "1:2793:mid");
    fea_result_set._fea_result_array[0].fea_slot = 1;
    fea_result_set._fea_result_array[0].is_hash = false;

    fea_result_set.reset();
    EXPECT_EQ(0, fea_result_set._fea_result_num);
    for (unsigned int i = 0; i < GlobalParameter::kMaxFeaResultNum; ++i) {
        EXPECT_TRUE('\0' == fea_result_set._fea_result_array[i].fea_text[0]);
        EXPECT_EQ(0, fea_result_set._fea_result_array[i].fea_slot);
        EXPECT_FALSE(fea_result_set._fea_result_array[i].is_hash);
        EXPECT_EQ(0UL, fea_result_set._fea_result_array[i].origin_fea_sign);
        EXPECT_EQ(0UL, fea_result_set._fea_result_array[i].final_fea_sign);
    }
}

TEST(FeaResultSet, fill_and_get_value) {
    FeaResultSet fea_result_set;
    EXPECT_TRUE(RC_WARNING == fea_result_set.fill_fea_result(NULL, 1, false, 123, 456));
    EXPECT_EQ(0, fea_result_set._fea_result_num);

    EXPECT_TRUE(RC_SUCCESS == fea_result_set.fill_fea_result("1:234:mid", 1, true, 123, 456));
    EXPECT_EQ(1, fea_result_set._fea_result_num);
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "1:234:mid"));
    EXPECT_EQ(1, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_TRUE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(123UL, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(456UL, fea_result_set._fea_result_array[0].final_fea_sign);

    EXPECT_TRUE(RC_SUCCESS == fea_result_set.fill_fea_result("2:234:adid", 2));
    EXPECT_EQ(2, fea_result_set._fea_result_num);
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[1].fea_text, "2:234:adid"));
    EXPECT_EQ(2, fea_result_set._fea_result_array[1].fea_slot);
    EXPECT_FALSE(fea_result_set._fea_result_array[1].is_hash);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[1].origin_fea_sign);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[1].final_fea_sign);

    EXPECT_TRUE(NULL == fea_result_set.get_fea_result(2));
    const fea_result_t* fea_result = fea_result_set.get_fea_result(1);
    EXPECT_TRUE(0 == strcmp(fea_result->fea_text, "2:234:adid"));
    EXPECT_EQ(2, fea_result->fea_slot);
    EXPECT_FALSE(fea_result->is_hash);
    EXPECT_EQ(0UL, fea_result->origin_fea_sign);
    EXPECT_EQ(0UL, fea_result->final_fea_sign);
}

TEST(FeaResultSet, fill_int32_fea_result) {
    FeaResultSet fea_result_set;
    char buf[64];
    for (unsigned int i = 0; i < GlobalParameter::kMaxFeaResultNum; ++i) {
        if (i % 2 == 0) {
            EXPECT_TRUE(RC_SUCCESS == fea_result_set.fill_int32_fea_result(i, i + 1, true, 123, 124));
        } else {
            EXPECT_TRUE(RC_SUCCESS == fea_result_set.fill_int32_fea_result(i, i + 1));
        }
        EXPECT_EQ(static_cast<uint32_t>(i + 1),  fea_result_set.size());
        snprintf(buf, 64, "%d", i); 
        EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[i].fea_text, buf));
        EXPECT_EQ(i + 1, fea_result_set._fea_result_array[i].fea_slot);
        if (i % 2 == 0) {
            EXPECT_TRUE(fea_result_set._fea_result_array[i].is_hash);
            EXPECT_EQ(123UL, fea_result_set._fea_result_array[i].origin_fea_sign);
            EXPECT_EQ(124UL, fea_result_set._fea_result_array[i].final_fea_sign);
        } else {
            EXPECT_FALSE(fea_result_set._fea_result_array[i].is_hash);
            EXPECT_EQ(0UL, fea_result_set._fea_result_array[i].origin_fea_sign);
            EXPECT_EQ(0UL, fea_result_set._fea_result_array[i].final_fea_sign);
        }
    }
    EXPECT_TRUE(RC_WARNING == fea_result_set.fill_int32_fea_result(1, 1, true, 123, 124));
}
// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
