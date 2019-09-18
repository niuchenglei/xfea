#include "core/extractor_config.h"

#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>

#include "util/bisheng_string_tool.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

std::string fea_op_config_t::to_string() const {
    std::ostringstream oss;
    oss << "name=" << name << ";slot=" << fea_slot << ";class=" << fea_op_name << ";depend=";
    for (std::vector<std::string>::const_iterator iter = depend_col_name_vec.begin();
             iter != depend_col_name_vec.end();
             ++iter) {
        oss << *iter << ",";
    }
    oss << ";depend_index=";
    for (std::vector<int>::const_iterator iter = depend_col_index_vec.begin();
             iter != depend_col_index_vec.end();
             ++iter) {
        oss << *iter << ",";
    }
    oss << ";arg=" << arg_str << ";is_hash=" << is_need_hash << ";hash_range_min=" << hash_range_min <<
            ";hash_range_max=" << hash_range_max << "\n";
 
    return oss.str();
}

ReturnCode ExtractorConfig::init(const std::string& config_file_path, const std::string& feature_list_conf_file_path) {
    return load_config(config_file_path, feature_list_conf_file_path);
}

std::string ExtractorConfig::to_string() const {
    std::ostringstream oss;
    oss << "tag_name=" << _tag_name << "\n"
            << "input_schema_size=" << _input_schema_size << "\n";
    for (std::map<std::string, int>::const_iterator iter = _input_schema_key_index_map.begin();
            iter != _input_schema_key_index_map.end();
            ++iter) {
        oss << "  " << iter->first << ":" << iter->second;
    }
    oss << "\n";

    oss << "output_add_field_name:";
    for (std::vector<std::string>::const_iterator iter = _output_add_field_name_vec.begin();
            iter != _output_add_field_name_vec.end();
            ++iter) {
        oss << " " << *iter;
    }
    oss << "\n";

    oss << "feature_default_is_hash: " << _feature_global_is_hash << "\n";
    oss << "feature_default_hash_range_min: " << _feature_global_hash_range_min << "\n";
    oss << "feature_default_hash_range_max: " << _feature_global_hash_range_max<< "\n";

    oss << "feature list size: " << _feature_op_config_vec.size() << "\n";
    for (std::vector<fea_op_config_t>::const_iterator iter = _feature_op_config_vec.begin();
            iter != _feature_op_config_vec.end();
            ++iter) {
        oss << "  " << iter->to_string();
    }
    oss << "\n";

    return oss.str();
}

ReturnCode ExtractorConfig::parse_output_add_field_name(const std::string& str_buf) {
    if (str_buf.empty()) {
        XFEA_BISHENG_FATAL_LOG("input is empty in ExtractorConfig::parse_output_add_field_name!");
        return RC_ERROR;
    }

    std::vector<std::string> tokens;
    int ret = StringTool::tokenize(str_buf, ",", tokens);
    if (0 != ret) {
        XFEA_BISHENG_FATAL_LOG("tokenize string[%s] failed!", str_buf.c_str());
        return RC_ERROR;
    }

    _output_add_field_name_vec.clear();
    std::vector<std::string>::const_iterator iter = tokens.begin();
    for (; iter != tokens.end(); ++iter) {
        // 性能低，但是只调用一次
        if (find(_output_add_field_name_vec.begin(), _output_add_field_name_vec.end(), *iter)
                != _output_add_field_name_vec.end()) {
            XFEA_BISHENG_FATAL_LOG("find duplicated field[%s] in config item output_add_field_name!", iter->c_str());
            return RC_ERROR;
        }
        _output_add_field_name_vec.push_back(*iter);
    }

    return RC_SUCCESS;
}

