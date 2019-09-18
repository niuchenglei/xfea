#include "util/bisheng_factory.h"
#include "feature_op/S_direct.h"
#include "feature_op/S_multi.h"
#include "feature_op/S_bias.h"
#include "feature_op/S_ctr_bin.h"
#include "feature_op/S_combine.h"

XFEA_BISHENG_NAMESPACE_GUARD_BEGIN

ComponentManager* ComponentManager::_s_instance;

// 注册Feature OpFactoryManager
XFEA_BISHNEG_USING_FACTORY_MANAGER(FEATURE);

// TODO 注册特征类，在S_direct.cpp中注册不生效
XFEA_BISHNEG_USING_FEATURE(S_direct);
XFEA_BISHNEG_USING_FEATURE(S_multi);
XFEA_BISHNEG_USING_FEATURE(S_bias);
XFEA_BISHNEG_USING_FEATURE(S_ctr_bin);
XFEA_BISHNEG_USING_FEATURE(S_combine);


XFEA_BISHENG_NAMESPACE_GUARD_END
