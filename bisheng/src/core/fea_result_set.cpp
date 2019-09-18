#include "core/fea_result_set.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 填充一个特征明文为char*的特征提取结果
ReturnCode FeaResultSet::fill_fea_result(const char* value, const int32_t fea_slot,
        const bool is_hash, const uint64_t origin_fea_sign, const uint64_t final_fea_sign, bool copy_value) {
    if (NULL == value) {
        XFEA_BISHENG_WARN_LOG("input param value is NULL!");
        return RC_WARNING;
    }
    if (_fea_result_num >= GlobalParameter::kMaxFeaResultNum) {
        XFEA_BISHENG_WARN_LOG("fea result array size [%u] is not less than max size[%u], so can not write fea result array!",
                _fea_result_num, GlobalParameter::kMaxFeaResultNum);
        return RC_WARNING;
    }

    if (copy_value) {
        int wn = snprintf(_fea_result_array[_fea_result_num].fea_text, GlobalParameter::kMaxFeaTextResultSize, "%s", value);
        if (wn < 0) {
            XFEA_BISHENG_WARN_LOG("snprintf fea_result_array failed, return code [%d]!", wn);
            // fea result text置为'\0', 继续执行
            _fea_result_array[_fea_result_num].fea_text[0] = '\0';
        }
    }

    _fea_result_array[_fea_result_num].fea_slot = fea_slot;
    _fea_result_array[_fea_result_num].is_hash = is_hash;
    _fea_result_array[_fea_result_num].origin_fea_sign = origin_fea_sign;
    _fea_result_array[_fea_result_num].final_fea_sign = final_fea_sign;
    ++_fea_result_num;

    return RC_SUCCESS;
}

XFEA_BISHENG_NAMESPACE_GUARD_END

