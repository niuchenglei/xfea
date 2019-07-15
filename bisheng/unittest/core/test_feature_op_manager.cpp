#include <gtest/gtest.h>
#include <cstring>

#include "core/fea_result_set.h"
#include "core/feature_op_manager.h"
#include "core/log_record_array_impl.h"
#include "feature_op/S_bias.h"
#include "feature_op/S_direct.h"
#include "feature_op/S_multi.h"

using xfea::bisheng::FeatureOpManager;
using xfea::bisheng::fea_result_t;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_WARNING;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::GlobalParameter;
using xfea::bisheng::S_bias;
using xfea::bisheng::S_direct;
using xfea::bisheng::S_multi;
using xfea::bisheng::LogRecordArrayImpl;


TEST(FeatureOpManager, constructor) {
    FeatureOpManager feature_op_manager;
    EXPECT_TRUE(NULL == feature_op_manager._extractor_config);
    EXPECT_EQ(0U, feature_op_manager._fea_op_vec.size());
}

TEST(FeatureOpManager, extract_features) {
    LogRecordArrayImpl record;
    FeaResultSet fea_result_set;
    FeatureOpManager feature_op_manager;

    S_bias s_bias;
    s_bias._slot = 2;

    S_direct s_direct;
    s_direct._slot = 10;
    s_direct._depend_col_index_vec.push_back(0);

    S_multi s_multi;
    s_multi._slot = 5;
    s_multi._depend_col_index_vec.push_back(1);
    s_multi._depend_col_index_vec.push_back(0);

    S_direct s_direct2;
    s_direct2._slot = 6;
    s_direct2._depend_col_index_vec.push_back(1);

    feature_op_manager._fea_op_vec.push_back(&s_bias);
    feature_op_manager._fea_op_vec.push_back(&s_direct);
    feature_op_manager._fea_op_vec.push_back(&s_multi);
    feature_op_manager._fea_op_vec.push_back(&s_direct2);

    EXPECT_TRUE(RC_WARNING == feature_op_manager.extract_features(record, fea_result_set));
    
    fea_result_set.reset();
    record._expected_field_num = 2;
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("first", 0));
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("second", 1));

    EXPECT_TRUE(RC_SUCCESS == feature_op_manager.extract_features(record, fea_result_set));
    EXPECT_EQ(4U, fea_result_set._fea_result_num);

    EXPECT_TRUE(0 == strcmp("bias", fea_result_set._fea_result_array[0].fea_text));
    EXPECT_EQ(2, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_FALSE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[0].final_fea_sign);

    EXPECT_TRUE(0 == strcmp("first", fea_result_set._fea_result_array[1].fea_text));
    EXPECT_EQ(10, fea_result_set._fea_result_array[1].fea_slot);
    EXPECT_FALSE(fea_result_set._fea_result_array[1].is_hash);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[1].origin_fea_sign);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[1].final_fea_sign);

    EXPECT_TRUE(0 == strcmp("second_first", fea_result_set._fea_result_array[2].fea_text));
    EXPECT_EQ(5, fea_result_set._fea_result_array[2].fea_slot);
    EXPECT_FALSE(fea_result_set._fea_result_array[2].is_hash);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[2].origin_fea_sign);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[2].final_fea_sign);

    EXPECT_TRUE(0 == strcmp("second", fea_result_set._fea_result_array[3].fea_text));
    EXPECT_EQ(6, fea_result_set._fea_result_array[3].fea_slot);
    EXPECT_FALSE(fea_result_set._fea_result_array[3].is_hash);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[3].origin_fea_sign);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[3].final_fea_sign);
}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
