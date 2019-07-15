#include <gtest/gtest.h>

#include "util/bisheng_INIReader.h"
using xfea::bisheng::INIReader;

TEST(INIReader, Get) {
    INIReader reader;
    
    std::string value;
    EXPECT_EQ(1, reader.Get("", "key", value));
    EXPECT_EQ(0, reader.Get("", "key", value, "v"));
    EXPECT_TRUE(0 == value.compare("v"));

    reader._values.insert(std::pair<std::string, std::string>(".key", "val"));
    EXPECT_EQ(0, reader.Get("", "key", value));
    EXPECT_TRUE(0 == value.compare("val"));
    EXPECT_EQ(0, reader.Get("", "key", value, "v"));
    EXPECT_TRUE(0 == value.compare("val"));
}

TEST(INIReader, GetUInt64) {
    INIReader reader;
    
    uint64_t value;
    EXPECT_EQ(1, reader.GetUInt64("", "key", value));
    EXPECT_EQ(0, reader.GetUInt64("", "key", value, 5678UL));
    EXPECT_EQ(5678UL, value);

    reader._values.insert(std::pair<std::string, std::string>(".key", "1234"));
    EXPECT_EQ(0, reader.GetUInt64("", "key", value));
    EXPECT_EQ(1234UL, value);
    EXPECT_EQ(0, reader.GetUInt64("", "key", value, 5678UL));
    EXPECT_EQ(1234UL, value);

    reader._values.clear(); 
    reader._values.insert(std::pair<std::string, std::string>(".key", "abc"));
    EXPECT_EQ(-1, reader.GetUInt64("", "key", value));
    EXPECT_EQ(-1, reader.GetUInt64("", "key", value, 5678UL));
}

TEST(INIReader, GetReal) {
    INIReader reader;
    
    double value;
    EXPECT_EQ(1, reader.GetReal("", "key", value));
    EXPECT_EQ(0, reader.GetReal("", "key", value, 5.78));
    EXPECT_EQ(5.78, value);

    reader._values.insert(std::pair<std::string, std::string>(".key", "4.35"));
    EXPECT_EQ(0, reader.GetReal("", "key", value));
    EXPECT_EQ(4.35, value);
    EXPECT_EQ(0, reader.GetReal("", "key", value, 23.23));
    EXPECT_EQ(4.35, value);

    reader._values.clear(); 
    reader._values.insert(std::pair<std::string, std::string>(".key", "abc"));
    EXPECT_EQ(-1, reader.GetReal("", "key", value));
    EXPECT_EQ(-1, reader.GetReal("", "key", value, 23.2));
}

TEST(INIReader, GetBoolean) {
    INIReader reader;
    
    bool value = true;
    EXPECT_EQ(1, reader.GetBoolean("", "key", value));
    EXPECT_EQ(0, reader.GetBoolean("", "key", value, false));
    EXPECT_FALSE(value);

    reader._values.insert(std::pair<std::string, std::string>(".key", "true"));
    EXPECT_EQ(0, reader.GetBoolean("", "key", value));
    EXPECT_TRUE(value);
    value = false;
    EXPECT_EQ(0, reader.GetBoolean("", "key", value, true));
    EXPECT_TRUE(value);

    value = true;
    reader._values.clear(); 
    reader._values.insert(std::pair<std::string, std::string>(".key", "fal"));
    EXPECT_EQ(-1, reader.GetBoolean("", "key", value));
    EXPECT_EQ(-1, reader.GetBoolean("", "key", value, false));
}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
