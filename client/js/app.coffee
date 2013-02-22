define ['jquery', 'underscore', 'backbone', 'handlebars', 'ace/ace'], ($, _, Backbone, Handlebars, ace) ->

    SourceCode = Backbone.Model.extend
        defaults :
            text : ''
            type : ''
            ir : {}
            id : ''
        urlRoot : '/llvm/source'

    editor = ace.edit 'editor'
    editor.setTheme 'ace/theme/monokai'
    editor.getSession().setMode('ace/mode/c_cpp')

    IRContainer = Backbone.View.extend
        tagName : 'div'
        className : 'ir-container'

        onChangeIR : () ->
            ir = @model.get 'ir'
            _.each ir, (func) ->
                false

            return

        onChangePDG : () ->
            return
        
        initialize : () ->
            @model.on 'change', () =>
                if @model.hasChanged 'ir'
                    @onChangeIR()

            false

        render : () ->
            $('body').append(@$el)
            @
    
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
                    data.set 'id', data['_id']
                    data.set 'ir', data['ir']

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

    sourceCode = new SourceCode()
    irContainer = new IRContainer({model : sourceCode})
    compileButton = new CompileButton({model : sourceCode})

    init : () ->
        uis = [ irContainer, compileButton ]
        _.each uis, (ui) ->
            ui.render()
        false

