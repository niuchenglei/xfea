#include <gtest/gtest.h>

#include "util/bisheng_string_tool.h"
using xfea::bisheng::StringTool;

TEST(StringTool, trim) {
    std::string src = "";
    EXPECT_TRUE(0 == src.compare(StringTool::trim(src)));
    EXPECT_TRUE(src.empty());

    std::string target_result = "rs";
    std::string src1 = "rs";
    EXPECT_TRUE(0 == target_result.compare(StringTool::trim(src1)));

    std::string src2 = "  rs";
    EXPECT_TRUE(0 == target_result.compare(StringTool::trim(src2)));
    EXPECT_TRUE(0 == target_result.compare(src2));

    std::string src3 = "rs      ";
    EXPECT_TRUE(0 == target_result.compare(StringTool::trim(src3)));
    EXPECT_TRUE(0 == target_result.compare(src3));

    std::string src4 = "    rs      ";
    EXPECT_TRUE(0 == target_result.compare(StringTool::trim(src4)));
    EXPECT_TRUE(0 == target_result.compare(src4));
}

TEST(StringTool, tokenize) {
    std::string src = "";
    std::string tok = ";";
    std::vector<std::string> tokens;

    EXPECT_EQ(-1, StringTool::tokenize("", tok, tokens));
    EXPECT_EQ(0U, tokens.size());
    EXPECT_EQ(-1, StringTool::tokenize(src, "", tokens));
    EXPECT_EQ(0U, tokens.size());

    src = "a;def;d"; 
    EXPECT_EQ(0, StringTool::tokenize(src, tok, tokens));
    EXPECT_EQ(3U, tokens.size());
    EXPECT_TRUE(0 == tokens[0].compare("a"));  
    EXPECT_TRUE(0 == tokens[1].compare("def"));  
    EXPECT_TRUE(0 == tokens[2].compare("d"));  

    tokens.clear();
    src = "a ; def ; d"; 
    EXPECT_EQ(0, StringTool::tokenize(src, tok, tokens));
    EXPECT_EQ(3U, tokens.size());
    EXPECT_TRUE(0 == tokens[0].compare("a"));  
    EXPECT_TRUE(0 == tokens[1].compare("def"));  
    EXPECT_TRUE(0 == tokens[2].compare("d"));  

    src = "a;b;c;";
    EXPECT_EQ(-1, StringTool::tokenize(src, tok, tokens));

    src = "a;b;;c;";
    EXPECT_EQ(-1, StringTool::tokenize(src, tok, tokens));
}

TEST(StringTool, get_var_value) {
    std::string src = "";
    std::string var = "slot";
    std::string value;

    EXPECT_EQ(-1, StringTool::get_var_value(src, var, value));

    var = "";
    EXPECT_EQ(-1, StringTool::get_var_value(src, "", value));
    src = "abc=idef";
    EXPECT_EQ(-1, StringTool::get_var_value(src, var, value));

    var = "slot";
    src = "abcidef";
    EXPECT_EQ(-1, StringTool::get_var_value(src, var, value));
    src = "ab=ci=def";
    EXPECT_EQ(-1, StringTool::get_var_value(src, var, value));
    src = "slot=";
    EXPECT_EQ(-1, StringTool::get_var_value(src, var, value));

    src = "slot=10";
    EXPECT_EQ(0, StringTool::get_var_value(src, var, value));
    EXPECT_TRUE(0 == value.compare("10"));

    src = "slot= 10";
    EXPECT_EQ(0, StringTool::get_var_value(src, var, value));
    EXPECT_TRUE(0 == value.compare("10"));
}

TEST(StringTool, StringToUint64) {
    uint64_t rs = 0;
    EXPECT_EQ(-1, StringTool::StringToUint64("", rs));
    EXPECT_EQ(-1, StringTool::StringToUint64("abc", rs));
    EXPECT_EQ(0, StringTool::StringToUint64("1234a", rs));
    EXPECT_EQ(0, StringTool::StringToUint64("1234u", rs));
    EXPECT_EQ(1234, rs);
    EXPECT_EQ(0, StringTool::StringToUint64("1234", rs));
    EXPECT_EQ(1234, rs);
}

TEST(StringTool, StringToInt32) {
    int32_t rs = 0;
    EXPECT_EQ(-1, StringTool::StringToInt32("", rs));
    EXPECT_EQ(-1, StringTool::StringToInt32("abc", rs));
    EXPECT_EQ(0, StringTool::StringToInt32("1234a", rs));
    EXPECT_EQ(0, StringTool::StringToInt32("1234u", rs));
    EXPECT_EQ(1234, rs);
    EXPECT_EQ(0, StringTool::StringToInt32("-1234", rs));
    EXPECT_EQ(-1234, rs);
}

TEST(StringTool, StringToDouble) {
    double rs = 0;
    EXPECT_EQ(-1, StringTool::StringToDouble("", rs));
    EXPECT_EQ(-1, StringTool::StringToDouble("abc", rs));
    EXPECT_EQ(0, StringTool::StringToDouble("1.234a", rs));
    EXPECT_EQ(1.234, rs);
    EXPECT_EQ(0, StringTool::StringToDouble("-1.234", rs));
    EXPECT_EQ(-1.234, rs);
}

TEST(StringTool, StringToBool) {
    bool rs = false;
    EXPECT_EQ(-1, StringTool::StringToBool("abc", rs));
    EXPECT_EQ(-1, StringTool::StringToBool("", rs));

    EXPECT_EQ(0, StringTool::StringToBool("true", rs));
    EXPECT_TRUE(rs);
    EXPECT_EQ(0, StringTool::StringToBool("TrUe", rs));
    EXPECT_TRUE(rs);
 
    EXPECT_EQ(0, StringTool::StringToBool("false", rs));
    EXPECT_FALSE(rs);
    EXPECT_EQ(0, StringTool::StringToBool("faLse", rs));
    EXPECT_FALSE(rs);
}

// int main(int argc, char **argv) {
//   testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
