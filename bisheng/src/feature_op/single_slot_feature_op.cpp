#include "feature_op/single_slot_feature_op.h"
#include "util/bisheng_hash.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 将提取的特征明文进行签名变换等操作，并将相关结果存入fea_result_set
inline ReturnCode SingleSlotFeatureOp::emit_feature(const char* fea_text, FeaResultSet& fea_result_set, bool copy_value) {
    if (NULL == fea_text) {
        XFEA_BISHENG_WARN_LOG("feature text to emit is NULL!");
        return RC_WARNING;
    }
    if (!_is_need_hash) {  // 不需要进行特征签名
        return fea_result_set.fill_fea_result(fea_text, _slot, false, 0, 0, copy_value);
    } else {
        uint64_t origin_fea_sign = HashTool::crc64(fea_text, _slot);
        uint64_t transformed_fea_sign = origin_fea_sign;
        if (_hash_range > 0)
            transformed_fea_sign = _hash_range_min + (origin_fea_sign % _hash_range);

        return fea_result_set.fill_fea_result(fea_text, _slot, _is_need_hash, origin_fea_sign, transformed_fea_sign);
    }
}

XFEA_BISHENG_NAMESPACE_GUARD_END

