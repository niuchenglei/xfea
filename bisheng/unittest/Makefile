
ECHO="/bin/echo"
COLOR_START="\033[33m"
COLOR_END="\033[0m"

CXX=g++
RM=rm
##CXXFLAGS= -g -W -Wall -O2 -std=c++11 -Dprivate=public -Dprotected=public
CXXFLAGS= -g -W -Wall -O2 -Dprivate=public -Dprotected=public
VERSION=$(shell git describe --tags | awk -F '-' '{ print $1"."$2;}' | tr -d 'v' )

BUILD_DIR=build
OBJS=$(patsubst %.cpp,%.o,$(shell find ./ -name "*.cpp" -print))
DEPEND_INCPATH=-I../build/bisheng/include -I./deps/gtest/googletest/include/ 
BISHENG_LIB=../build/bisheng/lib/libbisheng_offline.a
DEPEND_LIB=-lpthread
GTEST_LIB=./deps/gtest/googletest/lib/libgtest.a
ALL_BIN_NAME=$(BUILD_DIR)/all_unittest

## 所有的unit test case会编译到一个bin中
## 想成单独bin时可以修改BIN_NAME和TEST_FILE配置, 会额外生成一个bin
BIN_NAME=$(BUILD_DIR)/seperate_test
TEST_FILE=feature_op/test_S_ctr_bin.cpp

all: $(ALL_BIN_NAME) $(BIN_NAME)
	@mkdir -p $(BUILD_DIR)
	@${ECHO} -e $(COLOR_START)"[make all succ]"$(COLOR_END)

$(ALL_BIN_NAME): $(OBJS) $(BISHENG_LIB) $(GTEST_LIB)
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $^ $(DEPEND_INCPATH) $(DEPEND_LIB) -o $@

$(BIN_NAME): $(TEST_FILE) $(BISHENG_LIB) $(GTEST_LIB)
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $^ $(DEPEND_INCPATH) $(DEPEND_LIB) -o $(BIN_NAME)

%.o: %.cpp ../build/bisheng/include/*/*.h
	@echo "COMPILING[Target:'$@', Using:'$<'] beginning..."
	@$(CXX) -c $(CXXFLAGS) $(DEPEND_INCPATH) $< -o $@ 
	@${ECHO} -e $(COLOR_START)"COMPILING[Target:'$@', Using:'$<'] successfully."${COLOR_END}

clean:
	-@$(RM) -rf $(BUILD_DIR)
	-@$(RM) -rf ./*/*.o
	@${ECHO} -e $(COLOR_START)"[make clean done]"${COLOR_END}

