#include "core/fea_result_set.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 填充一个特征明文为char*的特征提取结果
ReturnCode FeaResultSet::fill_fea_result(const char* value, const int32_t fea_slot,
        const bool is_hash, const uint64_t origin_fea_sign, const uint64_t final_fea_sign) {
    if (NULL == value) {
        XFEA_BISHENG_WARN_LOG("input param value is NULL!");
        return RC_WARNING;
    }
    if (_fea_result_num >= (sizeof(_fea_result_array) / sizeof(_fea_result_array[0]))) {
        XFEA_BISHENG_WARN_LOG("fea result array size [%u] is not less than max size[%lu], so can not write fea result array!",
                _fea_result_num, sizeof(_fea_result_array) / sizeof(_fea_result_array[0]));
        return RC_WARNING;
    }

    int wn = snprintf(_fea_result_array[_fea_result_num].fea_text, GlobalParameter::kMaxFeaTextResultSize, "%s", value);
    if (wn < 0) {
        XFEA_BISHENG_WARN_LOG("snprintf fea_result_array failed, return code [%d]!", wn);
        // fea result text置为'\0', 继续执行
        _fea_result_array[_fea_result_num].fea_text[0] = '\0';
    } else if (wn >= static_cast<int>(GlobalParameter::kMaxFeaTextResultSize)) {
        XFEA_BISHENG_WARN_LOG("snprintf fea_result_array failed, return code [%d], max fea_result_array size[%d]!",
                wn, GlobalParameter::kMaxFeaTextResultSize);
        // 继续执行
    }

    _fea_result_array[_fea_result_num].fea_slot = fea_slot;
    _fea_result_array[_fea_result_num].is_hash = is_hash;
    _fea_result_array[_fea_result_num].origin_fea_sign = origin_fea_sign;
    _fea_result_array[_fea_result_num].final_fea_sign = final_fea_sign;
    ++_fea_result_num;

    return RC_SUCCESS;
}

// 填充一个特征明文为char*类型特征提取结果, 不进行特征签名
ReturnCode FeaResultSet::fill_fea_result(const char* value, const int32_t fea_slot) {
    return fill_fea_result(value, fea_slot, false, 0, 0);
}

// 填充一个特征明文为int32_t的特征提取结果
ReturnCode FeaResultSet::fill_int32_fea_result(const int32_t value, const int32_t fea_slot,
        const bool is_hash, const uint64_t origin_fea_sign, const uint64_t final_fea_sign) {
    if (_fea_result_num >= (sizeof(_fea_result_array) / sizeof(_fea_result_array[0]))) {
        XFEA_BISHENG_WARN_LOG("fea result array size [%u] is not less than max size[%lu], so can not write fea result array!",
                _fea_result_num, sizeof(_fea_result_array) / sizeof(_fea_result_array[0]));
        return RC_WARNING;
    }

    int wn = snprintf(_fea_result_array[_fea_result_num].fea_text, GlobalParameter::kMaxFeaTextResultSize, "%d", value);
    if (wn < 0) {
        XFEA_BISHENG_WARN_LOG("snprintf fea_result_array failed, return code [%d]!", wn);
        // fea result text置为'\0', 继续执行
        _fea_result_array[_fea_result_num].fea_text[0] = '\0';
    } else if (wn >= static_cast<int>(GlobalParameter::kMaxFeaTextResultSize)) {
        XFEA_BISHENG_WARN_LOG("snprintf fea_result_array failed, return code [%d], max fea_result_array size[%d]!",
                wn, GlobalParameter::kMaxFeaTextResultSize);
        // 继续执行
    }

    _fea_result_array[_fea_result_num].fea_slot = fea_slot;
    _fea_result_array[_fea_result_num].is_hash = is_hash;
    _fea_result_array[_fea_result_num].origin_fea_sign = origin_fea_sign;
    _fea_result_array[_fea_result_num].final_fea_sign = final_fea_sign;
    ++_fea_result_num;

    return RC_SUCCESS;
}

// 填充一个特征明文为int32_t类型特征提取结果, 不进行特征签名
ReturnCode FeaResultSet::fill_int32_fea_result(const int32_t value, const int32_t fea_slot) {
    return fill_int32_fea_result(value, fea_slot, false, 0, 0);
}

XFEA_BISHENG_NAMESPACE_GUARD_END