ReturnCode ExtractorConfig::parse_input_schema(const std::string& input_schema_str_buf) {
    if (input_schema_str_buf.empty()) {
        XFEA_BISHENG_FATAL_LOG("input schema is empty in ExtractorConfig::parse_input_schema!");
        return RC_ERROR;
    }

    std::vector<std::string> tokens;
    int ret = StringTool::tokenize(input_schema_str_buf, ",", tokens);
    if (0 != ret) {
        XFEA_BISHENG_FATAL_LOG("tokenize input schema string[%s] failed!", input_schema_str_buf.c_str());
        return RC_ERROR;
    }

    _input_schema_key_index_map.clear();
    int index = 0;
    std::vector<std::string>::const_iterator iter = tokens.begin();
    for (; iter != tokens.end(); ++iter) {
        if (_input_schema_key_index_map.find(*iter) != _input_schema_key_index_map.end()) {
            XFEA_BISHENG_FATAL_LOG("find duplicated token[%s] in config item input_schema!", iter->c_str());
            return RC_ERROR;
        }
        _input_schema_key_index_map.insert(std::pair<std::string, int>(*iter, index));
        ++index;
    }
    _input_schema_size = _input_schema_key_index_map.size();

    return RC_SUCCESS;
}

ReturnCode ExtractorConfig::parse_feature_conf_line(std::string& line_input, std::set<int32_t>& exist_slot_set, fea_op_config_t& fea_op_config) {
    StringTool::trim(line_input);
    if (line_input.empty() || line_input[0] == '#') {
        return RC_SUCCESS;
    }

    std::vector<std::string> fea_info_vec;
    if (StringTool::tokenize(line_input, ";", fea_info_vec) != 0) {
        XFEA_BISHENG_FATAL_LOG("parsed featrue conf line[%s] failed!", line_input.c_str());
        return RC_ERROR;
    }
    uint32_t fea_info_size = fea_info_vec.size();
    if (fea_info_size < 3 ||  fea_info_size > 9) {
        XFEA_BISHENG_FATAL_LOG("There should be 3 to 9 parts in each line!");
        return RC_ERROR;
    }

    // 解析feature_name
    std::string fea_name = "";
    if (StringTool::get_var_value(fea_info_vec[0], "name", fea_name) != 0) {
        XFEA_BISHENG_FATAL_LOG("get feature name failed for line: [%s]!", line_input.c_str());
        return RC_ERROR;
    }
    fea_op_config.name = fea_name;

    // 解析feature_class_name
    std::string fea_op_name = "";
    if (StringTool::get_var_value(fea_info_vec[1], "class", fea_op_name) != 0) {
        XFEA_BISHENG_FATAL_LOG("get class name failed for feature [%s]!", fea_name.c_str());
        return RC_ERROR;
    }
    fea_op_config.fea_op_name = fea_op_name;

    // 解析slot
    std::string str_slot;
    if (StringTool::get_var_value(fea_info_vec[2], "slot", str_slot) != 0) {
        XFEA_BISHENG_FATAL_LOG("get slot failed for feature [%s]!", fea_name.c_str());
        return RC_ERROR;
    }
    int32_t slot = 0;
    if (StringTool::StringToInt32(str_slot, slot) != 0) {
        XFEA_BISHENG_FATAL_LOG("parse slot failed for feature [%s]!", fea_name.c_str());
        return RC_ERROR;
    }
    if (0 == slot) {
        XFEA_BISHENG_FATAL_LOG("slot[%d] can not be zero!", slot);
        return RC_ERROR;
    }

    // 检查slot的唯一性
    if (exist_slot_set.find(slot) != exist_slot_set.end()) {
        XFEA_BISHENG_FATAL_LOG("slot[%d] duplicated!", slot);
        return RC_ERROR;
    } else {
       fea_op_config.fea_slot = slot;
       exist_slot_set.insert(slot);
    }

    // 解析其他配置
    fea_op_config.depend_col_name_vec.clear();
    fea_op_config.depend_col_index_vec.clear();
    fea_op_config.arg_str = "";
    fea_op_config.is_need_hash = _feature_global_is_hash;
    fea_op_config.hash_range_min = _feature_global_hash_range_min;
    fea_op_config.hash_range_max = _feature_global_hash_range_max;
    uint32_t start_idx = 3;
    while (start_idx < fea_info_size) {
        std::vector<std::string> key_value_vec;
        if (StringTool::tokenize(fea_info_vec[start_idx], "=", key_value_vec) != 0 || key_value_vec.size() != 2) {
            XFEA_BISHENG_FATAL_LOG("parse other field failed for feature [%s]!", fea_name.c_str());
            return RC_ERROR;
        }
        // 讨论：以下配置项重复时，后一个会覆盖前一个
        if (key_value_vec[0].compare("depend") == 0)  {  // 解析depend
            fea_op_config.depend_col_name_vec.clear();
            if (StringTool::tokenize(key_value_vec[1], ",", fea_op_config.depend_col_name_vec) != 0) {
                XFEA_BISHENG_FATAL_LOG("parse depend failed for feature [%s]!", fea_name.c_str());
                return RC_ERROR;
            }
            if (fea_op_config.depend_col_name_vec.size() < 1) {
                XFEA_BISHENG_FATAL_LOG("depend can not be empty for feature [%s]!", fea_name.c_str());
                return RC_ERROR;
            }
            // 查询depend的schema在inut schema对应的index，并存入depend_col_index_vec
            fea_op_config.depend_col_index_vec.clear();
            for (std::vector<std::string>::const_iterator iter = fea_op_config.depend_col_name_vec.begin();
                iter != fea_op_config.depend_col_name_vec.end();
                ++iter) {
                int index = key_to_index(*iter);
                if (index < 0) {
                    XFEA_BISHENG_FATAL_LOG("get index of schema[%s] failed: maybe not in the input schema!", iter->c_str());
                    return RC_ERROR;
                } else {
                    fea_op_config.depend_col_index_vec.push_back(index);
                }
            }
        } else if (key_value_vec[0].compare("arg") == 0) {
            fea_op_config.arg_str = key_value_vec[1];
        } else if (key_value_vec[0].compare("is_hash") == 0) {
            if (StringTool::StringToBool(key_value_vec[1], fea_op_config.is_need_hash) != 0) {
                XFEA_BISHENG_FATAL_LOG("parse is_hash field failed for feature [%s]!", fea_name.c_str());
                return RC_ERROR;
            }
        } else if (key_value_vec[0].compare("hash_range_min") == 0) {
            if (StringTool::StringToUint64(key_value_vec[1], fea_op_config.hash_range_min) != 0) {
                XFEA_BISHENG_FATAL_LOG("parse hash_range_min field failed for feature [%s]!", fea_name.c_str());
                return RC_ERROR;
            }
        } else if (key_value_vec[0].compare("hash_range_max") == 0) {
            if (StringTool::StringToUint64(key_value_vec[1], fea_op_config.hash_range_max) != 0) {
                XFEA_BISHENG_FATAL_LOG("parse hash_range_max field failed for feature [%s]!", fea_name.c_str());
                return RC_ERROR;
            }
        } else {
            XFEA_BISHENG_FATAL_LOG("unknown field name[%s] for feature [%s]!", key_value_vec[0].c_str(), fea_name.c_str());
            return RC_ERROR;
        }
        ++start_idx;
    }
    // hash_range_max必须大于hash_range_min
    if (fea_op_config.hash_range_max <= fea_op_config.hash_range_min && 0 != fea_op_config.hash_range_min) {
        XFEA_BISHENG_FATAL_LOG("wrong hash range set hash_range_max[%lu] <= hash_range_min[%lu]!",
                fea_op_config.hash_range_max, fea_op_config.hash_range_min);
        return RC_ERROR;
    }

    return RC_SUCCESS;
}

