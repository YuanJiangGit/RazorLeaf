COFFEE := coffee
COFFEE_FLAGS := -b
COFFEE_SRCS := $(wildcard js/*.coffee)
JS_SRCS := $(COFFEE_SRCS:%.coffee=%.js)

.PHONY : all clean
all : $(JS_SRCS)

%.js : %.coffee
	$(COFFEE) $(COFFEE_FLAGS) -c $^

clean :
	rm -f $(JS_SRCS)

