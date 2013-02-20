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

    paths :
        'app' : '../js/app'

requirejs ['app'], (app) ->
    app.init()
    false

