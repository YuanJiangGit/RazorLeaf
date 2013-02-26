define ['jquery', 'underscore', 'backbone', 'handlebars', 'ace/ace'], ($, _, Backbone, Handlebars, ace) ->

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

            _.each sliceResult.bb, (sr) ->
                realId = sr['realId']
                $($funcDiv.find('.bb')[realId]).find('.inst')
                    .addClass('sliced')
                return



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

    SliceButton = Backbone.View.extend
        tagName : 'button'
        className : 'bt-slice'

        events :
            'click' : 'onClick'

        forwardSlice : () ->
            sc = @model.get 'sliceCriterion'
            ir = @model.get('ir')
            func = ir[sc['funcid']]
            pdg = func['pdg']
            cdg = func['cdg']
            bbs = func['bb']

            worklist = []
            sliceResult = []
            bbSliceResult = []

            # the following 2 functions are for ctrl. deps.
            # return [vertex in cdg]
            findAllBBs = (bbRealId) ->
                bbList = []
                startBB = _.find cdg, (bb) ->
                    bb.id is bbRealId
                if startBB
                    bbWorkList = []
                    bbWorkList.push startBB
                    while bbWorkList.length > 0
                        currentBB = bbWorkList.shift()

                        _.each currentBB.deps, (dep) ->
                            candidateBB = cdg[dep.id]
                            if not (_.find bbList, (_bb) -> _bb.realId is candidateBB.realId)
                                # self loop
                                bbList.push candidateBB
                                if currentBB.realId isnt candidateBB.realId
                                    # control deps should be a DAG
                                    bbWorkList.concat candidateBB.deps

                else
                    console.warn 'error in cdg:', startBB, 'not found'
                bbList

            candidate = _.find pdg, (v) ->
                v.realId is sc['id']

            worklist.push candidate

            while worklist.length > 0
                v = worklist.shift()

                # process if the instruction is a terminator
                rid = v['realId']
                bbId = v['bbId']
                bb = bbs[bbId]
                idOffset = rid - bb.startId
                console.log bb, rid, bbId
                if idOffset >= 0 and idOffset < bb.inst.length
                    termInst = bb.inst[idOffset]
                    if termInst.hasOwnProperty 'term'
                        # bbDeps : [ vertex in cdg ]
                        bbDeps = findAllBBs bb.realId
                        console.log bbDeps
                        bbSliceResult = bbSliceResult.concat bbDeps
                        _.each bbDeps, (bbDep) ->
                            bbRef = bbs[bbDep.realId]

                            _.each pdg, (vertex) ->
                                if vertex.realId >= bbRef.startId and vertex.realId < bbRef.startId + bbRef.inst.length
                                    if not (_.find worklist.concat(sliceResult), (v) -> v.realId is vertex.realId)
                                        worklist.push vertex

                                return
                            return


                        # find all deps inst
                        console.log 'termInst', termInst
                else
                    console.warn 'the json may be broken'


                _.each v.deps, (dep) ->
                    vertex = pdg[dep['id']]
                    console.log vertex
                    if (_.find sliceResult, (sr) -> vertex.realId is sr.realId)
                        return

                   
                    _.each vertex.deps, (v) ->
                        vid = v['realId']
                        if (_.find sliceResult.concat(worklist), (sr) -> sr['realId'] is vid)
                            return
                        else
                            worklist.push pdg[v.id]

                        return
                    sliceResult.push vertex
                    return
                #console.log worklist
                #"""


            @model.set 'sliceResult',
                'inst' : sliceResult
                'bb' : bbSliceResult

            false

        onClick : (e) ->
            e.preventDefault()
            e.stopPropagation()

            # TODO remove following lines
            @model.set 'sliceResult', []
            $('.inst').removeClass('sliced')

            @forwardSlice()

            false

        initialize : () ->
            @$el.html 'Slice'

            false

        render : () ->
            $('body').append(@$el)
            @

    sourceCode = new SourceCode()
    irContainer = new IRContainer({model : sourceCode})
    compileButton = new CompileButton({model : sourceCode})
    sliceButton = new SliceButton({model : sourceCode})

    init : () ->
        uis = [ irContainer, compileButton, sliceButton ]
        _.each uis, (ui) ->
            ui.render()
        false

