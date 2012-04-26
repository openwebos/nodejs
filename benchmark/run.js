// Use of this file is subject to license terms as set forth in the LICENSE file available in the root directory of the project.

var launchTime = process.uptime();
var path = require("path");
var util = require("util");
var childProcess = require("child_process");
var benchmarks = [ "timers.js"
                 , "process_loop.js"
                 , "static_http_server.js"
                 ];

var benchmarkDir = path.dirname(__filename);

function exec (script, callback) {
  // webOS: Use the .uptime() method we added instead of Date()
  var start = process.uptime();
  var child = childProcess.spawn(process.argv[0], [path.join(benchmarkDir, script)]);
  child.addListener("exit", function (code) {
    var elapsed = process.uptime() - start;
    callback(elapsed, code);
  });
}

function runNext (i) {
  if (i >= benchmarks.length) return;
  util.print(benchmarks[i] + ": ");
  exec(benchmarks[i], function (elapsed, code) {
    if (code != 0) {
      console.log("ERROR  ");
    }
    console.log(elapsed);
    runNext(i+1);
  });
};
console.log("Startup in " + launchTime + " ms");
runNext(0);
