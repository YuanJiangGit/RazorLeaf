define ['backbone', 'underscore', 'jquery'], (Backbone, _, $) ->
    BackwardSliceButton = Backbone.View.extend
        tagName : 'div'
        className : 'button bt-slice'

        events :
            'click' : 'onClick'

        slice : () ->

            false

        onClick : (e) ->
            e.preventDefault()
            e.stopPropagation()
            @slice()
            false

        slice : () ->
            sc = @model.get 'sliceCriterion'
            ir = @model.get('ir')
            func = ir[sc['funcid']]
            pdg = func['pdg']
            cdg = func['cdg']
            bbs = func['bb']

            worklist = []
            instSliceResult = []
            bbSliceResult = []

            instSc = undefined
            for inst, i in pdg
                if inst.realId is sc.id
                    instSc = inst
                    break
            
            if instSc

                for v, j in pdg
                    if (_.find v.deps, (dep) -> dep.id is i) isnt undefined
                        if (_.find worklist, (x) -> x is j) is undefined
                            worklist.push j
                            instSliceResult.push j

                while worklist.length > 0
                    instCan = worklist.shift()
                    for v, j in pdg
                        if (_.find v.deps, (dep) -> dep.id is instCan) isnt undefined
                            if (_.find worklist.concat(instSliceResult), (x) -> x is j) is undefined
                                worklist.push j
                                instSliceResult.push j

                # process cdg
                processCdg = (_instSc) ->
                    bbSc = _instSc.bbId
                    bbWorkList = []

                    for bb, i in cdg
                        if bb.realId is bbSc
                            bbSc = bb
                            break

                    for v, j in cdg
                        if (_.find v.deps, (dep) -> dep.id is i)
                            if not (_.find bbSliceResult, (x) -> x is j)
                                bbSliceResult.push j
                                bbWorkList.push j
                    while bbWorkList.length > 0
                        bbCan = bbWorkList.shift()
                        for v, j in cdg
                            if (_.find v.deps, (dep) -> dep.id is bbCan) isnt undefined
                                console.log j, bbWorkList.concat(bbSliceResult)
                                console.log (_.find bbWorkList.concat(bbSliceResult), (x) -> x is j)
                                if (_.find bbWorkList.concat(bbSliceResult), (x) -> x is j) is undefined
                                    bbWorkList.push j
                                    bbSliceResult.push j
                                    console.log 'pushing', j
                    return

                processCdg instSc
            else
                console.warn 'error:', sc, 'not found'

            console.log cdg
            instSliceResult = _.map instSliceResult, (instId) ->
                pdg[instId]
            for isr in instSliceResult
                processCdg isr

            console.log bbSliceResult
            bbSliceResult = _.map bbSliceResult, (bbId) ->
                cdg[bbId]
            console.log bbSliceResult


            @model.set 'sliceResult',
                'inst' : instSliceResult,
                'bb' : bbSliceResult,
                'type' : 'backward'


            false

        initialize : () ->
            @$el.html 'backward slice'
            false

        render : () ->
            #$('body').append @$el
            @
        
        
    ForwardSliceButton = Backbone.View.extend
        tagName : 'div'
        className : 'button bt-slice'

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

            # the following function are for ctrl. deps.
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
                    if (_.find sliceResult.concat(worklist), (sr) -> vertex.realId is sr.realId)
                        return
                   
                    """
                    _.each vertex.deps, (v) ->
                        vid = pdg[v.id]['realId']

                        if (_.find sliceResult.concat(worklist), (sr) -> sr['realId'] is vid)
                            return
                        else
                            worklist.push pdg[v.id]

                        return
                    """
                    worklist.push vertex
                    sliceResult.push vertex
                    return
                #console.log worklist
                #"""


            @model.set 'sliceResult',
                'inst' : sliceResult
                'bb' : bbSliceResult
                'type' : 'forward'

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
            @$el.html 'forward slice'

            false

        render : () ->
            #$('body').append(@$el)
            @


    'ForwardSliceButton' : ForwardSliceButton
    'BackwardSliceButton' : BackwardSliceButton



