Reveal.initialize
  controls : true
  keyboard : true
  progress : true
  overview : true
  # center : true
  transition : 'linear'
  dependencies : [
    {
      src : 'js/highlight.js'
      async : true
      callback : () ->
        hljs.initHighlightingOnLoad()
    }
    {
      src : 'js/showdown.js'
      condition : () ->
        !!document.querySelector '[data-markdown]'
    }
    {
      src : 'js/markdown.js'
      condition : () ->
        !!document.querySelector '[data-markdown]'
    }
  ]

timebar = document.getElementById 'timebar'

Reveal.addEventListener 'slidechanged', (e) ->
  if e.indexh is 1 and e.indexv is 0
    TOTAL_TIME = 600  # 10 minutes
    tick = 0
    onTick = () ->
      if tick <= TOTAL_TIME
        width = tick / TOTAL_TIME * window.innerWidth
        timebar.style.width = "#{width}px"
        tick += 1

    setInterval onTick, 1000


