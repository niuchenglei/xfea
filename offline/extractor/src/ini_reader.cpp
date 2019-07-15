// Read an INI file into easy-to-access name/value pairs.

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include "ini.h"
#include "ini_reader.h"

namespace xfea { namespace offxtractor {

int INIReader::Init(const std::string& filename) {
    return IniTool::ini_parse(filename.c_str(), ValueHandler, this);
}

int INIReader::Get(const std::string& section, const std::string& name, std::string& return_value) const {
    std::string key = MakeKey(section, name);
    std::map<std::string, std::string>::const_iterator iter = _values.find(key);
    if (iter != _values.end()) {
        return_value = iter->second;
        return 0;
    } else {
        return 1;
    }
}

int INIReader::Get(const std::string& section, const std::string& name,
    std::string& return_value, const std::string& default_value) const {
    if (Get(section, name, return_value) == 1) {
        return_value = default_value;
        return 0;
    }

    return 0;
}

int INIReader::GetUInt64(const std::string& section, const std::string& name, uint64_t& return_value) const {
    std::string valstr = "";
    if (Get(section, name, valstr) == 0) {
        const char* value = valstr.c_str();
        char* end;
        uint64_t n = strtoull(value, &end, 0);
        if (end > value) {
            return_value =  n;
            return 0;
        } else {
            return -1;  // 解析出错
        }
     } else {
        return 1;  // 未找到配置项
     }
}

int INIReader::GetUInt64(const std::string& section, const std::string& name,
    uint64_t& return_value, const uint64_t& default_value) const {
    int ret = GetUInt64(section, name, return_value);
    if (1 == ret) {  // 未找到配置项
        return_value = default_value;
        return 0;
    } else if (0 == ret) {  // 找到配置项并解析成功
        return 0;
    } else {  // 解析出错
        return -1;
    }
}

int INIReader::GetReal(const std::string& section, const std::string& name, double& return_value) const {
    std::string valstr = "";
    if (Get(section, name, valstr) == 0) {
        const char* value = valstr.c_str();
        char* end;
        double n = strtod(value, &end);
        if (end > value) {
            return_value =  n;
            return 0;
        } else {
            return -1;  // 解析出错
        }
     } else {
        return 1;  // 未找到配置项
     }
}

int INIReader::GetReal(const std::string& section, const std::string& name,
    double& return_value, const double& default_value) const {
    int ret = GetReal(section, name, return_value);
    if (1 == ret) {  // 未找到配置项
        return_value = default_value;
        return 0;
    } else if (0 == ret) {  // 找到配置项并解析成功
        return 0;
    } else {  // 解析出错
        return -1;
    }
}

int INIReader::GetBoolean(const std::string& section, const std::string& name, bool& return_value) const {
    std::string valstr = "";
    if (Get(section, name, valstr) == 0) {
        std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
        if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1") {
            return_value = true;
            return 0;
        } else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0") {
            return_value = false;
            return 0;
        } else {
            return -1;
        }
    } else {
        return 1;
    }
}

int INIReader::GetBoolean(const std::string& section, const std::string& name,
    bool& return_value, const bool& default_value) const {
    int ret = GetBoolean(section, name, return_value);
    if (1 == ret) {  // 未找到配置项
        return_value = default_value;
        return 0;
    } else if (0 == ret) {  // 到配置项并解析成功
        return 0;
    } else {  // 解析出错
        return -1;
    }
}

std::string INIReader::MakeKey(const std::string& section, const std::string& name) {
    std::string key = section + "." + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value) {
    INIReader* reader = static_cast<INIReader*>(user);
    std::string key = MakeKey(section, name);
    if (reader->_values[key].size() > 0) {  // 不允许有重复的key
        fprintf(stderr, "config item[%s] duplicated!", key.c_str());
        return 0;
    } else {
        reader->_values[key] = value;
        return 1;
    }
}

}}
