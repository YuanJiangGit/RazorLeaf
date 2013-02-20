define ['jquery', 'underscore', 'backbone', 'ace/ace'], ($, _, Backbone, ace) ->
    editor = ace.edit 'editor'
    editor.setTheme 'ace/theme/monokai'
    editor.getSession().setMode('ace/mode/c_cpp')

    SourceCode = Backbone.Model.extend
        defaults :
            text : ''
            type : ''
        urlRoot : 'sourcecode'

    CompileButton = Backbone.View.extend
        tagName : 'button'
        className : 'bt-cc'
        events :
            'click' : 'onClick'
        onClick : (e) ->
            e.preventDefault()
            e.stopPropagation()

            false

        initialize : () ->
            @$el.html 'Compiler2LLVM'
            false

        render : () ->
            $('body').append(@$el)
            @

    init : () ->
        uis = [ new CompileButton() ]
        _.map uis, (ui) ->
            ui.render()
        false

