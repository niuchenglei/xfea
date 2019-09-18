#include "feature_op/S_bias.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

static char bias[] = "bias";

// 注册该特征类
// TODO 在此处注册不生效，在util/bisheng_factory.cpp中注册可以
// XFEA_BISHNEG_USING_FEATURE(S_bias);

// 初始化及参数检查, 需要依赖的字段个数必须为1
ReturnCode S_bias::init(const ExtractorConfig& extractor_config) {
    return RC_SUCCESS;
}

// 根据单个字段的内容直接生产特征
ReturnCode S_bias::generate_fea(const LogRecordInterface& record, FeaResultSet& fea_result_set, bool copy_value) {
    // 计算特征签名及将抽取的特征放入fea_result_set，emit_feature位于SingleSlotFeatureOp
    return SingleSlotFeatureOp::emit_feature(bias, fea_result_set, copy_value);
}

XFEA_BISHENG_NAMESPACE_GUARD_END

