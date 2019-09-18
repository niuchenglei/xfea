#include "util/bisheng_string_tool.h"

#include <vector>
#include <algorithm>

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

std::string& StringTool::trim(std::string &s) {
    if (s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

int StringTool::get_var_value(const std::string& src_str, const std::string& var_name, std::string& value) {
    if (src_str.empty() || var_name.empty()) {
        return -1;
    }
    std::vector<std::string> str_vec;
    StringTool::tokenize(src_str, "=", str_vec);
    if (str_vec.size() != 2) {
        return -1;
    }
    if (var_name != str_vec[0]) {
        return -1;
    }

    if (str_vec[1].empty()) {
        return -1;
    }
    value = str_vec[1];
    return 0;
}
void StringTool::Split2(std::vector<std::string>& vs, const std::string& line, char dmt) {  
    if (line.empty()) 
        return;
    std::string::size_type p = 0;
    std::string::size_type q;
    vs.clear();
    for (;;) {
      q = line.find(dmt, p);
      std::string str = line.substr(p, q - p);
      vs.push_back(str);
      if (q == std::string::npos) break;
      p = q + 1;
    }
}
int StringTool::tokenize(const std::string& src, const std::string& tok, std::vector<std::string>& tokens) {
    if (src.empty() || tok.empty()) {
        return -1;
    }

    std::size_t pre_index = 0;
    std::size_t index = 0;
    std::size_t len = 0;
    std::string tmp_str = "";
    while ((index = src.find_first_of(tok, pre_index)) != std::string::npos) {
        if ((len = index - pre_index) != 0) {
            tmp_str = src.substr(pre_index, len);
            trim(tmp_str);
            tokens.push_back(tmp_str);
        } else {
            XFEA_BISHENG_FATAL_LOG("find empty token in StringTool::tokenize!");
            return -1;
        }
        pre_index = index + 1;
    }
    std::string endstr = src.substr(pre_index);
    trim(endstr);
    if (endstr.empty()) {
        XFEA_BISHENG_FATAL_LOG("find empty token in StringTool::tokenize!");
        return -1;
    } else {
        tokens.push_back(endstr);
    }

    return 0;
}

int StringTool::StringToUint64(const std::string& input_str, uint64_t& return_value) {
    const char* value = input_str.c_str();
    char* end;
    uint64_t n = strtoull(value, &end, 0);
    if (end > value) {
        return_value =  n;
        return 0;
    } else {
        return -1;
    }
}

int StringTool::StringToInt32(const char* input_str, int32_t& return_value) {
    char* end;
    int32_t n = strtol(input_str, &end, 0);
    if (end > input_str) {
        return_value = n;
        return 0;
    } else {
        return -1;
    }
}

int StringTool::StringToInt32(const std::string& input_str, int32_t& return_value) {
    const char* value = input_str.c_str();
    char* end;
    int32_t n = strtol(value, &end, 0);
    if (end > value) {
        return_value = n;
        return 0;
    } else {
        return -1;
    }
}

int StringTool::StringToDouble(const std::string& input_str, double& return_value) {
    const char* value = input_str.c_str();
    char* end;
    double n = strtod(value, &end);
    if (end > value) {
        return_value = n;
        return 0;
    } else {
        return -1;
    }
}

int StringTool::StringToBool(const std::string& input_str, bool& return_value) {
    std::string valstr = input_str;
    std::transform(input_str.begin(), input_str.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1") {
        return_value = true;
        return 0;
    } else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0") {
        return_value = false;
        return 0;
    } else {
        return -1;
    }
}

XFEA_BISHENG_NAMESPACE_GUARD_END

