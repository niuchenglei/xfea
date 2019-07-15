#include <gtest/gtest.h>
#include <cstring>

#include "feature_op/single_slot_feature_op.h"
#include "feature_op/S_ctr_bin.h"
#include "core/extractor_config.h"
#include "core/log_record_array_impl.h"

using xfea::bisheng::fea_result_t;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_WARNING;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::GlobalParameter;
using xfea::bisheng::S_ctr_bin;
using xfea::bisheng::SingleSlotFeatureOp;
using xfea::bisheng::FeatureOpBase;
using xfea::bisheng::fea_op_config_t;
using xfea::bisheng::LogRecordArrayImpl;
using xfea::bisheng::ExtractorConfig;


TEST(S_ctr_bin, constructor_and_set_info) {
    S_ctr_bin s_ctr_bin;
    EXPECT_TRUE(0 == strcmp("", s_ctr_bin._name.c_str()));
    EXPECT_EQ(0, s_ctr_bin._slot);
    EXPECT_TRUE(0 == strcmp("", s_ctr_bin._fea_op_name.c_str()));
    EXPECT_EQ(0U, s_ctr_bin._depend_col_name_vec.size());
    EXPECT_EQ(0U, s_ctr_bin._depend_col_index_vec.size());
    EXPECT_TRUE(0 == strcmp("", s_ctr_bin._arg_str.c_str()));
    EXPECT_FALSE(s_ctr_bin._is_need_hash);
    EXPECT_EQ(0U, s_ctr_bin._hash_range_min);
    EXPECT_EQ(0U, s_ctr_bin._hash_range_max);

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

    s_ctr_bin.set_info(fea_op_config);

    EXPECT_TRUE(0 == fea_op_config.name.compare(s_ctr_bin._name));
    EXPECT_EQ(fea_op_config.fea_slot, s_ctr_bin._slot);
    EXPECT_TRUE(0 == fea_op_config.fea_op_name.compare(s_ctr_bin._fea_op_name));
    EXPECT_EQ(1U, s_ctr_bin._depend_col_name_vec.size());
    EXPECT_TRUE(0 == fea_op_config.depend_col_name_vec[0].compare(s_ctr_bin._depend_col_name_vec[0]));
    EXPECT_EQ(1U, s_ctr_bin._depend_col_index_vec.size());
    EXPECT_EQ(fea_op_config.depend_col_index_vec[0], s_ctr_bin._depend_col_index_vec[0]);
    EXPECT_TRUE(0 == fea_op_config.arg_str.compare(s_ctr_bin._arg_str));
    EXPECT_TRUE(s_ctr_bin._is_need_hash);
    EXPECT_EQ(fea_op_config.hash_range_min, s_ctr_bin._hash_range_min);
    EXPECT_EQ(fea_op_config.hash_range_max, s_ctr_bin._hash_range_max);
}

