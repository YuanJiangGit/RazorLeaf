# for llvm-3.2
CXX := clang++-3.2
CFLAGS := -Wall `llvm-config-3.2 --cflags` -fno-rtti -std=c++11
LDFLAGS := `llvm-config-3.2 --ldflags`
LIBS := `llvm-config-3.2 --libs`
OPT := opt-3.2

LIBRARYNAME := Test.so

TEST_PROGS := 

SRCS := Test.cc
OBJS := $(SRCS:%.cc=%.o)

all : $(LIBRARYNAME)

%.o : %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

Test.so : Test.o
	$(CXX) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LDFLAGS)

clean :
	rm -f $(OBJS) $(LIBRARYNAME)

test : 
	$(OPT) -load ./Test.so -postdomtree -test < ../tests/cksum.bc > /dev/null

