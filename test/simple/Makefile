include ../common.mk

.PHONY : all clean

all : simple.bc simple.ll

simple.bc : simple.c
	$(CC) $(CFLAGS) -c $< -o $@

simple.ll : simple.bc
	$(LLVM_DIS) $<
	

clean : 
	rm -f simple.bc simple.ll

