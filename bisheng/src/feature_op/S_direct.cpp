#include "feature_op/S_direct.h"

#include <cstdlib>

#include "util/bisheng_string_tool.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 注册该特征类
// TODO 在此处注册不生效，在util/bisheng_factory.cpp中注册可以
// XFEA_BISHNEG_USING_FEATURE(S_direct);

// 初始化及参数检查, 需要依赖的字段个数必须为1
ReturnCode S_direct::init(const ExtractorConfig& extractor_config) {
    // 避免出现unused-parameter warning
    XFEA_BISHENG_UNUSED_VAR(extractor_config);

    // 依赖的字段个数必须为1
    if (_depend_col_index_vec.size() != 1) {
        XFEA_BISHENG_FATAL_LOG("depend field number must be equal to 1 in feature [%s]!", _name.c_str());
        return RC_ERROR;
    }

    if (_arg_str.size() >= 1) {
        // 讨论：为了支持单特征采样，实现了一种非常临时的方案，在S_direct中操作
        //       更好的方式是在基类特征输出时操作， 主要是考虑到这种特征采样，
        //       在模型训练阶段做最合理；
        if (StringTool::StringToDouble(_arg_str, _output_ratio) != 0) {
            XFEA_BISHENG_FATAL_LOG("StringTool::StringToDouble tranform [%s] to double failed in feature [%s]!",
                    _arg_str.c_str(), _name.c_str());
            return RC_ERROR;
        }
        //if (_output_ratio < 0.0 || _output_ratio > 1.0) {
        if (_output_ratio < 0.0) {
            XFEA_BISHENG_FATAL_LOG("_output_ratio[%f] of feature [%s] is not in range[0.0, 1.0]!",
                    _output_ratio, _name.c_str());
            return RC_ERROR;
        }
    }

    XFEA_BISHENG_NOTICE_LOG("_output_ratio of feature [%s] is [%f]!", _name.c_str(), _output_ratio);
    return RC_SUCCESS;
}

// 根据单个字段的内容直接生产特征
ReturnCode S_direct::generate_fea(const LogRecordInterface& record, FeaResultSet& fea_result_set) {
    float rnd = ((double)rand()) / RAND_MAX;
    if (_output_ratio > 1.0) {
        if (rnd > (_output_ratio-1.0))
            return SingleSlotFeatureOp::emit_feature("0", fea_result_set);

        // 获取抽取特征依赖的字段的index
        // 使用at有抛出异常的风险（init函数要做好_depend_col_index_vec size检查）
        int depend_col_field_index = _depend_col_index_vec.at(0);
        // 获取其对应的字段值
        const char* fea_text = record.get_value(depend_col_field_index);
        if (NULL == fea_text) {
            XFEA_BISHENG_WARN_LOG("get index[%d] value of record failed in feature [%s]!", depend_col_field_index, _name.c_str());
            return RC_WARNING;
        }
        // 计算特征签名及将抽取的特征放入fea_result_set，emit_feature位于SingleSlotFeatureOp
        return SingleSlotFeatureOp::emit_feature(fea_text, fea_result_set);
    }

    if (_arg_str.size() >= 1 && (rnd > _output_ratio)) {  // 采样
        return RC_SUCCESS;
    }
    // 获取抽取特征依赖的字段的index
    // 使用at有抛出异常的风险（init函数要做好_depend_col_index_vec size检查）
    int depend_col_field_index = _depend_col_index_vec.at(0);
    // 获取其对应的字段值
    const char* fea_text = record.get_value(depend_col_field_index);
    if (NULL == fea_text) {
        XFEA_BISHENG_WARN_LOG("get index[%d] value of record failed in feature [%s]!", depend_col_field_index, _name.c_str());
        return RC_WARNING;
    }
    // 计算特征签名及将抽取的特征放入fea_result_set，emit_feature位于SingleSlotFeatureOp
    return SingleSlotFeatureOp::emit_feature(fea_text, fea_result_set);
}

XFEA_BISHENG_NAMESPACE_GUARD_END

