#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "offline_extractor.h"
#include "util.h"
#include "ini_reader.h"

using namespace std;
using namespace xfea::offxtractor;

namespace {

int run(int argc, char** argv) {
    OfflineExtractor xtractor;

    if(argc < 2){
        std::cerr << "please input [map ort reduce]" << endl;
        return -1;
    }

    std::string config_path = "./offline.conf";
    if(argc >= 3) {
       config_path = argv[2];
    }
    INIReader ini_reader;
    int ret = ini_reader.Init(config_path);
    if(0 != ret) {
        std::cerr << "fail to read conf from " << config_path << " failed!" << endl;
        return -1;
    }
    //string bisheng_conf("./bisheng.conf");
    string bisheng_conf;
    ini_reader.Get("", "bisheng_conf", bisheng_conf, "");
    if(bisheng_conf.empty()) {
        std::cerr << "bisheng conf is not set" << std::endl;
        return -1;
    }
    string input_schema;
    ini_reader.Get("", "input_schema", input_schema, "");
    string pattern;  
    ini_reader.Get("", "output_pattern", pattern, "");
    if(pattern.empty()) {
        std::cerr << "output pattern is not set" << std::endl;
        return -1;
    }
    int delimiter;
    string delimiter_str;
    ini_reader.Get("", "delimiter", delimiter_str, "");
    if(delimiter_str.compare("'\\t'") == 0) {
        delimiter = Delimiter::TAB_DELIMITER;
    } else if (delimiter_str.compare("' '") == 0) {
        delimiter = Delimiter::SPACE_DELIMITER;
    } else if (delimiter_str.compare("'\\x01'") == 0) {
        delimiter = Delimiter::CTRLA_DELIMITER;
    } else if (delimiter_str.compare("'\\x02'") == 0) {
        delimiter = Delimiter::CTRLB_DELIMITER;
    } else if (delimiter_str.compare("'\\x03'") == 0) {
        delimiter = Delimiter::CTRLC_DELIMITER;
    } else if (delimiter_str.compare("'\\x09'") == 0) {
        delimiter = Delimiter::CTRLI_DELIMITER;
    }else {
        std::cerr << "delimiter: " << delimiter_str << " is not recognized" << endl;
        return -1;
    }

    if (argc >= 2) {
        if(strcmp("map", argv[1]) == 0) {
            if (xtractor.Init(bisheng_conf, input_schema) != 0) {
                 std::cerr << "offline extrator init failed!" << std::endl;
                 return -1;
            }
            string line;
            while (getline(cin, line)) {
                line = ADutil::Trim(line);
                if (xtractor.Map(line, delimiter) != 0) {
                    std::cerr << "occur error when handling line [" << line << "]." << std::endl;
                    return -1;
                }
            }
            xtractor.Finalize();
        } else if (strcmp("reduce", argv[1]) == 0) {
            xtractor.Reduce("reduce", pattern);
        } else if (strcmp("combine", argv[1]) == 0) {
            xtractor.Reduce("combine", pattern);
        } else {
            std::cerr <<"the argv[1] must be map or reduce or combine!" << endl;
            return -1;
        }
    }

    return 0;
}

} //end of namespace

int main(int argc, char* argv[]) {
    if (argc >= 2 && strcmp("-v", argv[1]) == 0) {
        xfea::bisheng::Extractor::print_version_info(); 
        return 0;
    } else { 
        return run(argc,argv);
    }
}
