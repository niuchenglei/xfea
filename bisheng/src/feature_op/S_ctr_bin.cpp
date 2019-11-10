#include "feature_op/S_ctr_bin.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "util/bisheng_string_tool.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

const std::string S_ctr_bin::kSepeartor = ",";

int S_ctr_bin::parse_ctr_mapping_line(const std::string& line, const std::string& ctr_bin_name, std::vector<std::string>& field_vec) {
    if (StringTool::tokenize(line, kSepeartor, field_vec) != 0) { 
        XFEA_BISHENG_FATAL_LOG("StringTool::tokenize the line[%s] failed!", line.c_str());
        return -1;
    }
    if (field_vec.size() != 3) {  // 检查切分结果是否为3列
        XFEA_BISHENG_FATAL_LOG("StringTool::tokenize result vector size is not 3 for line[%s] failed!", line.c_str());
        return -1;
    }
    if (ctr_bin_name.compare(field_vec[0]) != 0) {  // 检查第一个元素值是否为ctr_bin_name
        XFEA_BISHENG_FATAL_LOG("StringTool::tokenize result vector[0] is not expected string[%s] for line[%s]!",
                ctr_bin_name.c_str(), line.c_str());
        return -1;
    }

    int32_t ctr_value = 0;
    int32_t ctr_bin = 0;
    if (StringTool::StringToInt32(field_vec[1], ctr_value) != 0 
            || StringTool::StringToInt32(field_vec[2], ctr_bin) != 0) {
        XFEA_BISHENG_FATAL_LOG("tranform [%s] or [%s] to int32_t failed for line[%s]!", 
                field_vec[1].c_str(), field_vec[2].c_str(), line.c_str());
        return -1;
    }

    if (_ctr_value_and_bin_map.find(ctr_value) != _ctr_value_and_bin_map.end()) {
        XFEA_BISHENG_FATAL_LOG("ctr value[%d] is already in ctr map!", ctr_value);
        return -1;

    }
    if (_ctr_value_vec.size() > 0) {
        if (ctr_value <= _ctr_value_vec[_ctr_value_vec.size() - 1]) {
            XFEA_BISHENG_FATAL_LOG("ctr value[%d] is not in ascent order in ctr map file!", ctr_value);
            return -1;
        }
    }
    _ctr_value_vec.push_back(ctr_value);
    _ctr_value_and_bin_map.insert(std::pair<int32_t, int32_t>(ctr_value, ctr_bin));

    return 0;
}

int S_ctr_bin::load_ctr_mapping(const std::string& ctr_filename, const std::string& ctr_bin_name) {
    std::ifstream ifs(ctr_filename.c_str());
    if (!ifs) {
        XFEA_BISHENG_FATAL_LOG("can not find ctr map file: [%s]!", ctr_filename.c_str());
        return -1;
    }

    // 讨论：可以提前分配些内存
    _ctr_value_vec.clear();
    _ctr_value_and_bin_map.clear();

    std::string line;
    std::vector<std::string> field_vec;
    int line_no = 0;
    const std::string expected_prefix = ctr_bin_name + kSepeartor;
    while (std::getline(ifs, line)) {
        ++line_no;
        if (line.size() < expected_prefix.size() || line.compare(0, expected_prefix.size(), expected_prefix) != 0) {
            // 如果不以expected_prefix开头，则不不操作该行
            continue;
        }
        field_vec.clear(); 
        if (parse_ctr_mapping_line(line, ctr_bin_name, field_vec) != 0) {
            XFEA_BISHENG_FATAL_LOG("parse the [%d]th line for ctr map file[%s] failed!", line_no, ctr_filename.c_str());
            return -1;
        }
    }

    if (_ctr_value_and_bin_map.size() <= 0) {
        XFEA_BISHENG_FATAL_LOG("ctr map count is 0 for ctr map file[%s]!", ctr_filename.c_str());
        return -1;
    }

    XFEA_BISHENG_NOTICE_LOG("S_ctr_bin load total [%lu] ctr mapping of [%s] from [%s] successfully.",
            _ctr_value_and_bin_map.size(), ctr_bin_name.c_str(), ctr_filename.c_str());
    return 0;
}

int32_t S_ctr_bin::find_approximate_value(const std::vector<int32_t>& target_vec, const int32_t value) {
   int total_len = static_cast<int>(target_vec.size());
    if (total_len <= 0) {
        return -1;
    }
 
    int start = 0;
    int end = total_len - 1;
    int mid = (start + end) / 2;  // 讨论：数据通常较小，暂时不考虑越界
    while (start < end) {
        if (target_vec[mid] < value) {
            start = mid + 1;
        } else {
            end = mid - 1;
        }
        mid = (start + end) / 2;
    }

    int64_t appromiate_value = target_vec[mid];
    if (mid > 0) {
        if (std::abs((float)(target_vec[mid - 1] - value)) < std::abs((float)(appromiate_value - value)))
            appromiate_value = target_vec[mid - 1];
    }
    if (mid < total_len - 1) {
        if (std::abs((float)(target_vec[mid + 1] - value)) < std::abs((float)(appromiate_value - value)))
            appromiate_value = target_vec[mid + 1];
    }
    return appromiate_value;
}

