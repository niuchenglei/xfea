#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <time.h>
#include <sys/time.h>

#include "core/extractor.h"
#include "common/bisheng_common.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using xfea::bisheng::FeaResultSet;
using xfea::bisheng::fea_result_t;
using xfea::bisheng::ReturnCode;
using xfea::bisheng::RC_SUCCESS;
using xfea::bisheng::RC_ERROR;
using xfea::bisheng::RC_WARNING;

inline static void Split3(std::vector<std::string>& ss,
                            const string& line,
                            char dmt='\t')
    {
        if(line.empty())
            return;
        string::size_type p=0;
        string::size_type q;
        ss.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            string str = line.substr(p,q-p);
            ss.push_back(str);
            if(q == string::npos) break;
            p = q+1;
        }
    }


int main(int argc, char* argv[]) {
  xfea::bisheng::Extractor xtractor;

  srand((unsigned)time(NULL));

  if (xtractor.init("/data/niucl/xfea/offline/conf/suning_features.conf") != RC_SUCCESS) {
    std::cerr << "_xtractor init failed by config file " << std::endl;
    return -1;
  }
  xfea::bisheng::Extractor::print_version_info();

  const std::map<std::string, int>& bisheng_schema = xtractor.get_input_schema_key_index_map();

  ReturnCode ret;
  string str = "uid,solution_id,exchange_id,media_id,client_type,client_id,adspace_id,creative_id,slot_size,time_stamp,age,app_prefer,gender,cart_no_commit_15d,cart_no_commit_7d,cart_no_commit_3d,price_sensitive,work_day_prefer,work_time_prefer,gds_cd,cate1_pv_click";
  string val = "uid,solution_id,exchange_id,media_id,client_type,client_id,adspace_id,creative_id,slot_size,time_stamp,age,app_prefer,gender,cart_no_commit_15d,cart_no_commit_7d,cart_no_commit_3d,price_sensitive,work_day_prefer,work_time_prefer,gds_cd,cate1_pv_click";

  vector<string> tmp, tmp2;
  Split3(tmp, str, ',');
  Split3(tmp2, val, ',');


  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int _s=0; _s<1000; _s++) {

  for (int k=0; k<19; k++) {
    ret = xtractor.add_field_value(tmp2[k].c_str(), tmp[k], false);
    if (ret != RC_SUCCESS)
      fprintf(stderr, "error %s", tmp[k].c_str());
  }

  int count = 0;
  for (int n=0; n<1000; n++) {


  xtractor.just_result_reset();
 
  for (int k=19; k<tmp.size(); k++) {
    string value = tmp2[k] + boost::lexical_cast<string>(n);
    if (k==tmp.size()-1) {
      int num = (int)(rand()%10);
      for (int j=0; j<num; j++) {
        int v = (int)(rand()%100000);
        value += "_" + boost::lexical_cast<string>(v);
      }
    }
    ret = xtractor.add_field_value(value.c_str(), tmp[k], true);
    if (ret != RC_SUCCESS)
      fprintf(stderr, "error %s", tmp[k].c_str());
  }

  ret = xtractor.extract_features_from_record();

  // 从xtractor获取FeaResultSet
  const FeaResultSet& fea_result_set = xtractor.get_fea_result_set(); 

  uint32_t fea_result_num = fea_result_set.size();
  for (uint32_t i = 0; i < fea_result_num; ++i) {
    const fea_result_t* fea_result = fea_result_set.get_fea_result(i);
    if (NULL == fea_result) continue;
    if (!fea_result->is_valid) continue;
    count += 1;
    //printf("ins %d slot %d, text %s ->\t\t\t sign %ld\n", n, fea_result->fea_slot, fea_result->fea_text, fea_result->final_fea_sign);
  }

  }
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  long timeuse = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
  printf("time use %.0f\n", timeuse/1000.0); 
}
