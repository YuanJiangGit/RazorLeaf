var fs = require('fs');

process.stdin.resume();
process.stdin.setEncoding('utf8');
var time_info = {};

process.stdin.on('data', function(chunk) {
  var lines = chunk.split('\n');
  for (var i=0; i<lines.length; i++) {
    var line = lines[i];
    var infos = line.split(":");
    if (infos.length === 2) {
      time_info[infos[0]] = infos[1];
    }
  }
});

fs.readFile('./test.json', function(err, data) {
  var pdg = JSON.parse(data);
  for (var i=0; i<pdg.length; i++) {
    var func = pdg[i];
    var bb = func['bb'];
    var instCount = 0;
    for (var j=0; j<bb.length; j++) {
      var inst = bb[j]['inst'];
      instCount += inst.length;
    }
    console.log(func.name + "\t" + time_info[func.name] + 
                "\t"+bb.length + "\t" + instCount + "\t" + 
                (func['pdg'].length + func['cdg'].length));
  }
});
