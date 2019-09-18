#include "feature_op/S_multi.h"
#include <cstdlib>
#include "util/bisheng_string_tool.h"

using namespace std;

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 初始化及参数检查, 需要依赖的字段个数必须为1
ReturnCode S_multi::init(const ExtractorConfig& extractor_config) {
    // 依赖的字段个数必须为1
    if (_depend_col_index_vec.size() != 1) {
        XFEA_BISHENG_FATAL_LOG("depend field number must be equal to 1 in feature [%s]!", _name.c_str());
        return RC_ERROR;
    }

    delim = '#';
    if (_arg_str.size() >= 1) {
        string arg_str = StringTool::trim(_arg_str);
        if (arg_str.size() >= 1)
            delim = arg_str[0];
    }

    XFEA_BISHENG_NOTICE_LOG("_output_ratio of feature [%s] is delim:[%c]!", _name.c_str(), delim);
    return RC_SUCCESS;
}

// 根据单个字段的内容直接生产特征
ReturnCode S_multi::generate_fea(const LogRecordInterface& record, FeaResultSet& fea_result_set, bool copy_value) {
    int depend_col_field_index = _depend_col_index_vec.at(0);
    
    const char* fea_text = record.get_value(depend_col_field_index);
    if (NULL == fea_text) {
        XFEA_BISHENG_WARN_LOG("get index[%d] value of record failed in feature [%s]!", depend_col_field_index, _name.c_str());
        return RC_WARNING;
    }

    string value = string(fea_text);
    vector<string> array;
    StringTool::Split2(array, value, delim);
    ReturnCode ret = RC_SUCCESS;
    for (int k=0; k<array.size(); k++) {
        ret = SingleSlotFeatureOp::emit_feature(array[k].c_str(), fea_result_set, copy_value);

        if (ret != RC_SUCCESS)
            break;
    }

    return ret;
}

XFEA_BISHENG_NAMESPACE_GUARD_END

