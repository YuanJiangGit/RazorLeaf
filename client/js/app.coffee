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
        className : 'ir-wrapper'

        onChangeIR : () ->
            ir = @model.get 'ir'
            console.log ir
            for func, i in ir
                func.id = i
            
            @$el.html @template ir

            $('.inst').on 'click', (e) =>
                e.preventDefault()
                e.stopPropagation()
                console.log e.target
                false

            return

        onChangePDG : () ->
            return
        
        initialize : () ->
            @model.on 'change', () =>
                # console.log 'changed'
                if @model.hasChanged 'ir'
                    @onChangeIR()

            @template = Handlebars.compile """
                {{#each this}}
                <div class=func>{{this.name}}
                    {{#each this.bb}}
                    <div class=bb>{{this.name}}
                        {{#each this.inst}}
                        <div class=inst data-id={{this.id}} data-funcid={{../../id}}>{{this.content}}</div>
                        {{/each}}
                    </div>
                    {{/each}}
                </div> 
                {{/each}}
                """
            false

        render : () ->
            $('.ir-container').append(@$el)
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
                success : (data, response) =>
                    console.log arguments
                    console.log 'success: ', data
                    @model.set 'id', response['_id']
                    @model.set 'ir', response['ir']
                    @model.id = undefined

                    false
                error : (e) ->
                    console.log 'error: ', e
                    false

            false

        initialize : () ->
            @$el.html 'Compile2LLVM IR'
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