int32_t S_ctr_bin::find_ctr_map(const char* fea_text) {
    int32_t value = 0;
    if (StringTool::StringToInt32(fea_text, value) != 0) {
        return kDefaultCtrBin;
    }

    std::map<int32_t, int32_t>::const_iterator iter = _ctr_value_and_bin_map.find(value);
    if (iter != _ctr_value_and_bin_map.end()) {
        return iter->second;
    } else {  // 如果没找到，则延用之前粉丝通在线的处理方式，寻找和value最接近的值
        int32_t approximate_value = find_approximate_value(_ctr_value_vec, value);
        std::map<int32_t, int32_t>::const_iterator approximate_iter = _ctr_value_and_bin_map.find(approximate_value);
        if (approximate_iter != _ctr_value_and_bin_map.end()) {
            return approximate_iter->second;
        } else {
            return kDefaultCtrBin;
        }
    }
}

ReturnCode S_ctr_bin::init(const ExtractorConfig& extractor_config) {
    // 依赖的字段个数必须为1
    if (_depend_col_index_vec.size() != 1) {
        XFEA_BISHENG_FATAL_LOG("depend field number must be equal to 1 in S_ctr_bin!");
        return RC_ERROR;
    }
    // ctr bin name
    if (_arg_str.size() < 1) {
        XFEA_BISHENG_FATAL_LOG("arg[%s] size must be greater than 1 in S_ctr_bin!", _arg_str.c_str());
        return RC_ERROR;
    }

    // 解析arg参数, 格式为：依赖的配置项名称 逗号 ctr_bin_name
    std::vector<std::string> field_vec;
    if (StringTool::tokenize(_arg_str, ",", field_vec) != 0) { 
        XFEA_BISHENG_FATAL_LOG("StringTool::tokenize the arg_str[%s] of S_ctr_bin failed!", _arg_str.c_str());
        return RC_ERROR;
    }
    if (field_vec.size() != 2) {  // 检查切分结果是否为2列
        XFEA_BISHENG_FATAL_LOG("StringTool::tokenize result vector size is not 2 for arg_str[%s] of S_ctr_bin failed!", _arg_str.c_str());
        return RC_ERROR;
    }
    _config_item_name = field_vec[0];
    _ctr_bin_name = field_vec[1];

    if (extractor_config.get_config_str(_config_item_name, _ctr_filename) != 0) { 
        XFEA_BISHENG_FATAL_LOG("get config item[%s] failed!", _config_item_name.c_str());
        return RC_ERROR;
    }

    // 加载ctr file
    XFEA_BISHENG_NOTICE_LOG("S_ctr_bin begins to load ctr map from file[%s] for ctr type[%s].", 
            _ctr_filename.c_str(), _ctr_bin_name.c_str());
    if (load_ctr_mapping(_ctr_filename, _ctr_bin_name) != 0) {
        XFEA_BISHENG_FATAL_LOG("S_ctr_bin load ctr map from file[%s] for ctr type[%s] failed!", 
                _ctr_filename.c_str(), _ctr_bin_name.c_str());
        return RC_ERROR;
    }

    return RC_SUCCESS;
}

ReturnCode S_ctr_bin::generate_fea(const LogRecordInterface& record, FeaResultSet& fea_result_set, bool copy_value) {
    // 获取抽取特征依赖的字段的index
    // 使用at有抛出异常的风险（init函数要做好_depend_col_index_vec size检查）
    int depend_col_field_index = _depend_col_index_vec.at(0);

    if (!record.is_update(depend_col_field_index))
        return RC_SUCCESS;

    // 获取其对应的字段值
    const char* fea_text = record.get_value(depend_col_field_index);
    if (NULL == fea_text) {
        XFEA_BISHENG_WARN_LOG("get index[%d] value of record failed in S_ctr_bin!", depend_col_field_index);
        return RC_WARNING;
    }

    // 如果找不到按照默认值-1来处理
    int32_t ctr_bin = find_ctr_map(fea_text);
    std::string v = boost::lexical_cast<std::string>(ctr_bin);

    // 计算特征签名及将抽取的特征放入fea_result_set，emit_feature位于SingleSlotFeatureOp
    return SingleSlotFeatureOp::emit_feature(_name, 0, v.c_str(), fea_result_set, copy_value);
}

XFEA_BISHENG_NAMESPACE_GUARD_END
