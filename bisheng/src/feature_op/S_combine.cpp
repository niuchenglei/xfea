#include "feature_op/S_combine.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

ReturnCode S_combine::init(const ExtractorConfig& extractor_config) {
    // 依赖的字段个数必须大于1
    if (_depend_col_index_vec.size() < 1) {
        XFEA_BISHENG_FATAL_LOG("depend field number must be not less than 1 in feature [%s]!", _name.c_str());
        return RC_ERROR;
    }
    // 传入的arg参数为特征明文之间的分隔符，目前要求分隔符只能有1个字符, 不配置使用默认分隔符
    if (_arg_str.size() > 1) { 
        XFEA_BISHENG_FATAL_LOG("arg[%s] size must be equal to 1 in feature [%s]!", _arg_str.c_str(), _name.c_str());
        return RC_ERROR;
    } else if (_arg_str.size() == 1) {
        _seperator = _arg_str[0];
    }

    _buf[0] = '\0';
    return RC_SUCCESS;
}

ReturnCode S_combine::generate_fea(const LogRecordInterface& record, FeaResultSet& fea_result_set, bool copy_value) {
    const char* field = NULL;
    int wn = 0;
    std::vector<int>::const_iterator iter = _depend_col_index_vec.begin();
    for (; iter != _depend_col_index_vec.end(); ++iter) {
        field = record.get_value(*iter);
        if (NULL == field) {
            XFEA_BISHENG_WARN_LOG("get index[%d] value of record failed in feature [%s]!", *iter, _name.c_str());
            return RC_WARNING;
        }
        wn += snprintf(_buf + wn, kBufSize - wn, "%s%c", field, _seperator);
        if (wn < 0 || wn >= kBufSize) {
            XFEA_BISHENG_WARN_LOG("snprintf exceeds max buf size[%d] in feature [%s], return code [%d]!",
                    kBufSize, _name.c_str(), wn);
            return RC_WARNING;
        }
    }
    // 去除for循环中多写的一个分隔符
    _buf[--wn] = '\0'; 

    // 计算特征签名及将抽取的特征放入fea_result_set，emit_feature位于SingleSlotFeatureOp
    return SingleSlotFeatureOp::emit_feature(_buf, fea_result_set, copy_value);
}

XFEA_BISHENG_NAMESPACE_GUARD_END