ReturnCode ExtractorConfig::load_feature_list(const std::string& feature_list_file_path) {
    std::ifstream feature_conf_file;
    feature_conf_file.open(feature_list_file_path.c_str(), std::ios_base::in);
    if (!feature_conf_file.is_open()) {
        XFEA_BISHENG_FATAL_LOG("open feature_list_conf[%s] failed!", feature_list_file_path.c_str());
        return RC_ERROR;
    }

    // 清空已有的数据
    _feature_op_config_vec.clear();
    std::set<int32_t> _exist_slot_set;
    std::string line;
    while (getline(feature_conf_file, line)) {
        // 讨论：可以直接返回vector中的，这样可以减少一次拷贝
        fea_op_config_t fea_op_config;

        StringTool::trim(line);
        if (strlen(line.c_str()) <= 10)
            continue;
        if (line == "" || line == "\n")
            continue;
	if (line[0] == '#' || line[0] == '\n')
            continue;
        if (parse_feature_conf_line(line, _exist_slot_set, fea_op_config) != RC_SUCCESS) {
            XFEA_BISHENG_FATAL_LOG("parse feature item line[%s] error!", line.c_str());
            feature_conf_file.close();
            return RC_ERROR;
        } else if (!fea_op_config.name.empty()) {  // 非空行
            _feature_op_config_vec.push_back(fea_op_config);
        }
    }
    feature_conf_file.close();

    return RC_SUCCESS;
}

