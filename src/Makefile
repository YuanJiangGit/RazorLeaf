# for llvm-3.2
CXX := clang++-3.2
CFLAGS := -Wall `llvm-config-3.2 --cflags` -std=c++11 -fno-rtti -D_DEBUG
LDFLAGS := `llvm-config-3.2 --ldflags` -llog4cxx
LIBS := `llvm-config-3.2 --libs`
OPT := opt-3.2

LIBRARYNAME := PDGPass.so # CtrlDep.so DataDep.so

TEST_PROGS := 

SRCS := CtrlDep.cc GraphWriter.cc PDGPass.cc PDG.cc Serializer.cc
OBJS := $(SRCS:%.cc=%.o)

.PHONY : all clean
all : $(LIBRARYNAME)

%.o : %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

PDGPass.so : GraphWriter.o PDGPass.o CtrlDep.o PDG.o Serializer.o
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) ./libjson.a

clean :
	rm -f $(OBJS) $(LIBRARYNAME) *.svg *.dot *.json

.PHONY : testsimple cleansimple
testsimple : ../test/simple/simple.bc
	$(OPT) -load ./CtrlDep.so -postdomtree -ctrl-dep < $< | grep "^\[" > debug.log
	dot -Tsvg cd.main.dot > cd.simple.svg

cleangraph :
	rm *.svg *.dot

test : 
	# $(OPT) -load ./CtrlDep.so -postdomtree -ctrl-dep < ../test/cksum.bc | grep "^\[" > debug.log
	./test.sh
	#$(OPT) -load ./CtrlDep.so -postdomtree -ctrl-dep < ../test/cp.bc | grep "^\[" > debug.log

