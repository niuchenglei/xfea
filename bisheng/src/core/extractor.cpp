#include "core/extractor.h"
#include "core/log_record_array_impl.h"
#include "common/bisheng_macro.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

void Extractor::print_version_info() {
    XFEA_BISHENG_NOTICE_LOG("bisheng lib verion [%s], compile hostname[%s], compile path[%s].",
        BISHENG_VERSION_STR, BISHENG_COMPILE_HOSTNAME_STR, BISHENG_COMPILE_PWD_STR);
}

// 加载配置并初始化各个成员变量（只调用一次）
ReturnCode Extractor::init(const std::string& config_file_path, const std::string& feature_list_conf_file_path) {
   print_version_info();
    // 加载配置
    _extractor_config = new(std::nothrow) ExtractorConfig();
    if (NULL == _extractor_config) {
        XFEA_BISHENG_FATAL_LOG("allocate memory of ExtractorConfig failed!");
        return RC_ERROR;
    }
    ReturnCode ret = _extractor_config->init(config_file_path, feature_list_conf_file_path);
    if (RC_SUCCESS != ret) {
        XFEA_BISHENG_FATAL_LOG("extractor_config inits failed by config file path[%s]!", config_file_path.c_str());
        XFEA_BISHENG_DELETE_IF_NOT_NULL(_extractor_config);
        return RC_ERROR;
    } else {
        XFEA_BISHENG_NOTICE_LOG("load [%s] config file path successfully, config content: \n%s",
                config_file_path.c_str(), _extractor_config->to_string().c_str());
    }

    // 讨论：各个成员变化初始化方式可以封装为宏
    // 初始化record
    _record = new(std::nothrow) LogRecordArrayImpl();
    if (NULL == _record) {
        XFEA_BISHENG_FATAL_LOG("allocate memory of LogRecordArrayImpl failed!");
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_extractor_config);
        return RC_ERROR;
    }
    ret = _record->init(_extractor_config);
    if (RC_SUCCESS != ret) {
        XFEA_BISHENG_FATAL_LOG("record inits failed!");
        XFEA_BISHENG_DELETE_IF_NOT_NULL(_record);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_extractor_config);
        return RC_ERROR;
    }

    // 初始化result
    _fea_result_set = new(std::nothrow) FeaResultSet();
    if (NULL == _fea_result_set) {
        XFEA_BISHENG_FATAL_LOG("allocate memory of FeaResultSet failed!");
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_record);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_extractor_config);
        return RC_ERROR;
    }
    ret = _fea_result_set->init();
    if (RC_SUCCESS != ret) {
        XFEA_BISHENG_FATAL_LOG("fea_result_set inits failed!");
        XFEA_BISHENG_DELETE_IF_NOT_NULL(_fea_result_set);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_record);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_extractor_config);
        return RC_ERROR;
    }

    // 初始化预处理管理类

    // 初始化特征管理类
    _feature_op_manager = new(std::nothrow) FeatureOpManager();
    if (NULL == _feature_op_manager) {
        XFEA_BISHENG_FATAL_LOG("allocate memory of FeatureOpManager failed!");
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_fea_result_set);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_record);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_extractor_config);
        return RC_ERROR;
    }
    ret = _feature_op_manager->init(_extractor_config);
    if (RC_SUCCESS != ret) {
        XFEA_BISHENG_FATAL_LOG("feature_op_manager inits failed!");
        XFEA_BISHENG_DELETE_IF_NOT_NULL(_feature_op_manager);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_fea_result_set);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_record);
        XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_extractor_config);
        return RC_ERROR;
    }


    return RC_SUCCESS;
}

// 进行特征抽取
ReturnCode Extractor::extract_features_from_record() {
    // check record是否合法
    if (!_record->is_valid()) {
        XFEA_BISHENG_WARN_LOG("record is invalid: some input field values are not set!");
        return RC_WARNING;
    }

    // 预处理
    // 提取特征
    ReturnCode ret = _feature_op_manager->extract_features(*_record, *_fea_result_set);
    if (RC_ERROR == ret) {
        XFEA_BISHENG_FATAL_LOG("feature_op_manager's function extract_features occurs fatal error!");
        return RC_ERROR;
    } else if (RC_WARNING == ret) {
        XFEA_BISHENG_WARN_LOG("feature_op_manager's function extract_features occurs warning!");
        return RC_WARNING;
    }

    return RC_SUCCESS;
}

// 回收资源（只调用一次）
void Extractor::finalize() {
    XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_feature_op_manager);
    XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_fea_result_set);
    XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_record);
    XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(_extractor_config);
}

XFEA_BISHENG_NAMESPACE_GUARD_END
