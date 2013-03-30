JADE   := jade
COFFEE := coffee

COFFEE_SRCS   := $(wildcard js/*.coffee)
JS_OUTPUTS    := $(COFFEE_SRCS:%.coffee=%.js)

JADE_SRCS     := $(wildcard *.jade)
HTML_OUTPUTS  := $(JADE_SRCS:%.jade=%.html)

REVEAL_JS := js/reveal.js \
	js/head.min.js \
	js/markdown.js \
	js/showdown.js \
	js/highlight.js \
	css/reveal.css \
	css/zenburn.css \
	css/theme-default.css \
	css/theme-beige.css 

VENDOR_OUTPUTS := $(REVEAL_JS)

OUTPUTS := $(JS_OUTPUTS) $(HTML_OUTPUTS) \
	$(VENDOR_OUTPUTS)

.PHONY : all clean
all : $(OUTPUTS)

clean :
	rm -f $(OUTPUTS)

%.js : %.coffee
	$(COFFEE) -b -c $<

%.html : %.jade
	$(JADE) -P < $< > $@

# copying reveal.js
js/head.min.js : vendor/reveal-js/lib/js/head.min.js 
	cp $< $@

js/reveal.js : vendor/reveal-js/js/reveal.js 
	cp $< $@

js/markdown.js : vendor/reveal-js/plugin/markdown/markdown.js 
	cp $< $@

js/showdown.js : vendor/reveal-js/plugin/markdown/showdown.js 
	cp $< $@

js/highlight.js : vendor/reveal-js/plugin/highlight/highlight.js 
	cp $< $@

css/reveal.css : vendor/reveal-js/css/reveal.css
	cp $< $@

css/theme-default.css : vendor/reveal-js/css/theme/default.css
	cp $< $@

css/theme-beige.css : vendor/reveal-js/css/theme/beige.css
	cp $< $@

css/zenburn.css : vendor/reveal-js/lib/css/zenburn.css
	cp $< $@


