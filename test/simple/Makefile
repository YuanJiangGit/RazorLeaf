include ../common.mk

.PHONY : all clean

SRCS := $(wildcard *.c)
BYTE_CODES := $(SRCS:%.c=%.bc)
IRS := $(SRCS:%.c=%.ll)

all : $(BYTE_CODES) $(IRS)

%.bc : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.ll : %.bc
	$(LLVM_DIS) $<
	

clean : 
	rm -f $(BYTE_CODES) $(IRS)

