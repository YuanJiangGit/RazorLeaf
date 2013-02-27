define ['jquery', 'underscore', 'backbone', 'handlebars', 'chopper/slicer', 'ace/ace'], ($, _, Backbone, Handlebars, Slicer, ace) ->

    SourceCode = Backbone.Model.extend
        defaults :
            text : ''
            type : ''
            ir : {}
            sliceCriterion : {}
            sliceResult : {}
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
            else if sliceResult.type is 'backward'
                _.each sliceResult.bb, (sr) ->
                    termInstId = sr['termInst']
                    $funcDiv.find(".inst[data-id=#{termInstId}]")
                        .addClass('sliced')
            else
                console.warn 'Unknown slice result type'

            return
        
        initialize : () ->
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
                    @model.set 'id', response['_id']
                    @model.set 'ir', response['ir']
                    @model.id = undefined

                    false
                error : (e) ->
                    alert e['error']
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
    fSliceButton = new Slicer.ForwardSliceButton({model : sourceCode})
    bSliceButton = new Slicer.BackwardSliceButton({model : sourceCode})

    init : () ->
        uis = [ irContainer, compileButton, fSliceButton, bSliceButton ]
        _.each uis, (ui) ->
            ui.render()
        false

