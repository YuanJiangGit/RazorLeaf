# for llvm-3.2
CXX := clang++-3.2
CFLAGS := -Wall -std=c++11 -fno-rtti \
 -I/opt/local/llvm/2.9/include \
 -DNDEBUG -D_GNU_SOURCE -D__STDC_LIMIT_MACROS \
 -D__STDC_CONSTANT_MACROS -O3 -fomit-frame-pointer -fPIC
LDFLAGS := -lpthread -ldl -lm \
	-L/opt/local/llvm/2.9/lib
OPT := opt-2.9

LIBRARYNAME := PDGPass.so # CtrlDep.so DataDep.so

TEST_PROGS := 

SRCS := CtrlDep.cc GraphWriter.cc PDGPass.cc PDG.cc 
OBJS := $(SRCS:%.cc=%.o)

.PHONY : all clean
all : $(LIBRARYNAME) 


%.o : %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

PDGPass.so : GraphWriter.o PDGPass.o CtrlDep.o PDG.o 
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) 

clean :
	rm -f $(OBJS) $(LIBRARYNAME) 

test :
	$(OPT) -S -load $(PWD)/PDGPass.so -pdg < ../../test/simple.s > /dev/null