TEST(S_ctr_bin, parse_ctr_mapping_line) {
    S_ctr_bin s_ctr_bin;
    std::vector<std::string> field_vec;
    EXPECT_EQ(-1, s_ctr_bin.parse_ctr_mapping_line("", "ctr", field_vec));
    EXPECT_EQ(-1, s_ctr_bin.parse_ctr_mapping_line("abc,aa", "ctr", field_vec));
    EXPECT_EQ(-1, s_ctr_bin.parse_ctr_mapping_line("abc,aa,abc", "ctr", field_vec));

    EXPECT_EQ(-1, s_ctr_bin.parse_ctr_mapping_line("ctr,aa,abc", "ctr", field_vec));

    field_vec.clear();
    EXPECT_EQ(0, s_ctr_bin.parse_ctr_mapping_line("ctr,34,100", "ctr", field_vec));
    EXPECT_EQ(1U, s_ctr_bin._ctr_value_vec.size());
    EXPECT_EQ(1U, s_ctr_bin._ctr_value_and_bin_map.size());
    EXPECT_EQ(34, s_ctr_bin._ctr_value_vec[0]);
    EXPECT_EQ(100, s_ctr_bin._ctr_value_and_bin_map[34]);

    field_vec.clear();
    EXPECT_EQ(-1, s_ctr_bin.parse_ctr_mapping_line("ctr,34,10", "ctr", field_vec));
    field_vec.clear();
    EXPECT_EQ(-1, s_ctr_bin.parse_ctr_mapping_line("ctr,32,10", "ctr", field_vec));

    field_vec.clear();
    EXPECT_EQ(0, s_ctr_bin.parse_ctr_mapping_line("ctr,39,105", "ctr", field_vec));
    EXPECT_EQ(2U, s_ctr_bin._ctr_value_vec.size());
    EXPECT_EQ(2U, s_ctr_bin._ctr_value_and_bin_map.size());
    EXPECT_EQ(34, s_ctr_bin._ctr_value_vec[0]);
    EXPECT_EQ(100, s_ctr_bin._ctr_value_and_bin_map[34]);
    EXPECT_EQ(39, s_ctr_bin._ctr_value_vec[1]);
    EXPECT_EQ(105, s_ctr_bin._ctr_value_and_bin_map[39]);
}

TEST(S_ctr_bin, find_approximate_value) {
    S_ctr_bin s_ctr_bin;
    std::vector<int32_t> field_vec;
    field_vec.push_back(3);
    field_vec.push_back(10);
    field_vec.push_back(20);
    field_vec.push_back(50);
    EXPECT_EQ(3, s_ctr_bin.find_approximate_value(field_vec, 2));
    EXPECT_EQ(3, s_ctr_bin.find_approximate_value(field_vec, 5));
    EXPECT_EQ(10, s_ctr_bin.find_approximate_value(field_vec, 8));
    EXPECT_EQ(10, s_ctr_bin.find_approximate_value(field_vec, 13));
    EXPECT_EQ(20, s_ctr_bin.find_approximate_value(field_vec, 17));
    EXPECT_EQ(20, s_ctr_bin.find_approximate_value(field_vec, 23));
    EXPECT_EQ(50, s_ctr_bin.find_approximate_value(field_vec, 40));
    EXPECT_EQ(50, s_ctr_bin.find_approximate_value(field_vec, 51));
    EXPECT_EQ(50, s_ctr_bin.find_approximate_value(field_vec, 501));
}

TEST(S_ctr_bin, find_ctr_map) {
    S_ctr_bin s_ctr_bin;
    s_ctr_bin._ctr_value_vec.push_back(3);
    s_ctr_bin._ctr_value_vec.push_back(6);
    s_ctr_bin._ctr_value_vec.push_back(20);
    s_ctr_bin._ctr_value_vec.push_back(50);
    s_ctr_bin._ctr_value_and_bin_map.insert(std::pair<int32_t, int32_t>(3, 1));
    s_ctr_bin._ctr_value_and_bin_map.insert(std::pair<int32_t, int32_t>(6, 2));
    s_ctr_bin._ctr_value_and_bin_map.insert(std::pair<int32_t, int32_t>(20, 3));
    s_ctr_bin._ctr_value_and_bin_map.insert(std::pair<int32_t, int32_t>(50, 4));

    EXPECT_EQ(-1, s_ctr_bin.find_ctr_map("abc"));

    EXPECT_EQ(1, s_ctr_bin.find_ctr_map("3"));
    EXPECT_EQ(2, s_ctr_bin.find_ctr_map("5"));
    EXPECT_EQ(3, s_ctr_bin.find_ctr_map("20"));
    EXPECT_EQ(4, s_ctr_bin.find_ctr_map("50"));

    EXPECT_EQ(1, s_ctr_bin.find_ctr_map("2"));
    EXPECT_EQ(1, s_ctr_bin.find_ctr_map("4"));
    EXPECT_EQ(2, s_ctr_bin.find_ctr_map("10"));
    EXPECT_EQ(3, s_ctr_bin.find_ctr_map("19"));
    EXPECT_EQ(3, s_ctr_bin.find_ctr_map("29"));
    EXPECT_EQ(4, s_ctr_bin.find_ctr_map("39"));
    EXPECT_EQ(4, s_ctr_bin.find_ctr_map("59"));
    EXPECT_EQ(4, s_ctr_bin.find_ctr_map("509"));
}

