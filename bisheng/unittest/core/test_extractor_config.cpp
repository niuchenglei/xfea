#include <gtest/gtest.h>
#include <cstring>

#include "core/fea_result_set.h"
#include "core/feature_op_manager.h"
#include "core/log_record_array_impl.h"
#include "core/extractor_config.h"

using xfea::bisheng::FeatureOpManager;
using xfea::bisheng::fea_result_t;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_WARNING;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::GlobalParameter;
using xfea::bisheng::LogRecordArrayImpl;
using xfea::bisheng::ExtractorConfig;
using xfea::bisheng::fea_op_config_t;

TEST(fea_op_config_t, constructor) {
    fea_op_config_t fea_op_config;
    EXPECT_TRUE(0 == fea_op_config.name.compare(""));
    EXPECT_EQ(0, fea_op_config.fea_slot);
    EXPECT_TRUE(0 == fea_op_config.fea_op_name.compare(""));
    EXPECT_TRUE(0U == fea_op_config.depend_col_name_vec.size());
    EXPECT_TRUE(0U == fea_op_config.depend_col_index_vec.size());
    EXPECT_TRUE(0 == fea_op_config.arg_str.compare(""));
    EXPECT_FALSE(fea_op_config.is_need_hash);
    EXPECT_EQ(0U, fea_op_config.hash_range_min);
    EXPECT_EQ(0U, fea_op_config.hash_range_max);
}

TEST(ExtractorConfig, constructor) {
    ExtractorConfig config;
    EXPECT_TRUE(0 == config._tag_name.compare(""));
    EXPECT_EQ(0U, config._input_schema_size);
    EXPECT_EQ(0UL, config._feature_global_hash_range_min);
    EXPECT_EQ(0UL, config._feature_global_hash_range_max);
    EXPECT_EQ(0UL, config._input_schema_key_index_map.size());
    EXPECT_EQ(0UL, config._output_add_field_name_vec.size());
    EXPECT_EQ(0UL, config._preprocess_op_name_vec.size());
    EXPECT_EQ(0UL, config._feature_op_config_vec.size());
}

TEST(ExtractorConfig, parse_input_schema) {
    ExtractorConfig config;

    EXPECT_TRUE(RC_ERROR == config.parse_input_schema(""));
    EXPECT_TRUE(RC_ERROR == config.parse_input_schema(",ab,cd"));
    EXPECT_TRUE(RC_ERROR == config.parse_input_schema("aa,,ab,cd"));
    EXPECT_TRUE(RC_ERROR == config.parse_input_schema("ab,ab,cd"));

    EXPECT_TRUE(RC_SUCCESS == config.parse_input_schema("aa, ab,cd"));
    EXPECT_EQ(3U, config._input_schema_size);
    EXPECT_TRUE(0 == config._input_schema_key_index_map["aa"]);
    EXPECT_TRUE(1 == config._input_schema_key_index_map["ab"]);
    EXPECT_TRUE(2 == config._input_schema_key_index_map["cd"]);
}

TEST(ExtractorConfig, parse_output_add_field_name) {
    ExtractorConfig config;

    EXPECT_TRUE(RC_ERROR == config.parse_output_add_field_name(""));
    EXPECT_TRUE(RC_ERROR == config.parse_output_add_field_name(",ab,cd"));
    EXPECT_TRUE(RC_ERROR == config.parse_output_add_field_name("aa,,ab,cd"));
    EXPECT_TRUE(RC_ERROR == config.parse_output_add_field_name("ab,ab,cd"));

    EXPECT_TRUE(RC_SUCCESS == config.parse_output_add_field_name("aa, ab,cd"));
    ASSERT_EQ(3U, config._output_add_field_name_vec.size());
    EXPECT_TRUE(0 == config._output_add_field_name_vec[0].compare("aa"));
    EXPECT_TRUE(0 == config._output_add_field_name_vec[1].compare("ab"));
    EXPECT_TRUE(0 == config._output_add_field_name_vec[2].compare("cd"));
}

