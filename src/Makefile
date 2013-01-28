# for llvm-3.2
CXX := clang++-3.2
CFLAGS := -Wall `llvm-config-3.2 --cflags` -std=c++11 -fno-rtti
LDFLAGS := `llvm-config-3.2 --ldflags`
LIBS := `llvm-config-3.2 --libs`
OPT := opt-3.2

LIBRARYNAME := CtrlDep.so

TEST_PROGS := 

SRCS := CtrlDep.cc
OBJS := $(SRCS:%.cc=%.o)

all : $(LIBRARYNAME)

%.o : %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

CtrlDep.so : CtrlDep.o
	$(CXX) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LDFLAGS)

clean :
	rm -f $(OBJS) $(LIBRARYNAME)

test : 
	$(OPT) -load ./CtrlDep.so -postdomtree -ctrl-dep < ../test/cksum.bc > /dev/null

