requirejs.config
    baseUrl : 'vendor'

    shim :
        'backbone' :
            deps : ['underscore', 'jquery']
            exports : 'Backbone'
        'underscore' :
            exports : '_'
        'jquery' :
            exports : 'jQuery'
        'handlebars' :
            exports : 'Handlebars'

    paths :
        'app' : '../js/app'
        'chopper/slicer' : '../js/slicer'

    callback : () ->
        console.log arguments


requirejs ['app'], (app) ->
    app.init()
    false