TEST(S_ctr_bin, init) {
    ExtractorConfig extractor_config;
    FeaResultSet fea_result_set;
    S_ctr_bin s_ctr_bin;
    
    EXPECT_TRUE(RC_ERROR == s_ctr_bin.init(extractor_config));

    s_ctr_bin._depend_col_index_vec.push_back(3);
    EXPECT_TRUE(RC_ERROR == s_ctr_bin.init(extractor_config));

    s_ctr_bin._arg_str = "ctr_bin_file,hi_ctr";
    EXPECT_TRUE(RC_ERROR == s_ctr_bin.init(extractor_config));

    extractor_config._config_reader._values.insert(std::pair<std::string, std::string>(".ctr_bin_file", "conf/ctr_bin_test_file.txt"));
    ASSERT_TRUE(RC_SUCCESS == s_ctr_bin.init(extractor_config));

    ASSERT_EQ(3U, s_ctr_bin._ctr_value_vec.size());
    ASSERT_EQ(3U, s_ctr_bin._ctr_value_and_bin_map.size());

    EXPECT_EQ(50, s_ctr_bin._ctr_value_vec[0]);
    EXPECT_EQ(100, s_ctr_bin._ctr_value_vec[1]);
    EXPECT_EQ(300, s_ctr_bin._ctr_value_vec[2]);
    EXPECT_EQ(1, s_ctr_bin._ctr_value_and_bin_map[50]);
    EXPECT_EQ(2, s_ctr_bin._ctr_value_and_bin_map[100]);
    EXPECT_EQ(3, s_ctr_bin._ctr_value_and_bin_map[300]);
}

TEST(S_ctr_bin, generate_fea) {
    ExtractorConfig extractor_config;
    FeaResultSet fea_result_set;
    LogRecordArrayImpl record;
    record._expected_field_num = 1;

    S_ctr_bin s_ctr_bin;
    s_ctr_bin._depend_col_index_vec.push_back(0);
    s_ctr_bin._arg_str = "ctr_bin_file,hi_ctr";
    s_ctr_bin._slot = 100;
    extractor_config._config_reader._values.insert(std::pair<std::string, std::string>(".ctr_bin_file", "conf/ctr_bin_test_file.txt"));
    ASSERT_TRUE(RC_SUCCESS == s_ctr_bin.init(extractor_config));

    EXPECT_TRUE(RC_WARNING == s_ctr_bin.generate_fea(record, fea_result_set));

    record.fill_value("49", 0);
    EXPECT_TRUE(RC_SUCCESS == s_ctr_bin.generate_fea(record, fea_result_set));
    EXPECT_EQ(1, fea_result_set._fea_result_num);
    EXPECT_EQ(100, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "1"));
    EXPECT_FALSE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(0UL, fea_result_set._fea_result_array[0].final_fea_sign);

    fea_result_set.reset();
    s_ctr_bin._is_need_hash = true;
    EXPECT_TRUE(RC_SUCCESS == s_ctr_bin.generate_fea(record, fea_result_set));
    EXPECT_EQ(1, fea_result_set._fea_result_num);
    EXPECT_EQ(100, fea_result_set._fea_result_array[0].fea_slot);
    EXPECT_TRUE(0 == strcmp(fea_result_set._fea_result_array[0].fea_text, "1"));
    EXPECT_TRUE(fea_result_set._fea_result_array[0].is_hash);
    EXPECT_EQ(static_cast<int64_t>(100) << 32 | 1, fea_result_set._fea_result_array[0].origin_fea_sign);
    EXPECT_EQ(static_cast<int64_t>(100) << 32 | 1, fea_result_set._fea_result_array[0].final_fea_sign);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
