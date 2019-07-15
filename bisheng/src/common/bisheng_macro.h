#ifndef XFEA_BISHENG_COMMON_BISHENG_MACRO_H_
#define XFEA_BISHENG_COMMON_BISHENG_MACRO_H_

#define XFEA_BISHENG_DELETE_IF_NOT_NULL(var) \
    if (NULL != var) { \
        delete var; \
        var = NULL; \
    }

#define XFEA_BISHENG_FINALIZE_AND_DELETE_IF_NOT_NULL(var) \
    if (NULL != var) { \
        var->finalize(); \
        delete var; \
        var = NULL; \
    }

#endif
