
COFFEE_SRCS := server.coffee
COFFEE := coffee
COFFEE_FLAGS := -b
SERVER_SCRIPTS := $(COFFEE_SRCS:%.coffee=%.js)


.PHONY : all clean
all : $(SERVER_SCRIPTS)

%.js : %.coffee
	$(COFFEE) $(COFFEE_FLAGS) -c $^

clean :
	rm -f $(SERVER_SCRIPTS)



