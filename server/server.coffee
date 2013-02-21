express = require 'express'
fs = require 'fs'
spawn = require('child_process').spawn
mongoose = require 'mongoose'

DB_PATH = 'mongodb://127.0.0.1/test'

SourceCodeSchema = new mongoose.Schema
    text : 'string'
    type : 'string'
    llvmir : 'string'

SourceCode = mongoose.model 'SourceCode', SourceCodeSchema

app = express()

app.configure () ->
    app.use express.bodyParser()

    mongoose.connect DB_PATH, (err) ->
        if err
            throw err
        return

    return
    

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
        llvmir : ''
    res.type 'application/json'


    fs.writeFile './tmp/test.c', sourceCode.text, (err) ->
        if err
            res.json 500, error : 'fail to write source code'
            return
        compiler = spawn 'clang-3.2', ['-emit-llvm','-S',
            '-o','./tmp/test.s', './tmp/test.c']
        compiler.on 'exit', (code) ->
            if code is 0
                console.log 'successfully compiled'
                res.json '_id' : 123
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

