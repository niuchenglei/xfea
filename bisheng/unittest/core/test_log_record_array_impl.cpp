#include <gtest/gtest.h>
#include <cstring>

#include "core/log_record_array_impl.h"
#include "core/extractor_config.h"

using xfea::bisheng::LogRecordArrayImpl;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_WARNING;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::ExtractorConfig;


TEST(LogRecordArrayImpl, constructor) {
    LogRecordArrayImpl record;
    EXPECT_EQ(0, record._tag_name.compare(""));
    EXPECT_EQ(0, record._already_filled_field_num);
    EXPECT_EQ(0, record._expected_field_num);
    EXPECT_TRUE(NULL == record._extractor_config);
}

TEST(LogRecordArrayImpl, fill_value_index) {
    LogRecordArrayImpl record;
    record._expected_field_num = 5;
    EXPECT_EQ(5, record._expected_field_num);

    EXPECT_TRUE(RC_ERROR == record.fill_value(NULL, 0));
    EXPECT_TRUE(RC_ERROR == record.fill_value("abc", 5));
    EXPECT_TRUE(RC_ERROR == record.fill_value("abc", -1));

    int fill_index = 2;
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("abc", fill_index));
    EXPECT_EQ(1, record._already_filled_field_num);
    EXPECT_TRUE(record._is_set[fill_index]);
    EXPECT_TRUE(0 == strcmp("abc", record._value_buf[fill_index]));

    fill_index = 4;
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("efg", fill_index));
    EXPECT_EQ(2, record._already_filled_field_num);
    EXPECT_TRUE(record._is_set[fill_index]);
    EXPECT_TRUE(0 == strcmp("efg", record._value_buf[fill_index]));

    fill_index = 3;
    const char *super_str = "sjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;a;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfaa;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfaa;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfaa;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfaa;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfaa;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfajfj;fsjfa;jfla;jfa;fja;jfa;jfj;fsjfa;jfla;jfa;fja;jfa;jfj;f";
    EXPECT_EQ(RC_WARNING, record.fill_value(super_str, fill_index));
    EXPECT_EQ(2, record._already_filled_field_num);
    EXPECT_FALSE(record._is_set[fill_index]);
}
    
 TEST(LogRecordArrayImpl, is_valid) {
    LogRecordArrayImpl record;
    record._expected_field_num = 1;
    EXPECT_FALSE(record.is_valid());
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("abc", 0));
    EXPECT_TRUE(record.is_valid());
 }

 TEST(LogRecordArrayImpl, get_value_index) {
    LogRecordArrayImpl record;
    record._expected_field_num = 3;
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("abc", 0));
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("def", 1));
     
    EXPECT_TRUE(0 == strcmp("abc", record.get_value(0)));
    EXPECT_TRUE(0 == strcmp("def", record.get_value(1)));
    EXPECT_TRUE(NULL == record.get_value(2));
    EXPECT_TRUE(NULL == record.get_value(-3));
 }

TEST(LogRecordArrayImpl, fill_value_key) {
    ExtractorConfig config;
    config._tag_name = "ctr";
    config._input_schema_size = 2;
    config._input_schema_key_index_map.insert(std::pair<std::string, int>("mid", 0));
    config._input_schema_key_index_map.insert(std::pair<std::string, int>("adid", 1));

    LogRecordArrayImpl record;

    EXPECT_FALSE(RC_SUCCESS == record.init(NULL));
    EXPECT_TRUE(RC_SUCCESS == record.init(&config));

    EXPECT_TRUE(RC_SUCCESS == record.fill_value("afg", "adid"));
    EXPECT_EQ(1, record._already_filled_field_num);
    EXPECT_TRUE(record._is_set[1]);
    EXPECT_TRUE(0 == strcmp("afg", record._value_buf[1]));

    EXPECT_TRUE(RC_SUCCESS == record.fill_value("ffa", "mid"));
    EXPECT_EQ(2, record._already_filled_field_num);
    EXPECT_TRUE(record._is_set[0]);
    EXPECT_TRUE(0 == strcmp("ffa", record._value_buf[0]));

    EXPECT_TRUE(RC_ERROR == record.fill_value("ffa", "psid"));
}

TEST(LogRecordArrayImpl, get_value_key) {
    ExtractorConfig config;
    config._tag_name = "ctr";
    config._input_schema_size = 2;
    config._input_schema_key_index_map.insert(std::pair<std::string, int>("mid", 0));
    config._input_schema_key_index_map.insert(std::pair<std::string, int>("adid", 1));

    LogRecordArrayImpl record;

    EXPECT_TRUE(RC_SUCCESS == record.init(&config));
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("ffa", "mid"));
    EXPECT_TRUE(RC_SUCCESS == record.fill_value("afg", "adid"));

    EXPECT_TRUE(NULL == record.get_value("psid"));
    EXPECT_TRUE(0 == strcmp("ffa", record.get_value("mid")));
    EXPECT_TRUE(0 == strcmp("afg", record.get_value("adid")));
}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
