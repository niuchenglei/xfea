#include <vector>
#include <iostream>
#include <sstream>
#include "stdlib.h"
#include <boost/lexical_cast.hpp>

#include "offline_extractor.h"
#include "util.h"

namespace xfea { namespace offxtractor {

OfflineExtractor::OfflineExtractor() {
}
OfflineExtractor::~OfflineExtractor() {
}

bool OfflineExtractor::Finalize() {
    _xtractor.finalize();
    return true;
}

int OfflineExtractor::Init(const string& config_path, const string& input_schema) {
    // 初始化_xtractor
    if (_xtractor.init(config_path) != RC_SUCCESS) {
        std::cerr << "_xtractor init failed by config file " << config_path << std::endl;
        return -1;
    }
    std::vector<std::string> items;
    ADutil::Split2(items, input_schema, ',');
    boost::unordered_map<std::string, int> input_schema_m;
    for(int i = 0; i < int(items.size()); ++ i) {
        input_schema_m[items[i]] = i;
    }

    //(hive)input schema example:
    //  pvid,adid,markid,uid,custid,custid_bidtype,feedid,label,it_cnt,is_cnt,ig_cnt,iv_cnt,im_cnt
    //bisheng schema example:
    //  feedid, custid
    const std::map<std::string, int>& bisheng_schema=_xtractor.get_input_schema_key_index_map();
    std::map<std::string, int>::const_iterator Iter = bisheng_schema.begin();
    if(!input_schema_m.empty()) {
        for(; Iter != bisheng_schema.end(); ++ Iter) {
            std::string bisheng_fea_name(Iter->first);
            if(input_schema_m.find(bisheng_fea_name) != input_schema_m.end()) {
                int column_index = input_schema_m[bisheng_fea_name];
                _schema_index_m[Iter->second] = column_index;
            }
        }
    }

    std::cerr << " _xtractor input_schema_m size: " << input_schema_m.size() << std::endl;
    return 0;
}
void OfflineExtractor::MapOutput(const FeaResultSet& fea_result_set, const std::string& clk) {
    uint32_t fea_result_num = fea_result_set.size();
    if(fea_result_num <= 0) {
        std::cerr << "fea result num is zero";
        return;
    }
    //std::cout << "Instance result: " << std::endl;
    std::ostringstream oss;
    for (uint32_t i = 0; i < fea_result_num; ++i) {
         const fea_result_t* fea_result = fea_result_set.get_fea_result(i);
         if (NULL == fea_result) {
            std::cerr << "get index " << i << " fea_result failed!" << std::endl;
            return;
         } else if (fea_result->is_valid == false) {
            return;
         } else {
            oss << fea_result->final_fea_sign << ":" << fea_result->fea_slot << " ";
            //slot:sign:{text}\tview\tclk\tfeature\n;
            std::string fea_text(fea_result->fea_text);
            // 讨论：view置为1在click>1时不合理
            std::cout << fea_result->fea_slot << ":" << fea_result->final_fea_sign << ":" << "{" << fea_text << "}\t1\t" << clk << "\tfeature" << std::endl;
         }
    }
    std::string instance(oss.str());
    //11247741169237772917:1 323870924045377026:7 1145691244457766:127\tview\tclk\tinstance\n
    std::cout << ADutil::Trim(instance) << "\t1\t" << clk << "\tinstance" << std::endl;
}

int OfflineExtractor::Map(const string &line, const int delimiter) {
    int ret = 0;
    _xtractor.one_round_reset();
    //--重点--：请按照配置里头的schema，将样本相应的列填入
    //最好的办法就是：你的schema跟你的原始样本的字段顺序是一致的
    std::vector<std::string> items;
    if(delimiter == Delimiter::TAB_DELIMITER) {
        ADutil::Split2(items, line, '\t'); //'\t'
    } else if(delimiter == Delimiter::SPACE_DELIMITER) {
        ADutil::Split2(items, line, ' '); //' '
    } else if(delimiter == Delimiter::CTRLA_DELIMITER) {
        ADutil::Split2(items, line, '\x01'); //'\x01'
    } else if(delimiter == Delimiter::CTRLB_DELIMITER) {
        ADutil::Split2(items, line, '\x02'); //'\x01'
    } else if(delimiter == Delimiter::CTRLC_DELIMITER) {
        ADutil::Split2(items, line, '\x03'); //'\x01'
    } else if(delimiter == Delimiter::CTRLI_DELIMITER) {
        ADutil::Split2(items, line, '\x09'); //'\x01'
    }  

    std::string label("");
    std::string label_field = _xtractor.get_label_field();

    //如果hive表的schema不一致，遍历bisheng的schema, 通过特征名拿到hive表相应列的字段。
    //请注意：字段名字顺序可以不一致，但是名字命名要一致，比如bisheng的schema把微博id叫mid，hive表的schema的微博id也应该叫mid
    //input schema example:
    //       pvid,adid,markid,uid,custid,custid_bidtype,feedid,label,it_cnt,is_cnt,ig_cnt,iv_cnt,im_cnt
    //bisheng schema:
    //    feedid, custid
    const std::map<std::string, int>& bisheng_schema=_xtractor.get_input_schema_key_index_map();
    std::map<std::string, int>::const_iterator Iter = bisheng_schema.begin();
    if(!_schema_index_m.empty()) {
        for(; Iter != bisheng_schema.end(); ++ Iter) {
            std::string bisheng_fea_name(Iter->first);
            int bisheng_index = Iter->second;
            int column_index = _schema_index_m[bisheng_index];
            if(column_index < int(items.size())) {
                ReturnCode ret = _xtractor.add_field_value(items[column_index], bisheng_index);
                if (RC_ERROR == ret) {
                    std::cerr << "add fea " << bisheng_fea_name << " occurs fatal error!" << std::endl;
                    return -1;
                } else if (RC_WARNING == ret) {
                    std::cerr << "add fea " << bisheng_fea_name << " occurs warn error!" << std::endl;
                    continue;
                }
                if(bisheng_fea_name.compare(label_field) == 0) {
                    label.assign(items[column_index]);
                }
            } else {
                std::cerr << "column_index:" << column_index << " bigger than columns size:" << items.size() << std::endl;
                return -1;
            }
        }
    } else {
        //外部保证hdfs文件列跟bisheng的schema顺序一致的情况，第一个元素是label
        // schema数目不能大于输入日志的实际字段
        if(bisheng_schema.size() > items.size()) {
            std::cerr << "bisheng schema size:" << bisheng_schema.size() << " is greater than column size:" << items.size() << std::endl;
            return -1;
        }
        for(int i = 0; i < int(bisheng_schema.size()); ++i) {
            std::map<std::string, int>::const_iterator _iter = bisheng_schema.find(label_field);
            if (_iter != bisheng_schema.end() && _iter->second == i) {
                label.assign(items[i]);
            }
            // std::string bisheng_fea_name(Iter->first);
            ReturnCode ret = _xtractor.add_field_value(items[i], i);
            if (RC_ERROR == ret) {
                std::cerr << "add fea " << items[i] << " to index " << i << " occurs fatal error!" << std::endl;
                return -1;
            } else if (RC_WARNING == ret) {
                std::cerr << "add fea " << items[i] << " to index " << i << " occurs warn!" << std::endl;
                continue;
            }
        }
    }
    // _xtractor抽取特征
    ret = _xtractor.extract_features_from_record();
    if (RC_ERROR == ret) {
        std::cerr << "extract_features_from_record() for line [" << line << "] occurs fatal error!" << std::endl;
        return -1;
    } else if (RC_WARNING == ret) {
        std::cerr << "extract_features_from_record() for line [" << line << "] occurs warn!" << std::endl;
        return -2;
        // continue;
    }
    // 从_xtractor获取FeaResultSet
    const FeaResultSet& fea_result_set = _xtractor.get_fea_result_set();    
    MapOutput(fea_result_set, label);
    return 0;
}

void OfflineExtractor::ReduceOutput(const string &ins_fea, long int &show, long int &clk, const string &tag) {
    if (tag == "instance") {
        cout << show << " " << clk << " " << ins_fea << "\tinstance" << endl;
    } else if (tag == "feature") {
        float ctr = clk / float(show);
        cout << ins_fea << "\t" << show << "\t" << clk << "\t" << ctr << "\tfeature" << endl;
    }
}
/*
* instance: 11247741169237772917:1 323870924045377026:7 1145691244457766:127\tview\tclk\tinstance\n
* feature:  slot:sign:{text}\tview\tclk\tfeature\n;
*/
void OfflineExtractor::Reduce(const std::string& op, const std::string& pattern) {
    // combine是本地reduce，样本的聚合不能在本阶段进行
    if(op.compare("combine") == 0) {
        ReduceMerge("feature");
    } else {
        if (pattern.compare("fanstop") == 0) { 
            ReduceMerge("");
        } else {
            ReduceHebe();
        }
    }
}
/*
** 后续可以考虑把ReduceHebe和ReduceMerge合成一个逻辑更复杂些的实现
*/
void OfflineExtractor::ReduceHebe() {
    string line;
    string lastkey = "";
    long int fea_show, fea_clk, cur_clk;
    string ins_fea, tag;
    fea_show = fea_clk = 0;
    while (getline(cin, line)) {
        std::vector<std::string> items;
        line = ADutil::Trim(line);
        ADutil::Split2(items, line, '\t');
        int length = items.size();
        if (length < 4) {
            std::cerr << "bad line:" << line << std::endl;
            continue;
        }
        string multiout_key = ADutil::Trim(items[length-1]);
        std::string out_str;
        /*
        ** 从格式 11247741169237772917:1 323870924045377026:7 1145691244457766:127\tview\tclk\tinstance\n
        ** 变成 clk 11247741169237772917:1 323870924045377026:1 1145691244457766:1 
        */
      if(multiout_key.compare("instance") == 0) {
            out_str.append(items[2]).append(" ");
            std::vector<std::string> fea_v;
            ADutil::Split2(fea_v, items[0], ' ');
            std::map<uint64_t, int> m_sort;
            for(int i = 0; i < int(fea_v.size()); ++ i) {
                std::vector<std::string> fea_slot;
                ADutil::Split2(fea_slot, fea_v[i], ':');
                if(int(fea_slot.size()) > 1) {
                    m_sort.insert(std::make_pair(std::strtoull(fea_slot[0].c_str(), NULL, 10), 1));
                }
            }
            std::map<uint64_t, int>::iterator it;
            for(it = m_sort.begin(); it != m_sort.end(); ++ it) {
                out_str.append(boost::lexical_cast<std::string>(it->first)).append(":1 ");
            }
            out_str = ADutil::Trim(out_str);
            out_str.append("\tinstance");
            std::cout << out_str << std::endl;
        } else if(multiout_key.compare("feature") == 0) {
            // 对于feature，总是进行聚合
            string curkey = ADutil::Trim(items[0]);
            if (lastkey != curkey && lastkey != "") {
                ReduceOutput(ins_fea, fea_show, fea_clk, "feature");
                fea_show = 0;
                fea_clk = 0;
            } 
            cur_clk = atol(items[2].c_str());    
            fea_clk += cur_clk; 
            ins_fea = curkey;
            fea_show += atol(items[1].c_str());
            lastkey = curkey;
        } else {
            std::cerr << "line data error:" << line;
        }
    }

    if (lastkey != "") {
        ReduceOutput(ins_fea, fea_show, fea_clk, "feature");
        fea_show = 0;
        fea_clk = 0;
    }
}
/*
** 样本和特征都聚合
** 在本函数，通过参数filter_in也可以只对样本或者特征做聚合
*/
void OfflineExtractor::ReduceMerge(const std::string& filter_in) {
    string line;
    string lastkey = "";
    vector<string> record;
    string ins_fea, tag;
    long int show = 0;
    long int clk = 0, cur_clk;

    while (getline(cin, line)) {
        line = ADutil::Trim(line);
        ADutil::Split2(record, line, '\t');
        int length = record.size();
        if (length == 0) {
            cerr << "blank line" << endl;
            continue;
        }
        string multiout_key = ADutil::Trim(record[length-1]);
        // 某些时候，只需要对feature做聚合，比如combiner对feature聚合
        if(!filter_in.empty() && filter_in.compare(multiout_key) != 0) {
            // 不聚合情况原样输出，比如样本格式维持下面的不变
            // 11247741169237772917:1 323870924045377026:7 1145691244457766:127\tview\tclk\tinstance\n
            std::cout << line << std::endl;
            continue; 
        }
        string curkey = ADutil::Trim(record[0]);

        if (lastkey != curkey && lastkey != "") {
           ReduceOutput(ins_fea, show, clk, tag);
           show = 0;
           clk=0;
        } 

        cur_clk = atol(record[2].c_str());
        clk += cur_clk; 
        ins_fea = curkey;
        if (multiout_key.compare("instance") == 0) {
            tag = "instance";
        } else if (multiout_key.compare("feature") == 0) {
            tag = "feature";
        }

        //最原始的view是1，combiner聚合后可能是一个大于1的数
        show += atol(record[1].c_str());
        lastkey = curkey;

    }
    if (lastkey != "") {
        ReduceOutput(ins_fea, show, clk, tag);
        show = 0;
        clk=0;
    }
}

}}
