define ['jquery', 'underscore', 'backbone', 'ace/ace'], ($, _, Backbone, ace) ->

    SourceCode = Backbone.Model.extend
        defaults :
            text : ''
            type : ''
        urlRoot : '/llvm/source'

    editor = ace.edit 'editor'
    editor.setTheme 'ace/theme/monokai'
    editor.getSession().setMode('ace/mode/c_cpp')
    
    CompileButton = Backbone.View.extend
        tagName : 'button'
        className : 'bt-cc'
        events :
            'click' : 'onClick'
        onClick : (e) ->
            e.preventDefault()
            e.stopPropagation()
            @model.set 'text', editor.getValue()
            Backbone.sync 'create', @model,
                success : (data) ->
                    console.log 'success: ', data
                    false
                error : (e) ->
                    console.log 'error: ', e
                    false

            false

        initialize : () ->
            @$el.html 'Compiler2LLVM'
            false

        render : () ->
            $('body').append(@$el)
            @

    init : () ->
        sourceCode = new SourceCode({text:'aasdf'})
        uis = [ new CompileButton({model : sourceCode}) ]
        _.each uis, (ui) ->
            ui.render()
        false

