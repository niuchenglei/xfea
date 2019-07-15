#include <gtest/gtest.h>
#include <cstring>

#include "feature_op/single_slot_feature_op.h"
#include "feature_op/S_multi.h"
#include "core/extractor_config.h"
#include "core/log_record_array_impl.h"

using xfea::bisheng::fea_result_t;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_WARNING;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::GlobalParameter;
using xfea::bisheng::S_multi;
using xfea::bisheng::SingleSlotFeatureOp;
using xfea::bisheng::FeatureOpBase;
using xfea::bisheng::fea_op_config_t;
using xfea::bisheng::LogRecordArrayImpl;
using xfea::bisheng::ExtractorConfig;


TEST(S_multi, constructor_and_set_info) {
    S_multi s_multi;
    EXPECT_TRUE(0 == strcmp("", s_multi._name.c_str()));
    EXPECT_EQ(0, s_multi._slot);
    EXPECT_TRUE(0 == strcmp("", s_multi._fea_op_name.c_str()));
    EXPECT_EQ(0U, s_multi._depend_col_name_vec.size());
    EXPECT_EQ(0U, s_multi._depend_col_index_vec.size());
    EXPECT_TRUE(0 == strcmp("", s_multi._arg_str.c_str()));
    EXPECT_FALSE(s_multi._is_need_hash);
    EXPECT_EQ(0U, s_multi._hash_range_min);
    EXPECT_EQ(0U, s_multi._hash_range_max);

    EXPECT_TRUE('\0' == s_multi._buf[0]);
    EXPECT_TRUE('_' == s_multi._seperator);

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

    s_multi.set_info(fea_op_config);

    EXPECT_TRUE(0 == fea_op_config.name.compare(s_multi._name));
    EXPECT_EQ(fea_op_config.fea_slot, s_multi._slot);
    EXPECT_TRUE(0 == fea_op_config.fea_op_name.compare(s_multi._fea_op_name));
    EXPECT_EQ(1U, s_multi._depend_col_name_vec.size());
    EXPECT_TRUE(0 == fea_op_config.depend_col_name_vec[0].compare(s_multi._depend_col_name_vec[0]));
    EXPECT_EQ(1U, s_multi._depend_col_index_vec.size());
    EXPECT_EQ(fea_op_config.depend_col_index_vec[0], s_multi._depend_col_index_vec[0]);
    EXPECT_TRUE(0 == fea_op_config.arg_str.compare(s_multi._arg_str));
    EXPECT_TRUE(s_multi._is_need_hash);
    EXPECT_EQ(fea_op_config.hash_range_min, s_multi._hash_range_min);
    EXPECT_EQ(fea_op_config.hash_range_max, s_multi._hash_range_max);
}

TEST(S_multi, init) {
    ExtractorConfig extractor_config;
    FeaResultSet fea_result_set;
    S_multi s_multi;
    
    EXPECT_TRUE(RC_ERROR == s_multi.init(extractor_config));
    s_multi._depend_col_index_vec.push_back(3);
    EXPECT_TRUE(RC_SUCCESS == s_multi.init(extractor_config));
    EXPECT_TRUE('_' == s_multi._seperator);

    s_multi._arg_str = ":";
    EXPECT_TRUE(RC_SUCCESS == s_multi.init(extractor_config));
    EXPECT_TRUE(':' == s_multi._seperator);
    EXPECT_TRUE('\0' == s_multi._buf[0]);

    s_multi._depend_col_index_vec.push_back(5);
    EXPECT_TRUE(RC_SUCCESS == s_multi.init(extractor_config));

}

TEST(S_multi, generate_fea) {
    ExtractorConfig extractor_config;
    FeaResultSet fea_result_set;
    LogRecordArrayImpl record;
    record._expected_field_num = 2;

    S_multi s_multi;
    s_multi._depend_col_index_vec.push_back(1);
    s_multi._depend_col_index_vec.push_back(0);
    s_multi._slot = 100;
    s_multi._arg_str = ":";
    EXPECT_TRUE(RC_SUCCESS == s_multi.init(extractor_config));

    EXPECT_TRUE(RC_WARNING == s_multi.generate_fea(record, fea_result_set));

    record.fill_value("mid", 1);
    record.fill_value("adid", 0);
    EXPECT_TRUE(RC_SUCCESS == s_multi.generate_fea(record, fea_result_set));
    EXPECT_EQ(1, fea_result_set._fea_result_num);
    EXPECT_EQ(100, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "mid:adid"));
    EXPECT_FALSE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[0].final_fea_sign);

    fea_result_set.reset();
    s_multi._is_need_hash = true;
    EXPECT_TRUE(RC_SUCCESS == s_multi.generate_fea(record, fea_result_set));
    EXPECT_EQ(1, fea_result_set._fea_result_num);
    EXPECT_EQ(100, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "mid:adid"));
    EXPECT_TRUE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(7555252670203272624UL, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(7555252670203272624UL, fea_result_set._fea_result_array[0].final_fea_sign);
}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
