express = require 'express'
fs = require 'fs'
spawn = require('child_process').spawn
# mongoose = require 'mongoose'
getenv = require 'getenv'

DB_PATH = 'mongodb://127.0.0.1/test'

# SourceCodeSchema = new mongoose.Schema
#     text : String
#     type : String
#     ir : mongoose.Schema.Types.Mixed
# 
# SourceCode = mongoose.model 'SourceCode', SourceCodeSchema

app = express()

app.configure () ->
    app.use express.bodyParser()

    # mongoose.connect DB_PATH, (err) ->
    #     if err
    #         throw err
    #     return

    # return
    

app.get '/', (req, res) ->
    res.redirect('index.html')
    return

app.get /\.(html|css|js)$/, (req, res) ->
    res.sendfile req.path, {'root' : '../client'}
    return

app.post '/llvm/source', (req, res) ->

    sourceCode = new SourceCode
        text : req.param 'text'
        type : req.param 'type'
        ir : {}
    res.type 'application/json'
    
    TMP_C_SOURCE = './tmp/test.c'
    TMP_IR_CODE = './tmp/test.s'
    TMP_JSON_OUTPUT = getenv('CHOPPER_JSON')

    fs.writeFile TMP_C_SOURCE, sourceCode.text, (err) ->
        if err
            res.json 500, error : 'fail to write source code'
            return
        compiler = spawn 'clang-3.2', ['-emit-llvm','-S',
            '-o', TMP_IR_CODE, TMP_C_SOURCE]
        compiler.on 'exit', (code) ->
            if code is 0
                console.log 'successfully compiled'
                """
                opt-3.2 -S -load ./PDGPass.so -postdomtree -basicaa -globalsmodref-aa -scev-aa -aa-eval -memdep -pdg < $IR_FILE > result.ll
                """
                irFile = fs.openSync TMP_IR_CODE, 'r'
                pdgpass = spawn 'opt-3.2', ['-S', '-load',
                    '../src/PDGPass.so', '-postdomtree', '-basicaa'
                    '-globalsmodref-aa', '-scev-aa', '-aa-eval',
                    '-memdep', '-pdg'], {stdio : [irFile, null, null]}

                pdgpass.on 'exit', (code) ->
                    fs.closeSync irFile
                    if code is 0
                        fs.readFile TMP_JSON_OUTPUT, (err, data) ->
                            if err
                                res.json 500, error : 'Fail to open irfile'
                            else
                                sourceCode.ir = JSON.parse data
                                res.json 200,
                                    'ir' : sourceCode.ir
                                    '_id' : 0
                            false
                    else
                        res.json 500, error : 'PDGPass failed'
                    return

            else
                res.json 500,
                    error : 'Syntax error'
            return
        return

    """
    sourceCode.save (err) ->
        if err
            console.log err
        else
            console.log sourceCode
            res.json
                '_id' : sourceCode['_id']
        return


    llvm_cfg = spawn 'opt-3.2', ['-load', './llvm-pass/Cfg.so',
        '-cfg-json']

    console.log 'Spawned child pid :', llvm_cfg.pid

    cfg_data = '['

    llvm_cfg.stderr.on 'data', (data) =>
        cfg_data += data.toString()
        false
    """

    return

app.listen 3456

