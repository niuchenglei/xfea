#include "core/log_record_array_impl.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 初始化：从配置初始化成员变量
ReturnCode LogRecordArrayImpl::init(ExtractorConfig* extractor_config) {
    XFEA_BISHENG_NOTICE_LOG("LogRecordArrayImpl begins to init.");
    if (NULL == extractor_config) {
        XFEA_BISHENG_FATAL_LOG("input param value is NULL in LogRecordArrayImpl::init!");
        return RC_ERROR;
    }

    _extractor_config = extractor_config;
    reset();

    _tag_name = _extractor_config->get_tag_name();
    _expected_field_num = _extractor_config->get_input_field_num();
    if (_expected_field_num > GlobalParameter::kMaxRecordFieldNum) {
        XFEA_BISHENG_FATAL_LOG("[%s] input schema size[%u] exceeds max size[%d]!", _tag_name.c_str(), _expected_field_num, GlobalParameter::kMaxRecordFieldNum);
        return RC_ERROR;
    }
    XFEA_BISHENG_NOTICE_LOG("[%s] input schema size of LogRecordArrayImpl is %u.", _tag_name.c_str(), _expected_field_num);

    XFEA_BISHENG_NOTICE_LOG("[%s] LogRecordArrayImpl inits successfully.", _tag_name.c_str());
    return RC_SUCCESS;
}

// 填充字段值
ReturnCode LogRecordArrayImpl::fill_value(const char* value, const int field_index, bool copy_value) {
    if (NULL == value) {
        XFEA_BISHENG_FATAL_LOG("[%s] input param value is NULL in LogRecordArrayImpl::fill_value!",  _tag_name.c_str());
        return RC_ERROR;
    }

    // 检查field_index是否越界
    if (field_index < 0 || field_index >= _expected_field_num) {
        XFEA_BISHENG_FATAL_LOG("[%s] field_index[%d] exceeds bound [0, %u)!", _tag_name.c_str(), field_index, _expected_field_num);
        return RC_ERROR;
    }

    if (copy_value) {
        int wn = snprintf(_value_buf[field_index], GlobalParameter::kMaxRecordFieldValueSize, "%s", value);
        /*if (wn < 0 || wn >= GlobalParameter::kMaxRecordFieldValueSize) {
            XFEA_BISHENG_WARN_LOG("[%s] write using snprintf failed, return code [%d]!", _tag_name.c_str(), wn);
            return RC_WARNING;
        }*/
    } else {
        _value_buf_ptr[field_index] = value;
    }
    if (!_is_set[field_index]) {
        _is_set[field_index] = true;
        ++_already_filled_field_num;
    }
    _is_update[field_index] = true;

    return RC_SUCCESS;
}

// 填充字段值
ReturnCode LogRecordArrayImpl::fill_value(const char* value, const std::string& field_name, bool copy_value) {
    int field_index = _extractor_config->key_to_index(field_name);
    if (field_index < 0) {
        XFEA_BISHENG_FATAL_LOG("[%s] can not find index of field_name[%s]!", _tag_name.c_str(), field_name.c_str());
        return RC_ERROR;
    }

    return fill_value(value, field_index);
}
/*
// 更新字段值, 一般是预处理类使用
ReturnCode LogRecordArrayImpl::update_value(const char* value, const int field_index) {
    if (NULL == value) {
        XFEA_BISHENG_FATAL_LOG("[%s] input param value is NULL!",  _tag_name.c_str());
        return RC_ERROR;
    }

    // 检查field_index是否越界
    if (field_index < 0 || static_cast<uint32_t>(field_index) >= _expected_field_num) {
        XFEA_BISHENG_FATAL_LOG("[%s] field_index[%d] exceeds bound [0, %u)!", _tag_name.c_str(), field_index, _expected_field_num);
        return RC_ERROR;
    }

    int wn = snprintf(_value_buf[field_index], GlobalParameter::kMaxRecordFieldValueSize, "%s", value);
    if (wn < 0 || static_cast<uint32_t>(wn) >= GlobalParameter::kMaxRecordFieldValueSize) {
        XFEA_BISHENG_WARN_LOG("[%s] write using snprintf failed, return code [%d]!", _tag_name.c_str(), wn);
        return RC_WARNING;
    }
    if (!_is_set[field_index]) {
        _is_set[field_index] = true;
        ++_already_filled_field_num;
    }

    return RC_SUCCESS;
}

// 更新字段值, 一般是预处理类使用
ReturnCode LogRecordArrayImpl::update_value(const char* value, const std::string& field_name) {
    int field_index = _extractor_config->key_to_index(field_name);
    if (field_index < 0) {
        XFEA_BISHENG_FATAL_LOG("[%s] can not find index of field_name[%s]!", _tag_name.c_str(), field_name.c_str());
        return RC_ERROR;
    }

    return update_value(value, field_index);
}*/

// 清空存储的内容，供后续再次使用该类
void LogRecordArrayImpl::reset() {
    for (uint32_t i = 0; i < GlobalParameter::kMaxRecordFieldNum; ++i) {
        _value_buf[i][0] = '\0';
        _value_buf_ptr[i] = NULL;
        _is_set[i] = false;
        _is_update[i] = false;
    }
    _already_filled_field_num = 0;
}

void LogRecordArrayImpl::set_update(bool flag) {
    for (uint32_t i = 0; i < GlobalParameter::kMaxRecordFieldNum; ++i) {
        _is_update[i] = flag;
    }
}

// 检查是否可供特征提取使用（是否所有的字段都已填充值)
bool LogRecordArrayImpl::is_valid() const {
    if (_already_filled_field_num < _expected_field_num) {
        XFEA_BISHENG_WARN_LOG("[%s] already_filled_field_num %u is less than expected_field_num %u",
                _tag_name.c_str(), _already_filled_field_num, _expected_field_num);
        for (uint32_t i = 0; i < _expected_field_num; ++i) {
            if (!_is_set[i]) {
                XFEA_BISHENG_WARN_LOG("[%s] field_index[%u] is not set!", _tag_name.c_str(), i);
            }
        }
        return false;
    } else {
        return true;
    }
}

// 获取字段值
const char* LogRecordArrayImpl::get_value(const int field_index) const {
    if (field_index < 0 || field_index >= _expected_field_num) {
        XFEA_BISHENG_WARN_LOG("[%s] field_index[%d] exceeds bound [0, %u)!", _tag_name.c_str(), field_index, _expected_field_num);
        return NULL;
    }
    if (!_is_set[field_index]) {
        XFEA_BISHENG_WARN_LOG("[%s] field_index[%d] is not set!", _tag_name.c_str(), field_index);
        return NULL;
    }

    return (_value_buf_ptr[field_index] == NULL) ? _value_buf[field_index] : _value_buf_ptr[field_index];
}

const char* LogRecordArrayImpl::get_value(const std::string& field_name) const {
    int field_index = _extractor_config->key_to_index(field_name);
    if (field_index < 0) {
        XFEA_BISHENG_FATAL_LOG("[%s] can not find index of field_name[%s]!", _tag_name.c_str(), field_name.c_str());
        return NULL;
    }

    return get_value(field_index);
}

// 回收资源
void LogRecordArrayImpl::finalize() {
    XFEA_BISHENG_NOTICE_LOG("[%s] LogRecordArrayImpl begins to final.", _tag_name.c_str());

    _extractor_config = NULL;

    XFEA_BISHENG_NOTICE_LOG("[%s] LogRecordArrayImpl finals successfully.", _tag_name.c_str());
}

XFEA_BISHENG_NAMESPACE_GUARD_END

