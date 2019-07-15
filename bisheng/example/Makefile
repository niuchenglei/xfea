
ECHO="/bin/echo"
COLOR_START="\033[33m"
COLOR_END="\033[0m"

CXX=g++
RM=rm
##CXXFLAGS= -g -W -Wall -O2 -std=c++11
CXXFLAGS= -g -W -Wall -O2
VERSION=$(shell git describe --tags | awk -F '-' '{ print $1"."$2;}' | tr -d 'v' )

DEPEND_INCPATH=-I../build/bisheng/include
BISHENG_LIBPATH=../build/bisheng/lib/libbisheng_offline.a

BIN_NAME=main

all: extrator_example.cpp $(BISHENG_LIBPATH)
	@$(CXX) $(CXXFLAGS) $^ $(DEPEND_INCPATH) -o $(BIN_NAME)
	@${ECHO} -e $(COLOR_START)"[make all succ]"$(COLOR_END)


clean:
	-@$(RM) -rf *.o $(BIN_NAME)
	@${ECHO} -e $(COLOR_START)"[make clean done]"${COLOR_END}

