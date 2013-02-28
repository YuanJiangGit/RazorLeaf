define ['jquery', 'underscore', 'backbone', 'handlebars', 'chopper/slicer', 'chopper/mediator', 'ace/ace'], ($, _, Backbone, Handlebars, Slicer, mediator, ace) ->


    SourceCode = Backbone.Model.extend
        defaults :
            text : ''
            type : ''
            ir : {}
            sliceCriterion : {}
            sliceResult : {}
            id : ''
        urlRoot : '/llvm/source'

    editorId = 'editor'
    editor = ace.edit editorId
    editor.setTheme 'ace/theme/monokai'
    editor.getSession().setMode('ace/mode/c_cpp')

    IRContainer = Backbone.View.extend
        tagName : 'div'
        className : 'ir-wrapper'

        onChangeIR : () ->
            ir = @model.get 'ir'
            for func, i in ir
                func.id = i
            
            @$el.html @template ir

            $('.inst').on 'click', (e) =>
                e.preventDefault()
                e.stopPropagation()
                target = e.target
                instId = parseInt target.dataset['id']
                funcId = parseInt target.dataset['funcid']

                $('.inst').removeClass 'sliceInst'
                $(e.target).addClass 'sliceInst'

                @model.set 'sliceCriterion',
                    'id' : instId
                    'funcid' : funcId
                $('.info-area').html "id: #{instId}, funcid :#{funcId}"
                false

            return

        onChangePDG : () ->
            return

        onChangeSliceResult : () ->
            sliceResult = @model.get 'sliceResult'
            sliceCriterion = @model.get 'sliceCriterion'

            if not (sliceResult.hasOwnProperty 'type')
                $('.inst').removeClass 'sliced'
                return

            $funcDiv =
                $(".func[data-funcid=#{sliceCriterion['funcid']}]")
            _.each sliceResult.inst, (sr) ->
                realId = sr['realId']
                $funcDiv.find(".inst[data-id=#{realId}]")
                    .addClass('sliced')

                return

            if sliceResult.type is 'forward'
                _.each sliceResult.bb, (sr) ->
                    realId = sr['realId']
                    $($funcDiv.find('.bb')[realId]).find('.inst')
                        .addClass('sliced')
                    return
                mediator.trigger 'slice:success'
            else if sliceResult.type is 'backward'
                _.each sliceResult.bb, (sr) ->
                    termInstId = sr['termInst']
                    $funcDiv.find(".inst[data-id=#{termInstId}]")
                        .addClass('sliced')
                mediator.trigger 'slice:success'
            else
                console.warn 'Unknown slice result type'

            return
        
        initialize : () ->

            mediator.on 'compile:success', () ->
                false

            @model.on 'change', () =>
                # console.log 'changed'
                if @model.hasChanged 'ir'
                    @onChangeIR()
                else if @model.hasChanged 'sliceResult'
                    @onChangeSliceResult()

            @template = Handlebars.compile """
                {{#each this}}
                <div class=func data-funcid={{this.id}}>{{this.name}}
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
        tagName : 'div'
        className : 'button bt-compile'
        events :
            'click' : 'onClick'
        onClick : (e) ->
            e.preventDefault()
            e.stopPropagation()
            @model.set 'text', editor.getValue()
            Backbone.sync 'create', @model,
                success : (data, response) =>
                    @model.set 'id', response['_id']
                    @model.set 'ir', response['ir']
                    @model.id = undefined
                    mediator.trigger 'compile:success'
                    false
                error : (e) ->
                    console.log arguments
                    mediator.trigger 'compile:fail'
                    false

            false

        initialize : () ->
            @$el.html 'Compile2LLVM IR'
            false

        render : () ->
            @

    ControlPanel = Backbone.View.extend
        initialize : () ->
            @$el = $('.control-panel')
            @compileButton =
                new CompileButton({model : @model})
            @fSliceButton =
                new Slicer.ForwardSliceButton({model : @model})
            @bSliceButton =
                new Slicer.BackwardSliceButton({model : @model})
            false

            @$btReslice = $('<div>').addClass('button bt-reslice')
                .html('re-slice')

            mediator.on 'compile:success', () =>
                @compileButton.$el.addClass 'bt-disabled'
                @$el.append @fSliceButton.$el
                @$el.append @bSliceButton.$el
                false

            mediator.on 'slice:success', () =>
                @fSliceButton.$el.addClass 'bt-disabled'
                @bSliceButton.$el.addClass 'bt-disabled'
                @$btReslice.on 'click', (e) =>
                    e.preventDefault()
                    e.stopPropagation()
                    @fSliceButton.$el.removeClass 'bt-disabled'
                    @bSliceButton.$el.removeClass 'bt-disabled'
                    @model.set 'sliceResult', {}
                    $(e.target).remove()
                    false
                @$el.append @$btReslice
                false

        render : () ->
            @$el.append @compileButton.$el
            @



    sourceCode = new SourceCode()
    irContainer = new IRContainer({model : sourceCode})
    controlPanel = new ControlPanel({model : sourceCode})

    mediator.on 'compile:success', () ->
        $("##{editorId}").hide()
        false

    init : () ->
        uis = [ irContainer, controlPanel ]
        _.each uis, (ui) ->
            ui.render()
        false

