#include "core/feature_op_manager.h"
#include "util/bisheng_factory.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

// 初始化：创建特征配置列表配置的特征类对象，并初始化这些对象
ReturnCode FeatureOpManager::init(ExtractorConfig* extractor_config) {
    if (NULL == extractor_config) {
        XFEA_BISHENG_FATAL_LOG("input param extractor_config is NULL, so FeatureOpManager init failed!");
        return RC_ERROR;
    }
    _extractor_config = extractor_config;

    // 清空_fea_op_vec
    _fea_op_vec.clear();

    // 获取特征列表配置
    const std::vector<fea_op_config_t>& feature_op_config_vec = _extractor_config->get_feature_op_config_vec();
    // 创建特征类并初始化
    std::vector<fea_op_config_t>::const_iterator iter = feature_op_config_vec.begin();
    for (; iter != feature_op_config_vec.end(); ++iter) {
        // 创建新的特征类对象
        FeatureOpBase* fea_op = ComponentManager::create_feature_op_object(iter->fea_op_name);
        if (NULL == fea_op) {
            XFEA_BISHENG_FATAL_LOG("create fea op object [%s] failed!", iter->fea_op_name.c_str());
            return RC_ERROR;
        }
        // 设置特征类对象的基本信息
        fea_op->set_info(*iter);

        // 初始化特征类对象
        if (fea_op->init(*_extractor_config) != RC_SUCCESS) {
            XFEA_BISHENG_FATAL_LOG("feature op [%s] of name [%s] init failed!",
                    iter->fea_op_name.c_str(), iter->name.c_str());
            return RC_ERROR;
        }
        // 将初始化成功的特征类对象添加到_fea_op_vec
        _fea_op_vec.push_back(fea_op);
    }
    if (_fea_op_vec.size() < 1) {  // 配置的要提取特征的个数不能小于1
        XFEA_BISHENG_FATAL_LOG("feature op count must be not less than 1!");
        return RC_ERROR;
    }

    return RC_SUCCESS;
}

// 调用所有配置的特征类提取特征，并将结果存入FeaResultSet
ReturnCode FeatureOpManager::extract_features(const LogRecordInterface& record, FeaResultSet& fea_result_set) {
    std::vector<FeatureOpBase*>::iterator iter = _fea_op_vec.begin();
    // 调用每一个配置的特征类进行特征抽取; 如果某个op出现返回结果不为RC_SUCCESS，则停止执行本函数
    for (; iter != _fea_op_vec.end(); ++iter) {
        ReturnCode ret = (*iter)->generate_fea(record, fea_result_set);
        if (RC_ERROR == ret) {
            XFEA_BISHENG_FATAL_LOG("fea op [%s] generate_fea failed occurs fatal error!", (*iter)->get_name_char_ptr());
            return RC_ERROR;
        } else if (RC_WARNING == ret) {
            XFEA_BISHENG_WARN_LOG("fea op [%s] generate_fea occurs warning!", (*iter)->get_name_char_ptr());
            return RC_WARNING;
        }
    }

    return RC_SUCCESS;
}

// 回收资源：delete创建的特征类对象
void FeatureOpManager::FeatureOpManager::finalize() {
    std::vector<FeatureOpBase*>::iterator iter = _fea_op_vec.begin();
    for (; iter != _fea_op_vec.end(); ++iter) {
        delete *iter;
    }

    _extractor_config = NULL;
}

XFEA_BISHENG_NAMESPACE_GUARD_END
