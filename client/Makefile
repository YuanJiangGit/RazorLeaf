COFFEE := coffee
COFFEE_FLAGS := -b
COFFEE_SRCS := $(wildcard js/*.coffee)
JS_SRCS := $(COFFEE_SRCS:%.coffee=%.js)

STYLUS := stylus
STYLUS_FLAGS := $(wildcard css/*.styl)
STYLUS_SRCS := $(wildcard css/*.styl)
CSS_SRCS := $(STYLUS_SRCS:%.styl=%.css)

.PHONY : all clean
all : $(JS_SRCS) $(CSS_SRCS)

%.js : %.coffee
	$(COFFEE) $(COFFEE_FLAGS) -c $^

%.css : %.styl
	$(STYLUS) $(STYLUS_FLAGS) 

clean :
	rm -f $(JS_SRCS) $(CSS_SRCS)