ReturnCode ExtractorConfig::load_config(const std::string& config_file_path, const std::string& feature_list_conf_file_path) {
    int ret = _config_reader.Init(config_file_path);
    if (0 != ret) {
        XFEA_BISHENG_FATAL_LOG("load config file[%s] faield, err[%d]!", config_file_path.c_str(), ret);
        return RC_ERROR;
    }

    // 加载tag name, 为空则取默认值“”
    _config_reader.Get("", "tag_name", _tag_name, "");

    // 加载input_schema
    std::string input_schema_str_buf = "";
    if (_config_reader.Get("", "input_schema", input_schema_str_buf) != 0) {
        XFEA_BISHENG_FATAL_LOG("get config item[input_schema] faield from config file[%s] faield!", config_file_path.c_str());
        return RC_ERROR;
    } else if (parse_input_schema(input_schema_str_buf) != RC_SUCCESS) {
        XFEA_BISHENG_FATAL_LOG("parse_input_schema [%s] failed!", input_schema_str_buf.c_str());
        return RC_ERROR;
    }

    _label_field = "label";
    _config_reader.Get("", "label", _label_field, "label");

    // 加载_output_add_field_name
    std::string output_add_field_str_buf = "";
    _config_reader.Get("", "output_add_field_name", output_add_field_str_buf, "");
    if ((!output_add_field_str_buf.empty()) && parse_output_add_field_name(output_add_field_str_buf) != RC_SUCCESS) {
        XFEA_BISHENG_FATAL_LOG("parse_output_add_field_name [%s] failed!", output_add_field_str_buf.c_str());
        return RC_ERROR;
    }

    // 加载特征相关配置
    ret = _config_reader.GetBoolean("", "feature_default_is_hash", _feature_global_is_hash, true);
    if (0 != ret) {
        XFEA_BISHENG_FATAL_LOG("get config item feature_default_is_hash failed from config file[%s] faield!", config_file_path.c_str());
        return RC_ERROR;
    }

    ret = _config_reader.GetUInt64("", "feature_default_hash_range_min", _feature_global_hash_range_min, 0);
    if (0 != ret) {
        XFEA_BISHENG_FATAL_LOG("get config item feature_default_hash_range_min failed from config file[%s] faield!", config_file_path.c_str());
        return RC_ERROR;
    }
    ret = _config_reader.GetUInt64("", "feature_default_hash_range_max", _feature_global_hash_range_max, 0);
    if (0 != ret) {
        XFEA_BISHENG_FATAL_LOG("get config item feature_default_hash_range_max failed from config file[%s] faield!", config_file_path.c_str());
        return RC_ERROR;
    }
    // 检查hash range
    if (_feature_global_hash_range_max <= _feature_global_hash_range_min && 0 != _feature_global_hash_range_min) {
        XFEA_BISHENG_FATAL_LOG("wrong hash range set hash_range_max[%lu] <= hash_range_min[%lu]!",
               _feature_global_hash_range_max, _feature_global_hash_range_min);
        return RC_ERROR;
    }

    // 记载特征配置列表 
    std::string feature_list_file_path = "";
    if(feature_list_conf_file_path.empty()) {
        if (_config_reader.Get("", "feature_list", feature_list_file_path) != 0) {
            XFEA_BISHENG_FATAL_LOG("get config item feature_list failed from config file[%s] faield!", config_file_path.c_str());
            return RC_ERROR;
        }
    } else {
        feature_list_file_path.assign(feature_list_conf_file_path);
    }
    ret = load_feature_list(feature_list_file_path);
    if (RC_SUCCESS != ret) {
        XFEA_BISHENG_FATAL_LOG("load_feature_list from [%s] failed!", feature_list_file_path.c_str());
        return RC_ERROR;
    }

    return RC_SUCCESS;
}

XFEA_BISHENG_NAMESPACE_GUARD_END

