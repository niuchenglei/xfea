#include <string>
#include <iostream>
#include <sstream>

#include "core/extractor.h"
#include "common/bisheng_common.h"

using xfea::bisheng::FeaResultSet;
using xfea::bisheng::fea_result_t;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::RC_WARNING;

namespace {

void output_fea_result_set(const FeaResultSet& fea_result_set) {
    uint32_t fea_result_num = fea_result_set.size();
    // std::cout << "Instance result: " << std::endl;
    for (uint32_t i = 0; i < fea_result_num; ++i) {
         const fea_result_t* fea_result = fea_result_set.get_fea_result(i);
         if (NULL == fea_result) {
            XFEA_BISHENG_FATAL_LOG("get index[%d] fea_result failed!", i);
            return;
         } else {
            std::cout << "  " << fea_result->fea_slot << ":" << fea_result->fea_text << ":"
                    << fea_result->is_hash << "(" << fea_result->origin_fea_sign << " "
                    << fea_result->final_fea_sign << ")";
         }
    }
    std::cout << std::endl;
}

int run_fe_extract(const std::string& config_path, const std::string& input_file) {
    xfea::bisheng::Extractor extractor;
    // 初始化extractor
    if (extractor.init(config_path) != RC_SUCCESS) {
        XFEA_BISHENG_FATAL_LOG("Extractor init failed by config file[%s]!", config_path.c_str());
        return -1;
    }

    for (int i = 0; i < 20000; ++i) {
        std::ostringstream oss1;
        // extractor清空内部Record和Result存储
        extractor.one_round_reset();
        
        // 向extractor填充record字段
        oss1 << "mid_" << i;
        ReturnCode ret = extractor.add_field_value(oss1.str(), 0);
        if (RC_ERROR == ret) {
            XFEA_BISHENG_FATAL_LOG("add record to index 0 occurs fatal error in lineno[%d]!", i);
            return -1;
        } else if (RC_WARNING == ret) {
            XFEA_BISHENG_WARN_LOG("add record to index 0 occurs warn in lineno[%d]!", i);
            // 处理下一条
            continue;
        }

        std::ostringstream oss2;
        oss2 << "custom_id_" << i;
        ret = extractor.add_field_value(oss2.str(), 1);
        if (RC_ERROR == ret) {
            XFEA_BISHENG_FATAL_LOG("add record to index 1 occurs fatal error in lineno[%d]!", i);
            return -1;
        } else if (RC_WARNING == ret) {
            XFEA_BISHENG_WARN_LOG("add record to index 1 occurs warn in lineno[%d]!", i);
            // 处理下一条
            continue;
        }

        std::ostringstream oss3;
        oss3 << i;
        ret = extractor.add_field_value(oss3.str(), 2);
        if (RC_ERROR == ret) {
            XFEA_BISHENG_FATAL_LOG("add record to index 2 occurs fatal error in lineno[%d]!", i);
            return -1;
        } else if (RC_WARNING == ret) {
            XFEA_BISHENG_WARN_LOG("add record to index 2 occurs warn in lineno[%d]!", i);
            // 处理下一条
            continue;
        }

        // extractor抽取特征
        ret = extractor.extract_features_from_record();
        if (RC_ERROR == ret) {
            XFEA_BISHENG_FATAL_LOG("extract_features_from_record() occurs fatal error in lineno[%d]!", i);
            return -1;
        } else if (RC_WARNING == ret) {
            XFEA_BISHENG_WARN_LOG("extract_features_from_record() occurs warn in lineno[%d]!", i);
            // 处理下一条
            continue;
        }

        // 从extractor获取FeaResultSet
        const FeaResultSet& fea_result_set = extractor.get_fea_result_set();
 
        // 可以自定义对FeaResultSet的处理
        output_fea_result_set(fea_result_set); 
    }

    // extractor资源回收
    extractor.finalize();

    return 0;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string config_path = "./bisheng_example.conf";
    return run_fe_extract(config_path, "");
}

