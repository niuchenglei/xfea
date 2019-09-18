#ifndef XFEA_BISHENG_COMMON_BISHENG_COMMON_H_
#define XFEA_BISHENG_COMMON_BISHENG_COMMON_H_

#include <stdint.h>
#include <cstdio>


#define XFEA_BISHENG_NAMESPACE_GUARD_BEGIN \
    namespace xfea {\
    namespace bisheng {

#define XFEA_BISHENG_NAMESPACE_GUARD_END \
    } \
    }


// 日志相关
#ifdef _BISHENG_USE_BASE_LOG_

#include <base/log/log.h> 

#define XFEA_BISHENG_NOTICE_LOG(format, ...) LOG_INFO("bidfeed", format, ##__VA_ARGS__)

#define XFEA_BISHENG_WARN_LOG(format, ...) LOG_WARN("bidfeed", format, ##__VA_ARGS__)

#define XFEA_BISHENG_FATAL_LOG(format, ...) LOG_FATAL("bidfeed", format, ##__VA_ARGS__)

#else

#define XFEA_BISHENG_NOTICE_LOG(format, ...) \
    do { \
        fprintf(stderr, "[%s][%s@%s][%d] " format "\n", \
            "NOTICE", __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define XFEA_BISHENG_WARN_LOG(format, ...) \
    do { \
        fprintf(stderr, "[%s][%s@%s][%d] " format "\n", \
            "WARN", __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define XFEA_BISHENG_FATAL_LOG(format, ...) \
    do { \
        fprintf(stderr, "[%s][%s@%s][%d] " format "\n", \
            "ERROR", __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#endif

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

enum ReturnCode {
    RC_SUCCESS = 0,  // 执行成功
    RC_WARNING = -1,  // 执行失败，可接受错误
    RC_ERROR   = -2   // 执行失败，不可接受错误
};

class GlobalParameter {
public:
    static const uint32_t kMaxRecordFieldNum = 1024;           // Record的最大存储字段数
    static const uint32_t kMaxRecordFieldValueSize = 2048;     // Record的单个字段最大存储空间(含'\0'
    static const uint32_t kMaxFeaResultNum = 8192;             // Result最大数量（最多产生X个特征）
    static const uint32_t kMaxFeaTextResultSize = 64;          // Result的单个字段最大存储（处理后特征值长度）
};

XFEA_BISHENG_NAMESPACE_GUARD_END

#endif
