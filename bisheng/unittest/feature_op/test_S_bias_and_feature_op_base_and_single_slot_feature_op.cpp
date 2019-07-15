#include <gtest/gtest.h>
#include <cstring>

#include "feature_op/S_bias.h"
#include "feature_op/single_slot_feature_op.h"
#include "core/extractor_config.h"
#include "core/log_record_array_impl.h"

using xfea::bisheng::fea_result_t;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_WARNING;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::GlobalParameter;
using xfea::bisheng::S_bias;
using xfea::bisheng::SingleSlotFeatureOp;
using xfea::bisheng::FeatureOpBase;
using xfea::bisheng::fea_op_config_t;
using xfea::bisheng::LogRecordArrayImpl;


TEST(FeatureOpBase, constructor_and_set_info) {
    S_bias s_bias;
    EXPECT_TRUE(0 == strcmp("", s_bias._name.c_str()));
    EXPECT_EQ(0, s_bias._slot);
    EXPECT_TRUE(0 == strcmp("", s_bias._fea_op_name.c_str()));
    EXPECT_EQ(0U, s_bias._depend_col_name_vec.size());
    EXPECT_EQ(0U, s_bias._depend_col_index_vec.size());
    EXPECT_TRUE(0 == strcmp("", s_bias._arg_str.c_str()));
    EXPECT_FALSE(s_bias._is_need_hash);
    EXPECT_EQ(0U, s_bias._hash_range_min);
    EXPECT_EQ(0U, s_bias._hash_range_max);

    fea_op_config_t fea_op_config;
    fea_op_config.name = "bias";
    fea_op_config.fea_slot = 1;
    fea_op_config.fea_op_name = "S_bias";
    fea_op_config.depend_col_name_vec.push_back("b");
    fea_op_config.depend_col_index_vec.push_back(1);
    fea_op_config.arg_str = "no";
    fea_op_config.is_need_hash = true;
    fea_op_config.hash_range_min = 100;
    fea_op_config.hash_range_max = 10000;

    s_bias.set_info(fea_op_config);

    EXPECT_TRUE(0 == fea_op_config.name.compare(s_bias._name));
    EXPECT_EQ(fea_op_config.fea_slot, s_bias._slot);
    EXPECT_TRUE(0 == fea_op_config.fea_op_name.compare(s_bias._fea_op_name));
    EXPECT_EQ(1U, s_bias._depend_col_name_vec.size());
    EXPECT_TRUE(0 == fea_op_config.depend_col_name_vec[0].compare(s_bias._depend_col_name_vec[0]));
    EXPECT_EQ(1U, s_bias._depend_col_index_vec.size());
    EXPECT_EQ(fea_op_config.depend_col_index_vec[0], s_bias._depend_col_index_vec[0]);
    EXPECT_TRUE(0 == fea_op_config.arg_str.compare(s_bias._arg_str));
    EXPECT_TRUE(s_bias._is_need_hash);
    EXPECT_EQ(fea_op_config.hash_range_min, s_bias._hash_range_min);
    EXPECT_EQ(fea_op_config.hash_range_max, s_bias._hash_range_max);
}

TEST(SingleSlotFeatureOp, emit_feature) {
    FeaResultSet fea_result_set;
    S_bias s_bias;
    EXPECT_TRUE(RC_WARNING ==  s_bias.emit_feature(NULL, fea_result_set));
}

TEST(S_bias, generate_fea) {
    S_bias s_bias;

    fea_op_config_t fea_op_config;
    fea_op_config.name = "bias";
    fea_op_config.fea_slot = 1;
    fea_op_config.fea_op_name = "S_bias";
    fea_op_config.depend_col_name_vec.push_back("b");
    fea_op_config.depend_col_index_vec.push_back(1);
    fea_op_config.arg_str = "no";
    fea_op_config.is_need_hash = true;
    fea_op_config.hash_range_min = 100;
    fea_op_config.hash_range_max = 10000;

    s_bias.set_info(fea_op_config);

    LogRecordArrayImpl record;
    FeaResultSet fea_result_set;

    EXPECT_TRUE(RC_SUCCESS == s_bias.generate_fea(record, fea_result_set));
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "bias"));
    EXPECT_EQ(1U, fea_result_set._fea_result_num);
    EXPECT_EQ(1, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_TRUE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(7395324107403852630UL, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(2530UL, fea_result_set._fea_result_array[0].final_fea_sign);

    fea_op_config.is_need_hash = false;
    s_bias.set_info(fea_op_config);
    EXPECT_TRUE(RC_SUCCESS == s_bias.generate_fea(record, fea_result_set));
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "bias"));
    EXPECT_EQ(2U, fea_result_set._fea_result_num);
    EXPECT_EQ(1, fea_result_set._fea_result_array[1].fea_slot);
    EXPECT_FALSE(fea_result_set._fea_result_array[1].is_hash);
    EXPECT_EQ(0U, fea_result_set._fea_result_array[1].origin_fea_sign);
    EXPECT_EQ(0U, fea_result_set._fea_result_array[1].final_fea_sign);

    fea_result_set.reset();
    // when hash_range_max <= hash_range_min
    fea_op_config.is_need_hash = true;
    fea_op_config.hash_range_min = 10000000;
    s_bias.set_info(fea_op_config);

    EXPECT_TRUE(RC_SUCCESS == s_bias.generate_fea(record, fea_result_set));
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "bias"));
    EXPECT_EQ(1U, fea_result_set._fea_result_num);
    EXPECT_EQ(1, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_TRUE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(7395324107403852630UL, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(7395324107403852630UL, fea_result_set._fea_result_array[0].final_fea_sign);
}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
