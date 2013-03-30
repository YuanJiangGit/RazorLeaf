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


