# for llvm-3.2
CXX := clang++-3.2
CFLAGS := -Wall -std=c++11 -fno-rtti \
 -I/opt/local/llvm/2.9/include \
 -DNDEBUG -D_GNU_SOURCE -D__STDC_LIMIT_MACROS \
 -D__STDC_CONSTANT_MACROS -O3 -fomit-frame-pointer -fPIC
LDFLAGS := -lpthread -ldl -lm \
	-L/opt/local/llvm/2.9/lib
OPT := opt-2.9

LIBRARYNAME := SDGPass.so # CtrlDep.so DataDep.so

TEST_PROGS := 

SRCS := $(wildcard *.cc)
OBJS := $(SRCS:%.cc=%.o)

.PHONY : all clean
all : SDGPass.so

%.o : %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

SDGPass.so : $(OBJS)
	$(CXX) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) 

test :
	$(OPT) -S -load $(PWD)/pdg/PDGPass.so \
		-load $(PWD)/SDGPass.so \
    -postdomtree -basicaa -globalsmodref-aa  \
    -scev-aa  -memdep -pdg  -sdg \
		< ../test/simple.s > /dev/null
	# $(OPT) -S -load $(PWD)/SDGPass.so -sdg < ../test/recursive.s > /dev/null

clean :
	rm -f $(OBJS) $(LIBRARYNAME) *.svg *.dot 