TEST(ExtractorConfig, parse_feature_conf_line) {
    ExtractorConfig config;

    std::set<int32_t> exist_slot_set;
    fea_op_config_t fea_op_config;

    std::string line_input = "  ";
    EXPECT_TRUE(RC_SUCCESS == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));
    line_input = " ##  ";
    EXPECT_TRUE(RC_SUCCESS == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    line_input = "ab=1;";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));
    line_input = "name=bias;class=S_bias";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    line_input = "ame=bias;class=S_bias;slot=1";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));
    line_input = "name=;class=S_bias;slot=1";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    line_input = "name=bias;class=S_bias;slot=a";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    exist_slot_set.insert(1);
    line_input = "name=bias;class=S_bias;slot=1";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    exist_slot_set.clear();
    line_input = "name=bias;class=S_bias;slot=0";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    line_input = "name=bias;class=S_bias;slot=1";
    EXPECT_TRUE(RC_SUCCESS == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));
    EXPECT_TRUE(0 == fea_op_config.name.compare("bias"));
    EXPECT_TRUE(1 == fea_op_config.fea_slot);
    EXPECT_TRUE(0 == fea_op_config.fea_op_name.compare("S_bias"));
    EXPECT_TRUE(0U == fea_op_config.depend_col_name_vec.size());
    EXPECT_TRUE(0U == fea_op_config.depend_col_index_vec.size());
    EXPECT_TRUE(0 == fea_op_config.arg_str.compare(""));
    EXPECT_FALSE(fea_op_config.is_need_hash);
    EXPECT_EQ(0UL, fea_op_config.hash_range_min);
    EXPECT_EQ(0UL, fea_op_config.hash_range_max);


    line_input = "name=Mid_Adid;class=S_multi;slot=2;depend=mid,adid;arg=_;is_hash=true";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    exist_slot_set.clear();
    config._input_schema_key_index_map.insert(std::pair<std::string, int>("mid", 0));
    config._input_schema_key_index_map.insert(std::pair<std::string, int>("adid", 1));
    EXPECT_TRUE(RC_SUCCESS == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));
    EXPECT_TRUE(0 == fea_op_config.name.compare("Mid_Adid"));
    EXPECT_TRUE(2 == fea_op_config.fea_slot);
    EXPECT_TRUE(0 == fea_op_config.fea_op_name.compare("S_multi"));
    ASSERT_TRUE(2U == fea_op_config.depend_col_name_vec.size());
    EXPECT_TRUE(0 == fea_op_config.depend_col_name_vec[0].compare("mid"));
    EXPECT_TRUE(0 == fea_op_config.depend_col_name_vec[1].compare("adid"));
    ASSERT_TRUE(2U == fea_op_config.depend_col_index_vec.size());
    EXPECT_TRUE(0 == fea_op_config.depend_col_index_vec[0]);
    EXPECT_TRUE(1 == fea_op_config.depend_col_index_vec[1]);
    EXPECT_TRUE(0 == fea_op_config.arg_str.compare("_"));
    EXPECT_TRUE(fea_op_config.is_need_hash);
    EXPECT_EQ(0UL, fea_op_config.hash_range_min);
    EXPECT_EQ(0UL, fea_op_config.hash_range_max);

    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    exist_slot_set.clear();
    line_input = "name=Mid_Adid;class=S_mult;slot=2;depend=mid,adid;arg=_;is_hash=true;hash_range_min=1000;hash_range_max=10";
    EXPECT_TRUE(RC_ERROR == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));

    line_input = "name=Mid_Adid;class=S_mult;slot=3;depend=mid,adid;arg=_;is_hash=true;hash_range_min=10;hash_range_max=1001";
    EXPECT_TRUE(RC_SUCCESS == config.parse_feature_conf_line(line_input, exist_slot_set, fea_op_config));
    EXPECT_TRUE(0 == fea_op_config.name.compare("Mid_Adid"));
    EXPECT_TRUE(3 == fea_op_config.fea_slot);
    EXPECT_TRUE(0 == fea_op_config.fea_op_name.compare("S_mult"));
    ASSERT_TRUE(2U == fea_op_config.depend_col_name_vec.size());
    EXPECT_TRUE(0 == fea_op_config.depend_col_name_vec[0].compare("mid"));
    EXPECT_TRUE(0 == fea_op_config.depend_col_name_vec[1].compare("adid"));
    ASSERT_TRUE(2U == fea_op_config.depend_col_index_vec.size());
    EXPECT_TRUE(0 == fea_op_config.depend_col_index_vec[0]);
    EXPECT_TRUE(1 == fea_op_config.depend_col_index_vec[1]);
    EXPECT_TRUE(0 == fea_op_config.arg_str.compare("_"));
    EXPECT_TRUE(fea_op_config.is_need_hash);
    EXPECT_EQ(10UL, fea_op_config.hash_range_min);
    EXPECT_EQ(1001UL, fea_op_config.hash_range_max);

}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
