#ifndef __XFEA_OFFLINE_FEA_H_ 
#define __XFEA_OFFLINE_FEA_H_ 

#include <string>
#include <unordered_map>
#include "core/extractor.h"
#include "common/bisheng_common.h"

using namespace std;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::fea_result_t;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::RC_WARNING;

namespace xfea { namespace offxtractor {

class Delimiter {
public:
    static const int TAB_DELIMITER = 1;
    static const int SPACE_DELIMITER = 2;
    static const int CTRLA_DELIMITER = 3;
};
class OfflineExtractor {
    public:
        OfflineExtractor();
        virtual ~OfflineExtractor();

        bool Finalize();
        int Init(const string&, const string&);
        int Map(const string &line, const int delimiter);
        void Reduce(const std::string& op, const std::string& pattern);
    private:
        void MapOutput(const FeaResultSet& fea_result_set, const string& clk);
        void ReduceOutput(const string &ins_fea, long int &show, long int &clk, const string &tag);
        void ReduceMerge(const std::string& filter_in);
        void ReduceHebe();
        xfea::bisheng::Extractor _xtractor;
        std::unordered_map<int, int> _schema_index_m;
};

}}
#endif
