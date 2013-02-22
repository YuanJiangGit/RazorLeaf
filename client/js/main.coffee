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

requirejs ['app'], (app) ->
    app.init()
    false

