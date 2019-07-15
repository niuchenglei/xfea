#include "feature_op/single_slot_feature_op.h"
#include "util/bisheng_hash.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 将提取的特征明文进行签名变换等操作，并将相关结果存入fea_result_set
ReturnCode SingleSlotFeatureOp::emit_feature(const char* fea_text, FeaResultSet& fea_result_set) {
    if (NULL == fea_text) {
        XFEA_BISHENG_WARN_LOG("feature text to emit is NULL!");
        return RC_WARNING;
    }
    if (!_is_need_hash) {  // 不需要进行特征签名
        return fea_result_set.fill_fea_result(fea_text, _slot);
    } else {
        uint64_t origin_fea_sign = HashTool::hash64(fea_text, _slot);
        uint64_t transformed_fea_sign = origin_fea_sign;
        if (_hash_range_min < _hash_range_max) {  // 需要进行特征范围变换
            transformed_fea_sign = _hash_range_min + (origin_fea_sign % (_hash_range_max - _hash_range_min));
        }
        return fea_result_set.fill_fea_result(fea_text, _slot, _is_need_hash, origin_fea_sign, transformed_fea_sign);
    }
}

// 将提取的int32类型的特征明文进行签名变换等操作，并将相关结果存入fea_result_set
ReturnCode SingleSlotFeatureOp::emit_int32_feature(const int32_t fea_text, FeaResultSet& fea_result_set) {
    if (!_is_need_hash) {  // 不需要进行特征签名
        return fea_result_set.fill_int32_fea_result(fea_text, _slot);
    } else {
        uint64_t origin_fea_sign = HashTool::hash64_with_int32_value(fea_text, _slot);
        uint64_t transformed_fea_sign = origin_fea_sign;
        if (_hash_range_min < _hash_range_max) {  // 需要进行特征范围变换
            transformed_fea_sign = _hash_range_min + (origin_fea_sign % (_hash_range_max - _hash_range_min));
        }
        return fea_result_set.fill_int32_fea_result(fea_text, _slot, _is_need_hash, origin_fea_sign, transformed_fea_sign);
    }
}

XFEA_BISHENG_NAMESPACE_GUARD_END

