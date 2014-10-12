// Load the net module to create a TCP server.
var net = require('net');
var exec = require('child_process').execFile;

function spawnChild(type) {
  return exec("hello-myo-VisualStudio2013.exe", [type], function(error, stdout, stderr) {  
        console.log(err);
        console.log(stdout.toString());
        console.log(stderr.toString());                 
  });
}

function endChild(child) {
  child.stdin.write('e\n');
}

// Creates a new TCP server. The handler argument is automatically set as a listener for the 'connection' event.
var server = net.createServer(function(socket) {
  console.log("Connection from " + socket.remoteAddress);
  socket.setEncoding("ascii");

  var buffer = "";
  var ignoreFirst = true;
  var childProcess = null;

  socket.on("data", function(data) {
    // Buffer the data.
    data = data.replace("\r\n", "\n");
    buffer += data;
    var lines = buffer.split("\n");
    buffer = "";

    // Put the last line back in the buffer if it was incomplete.
    if (lines[lines.length - 1] !== '') {
      buffer = lines[lines.length - 1];
    }

    // Remove the final \n or incomplete line from the array.
    lines = lines.splice(0, lines.length - 1);
    for (var i = 0; i < lines.length; ++i) {
      if (ignoreFirst) {
        ignoreFirst = false;
      } else {
        console.log(socket.remoteAddress + ": " + lines[i]);

        // Start or end process.
        if (lines[i] == 'HammerStart') {
          childProcess = spawnChild("Hammers");
        } else if (lines[i] == 'DumbbellStart') {
          childProcess = spawnChild("Dumbbells");
        } else {
          endChild(childProcess);
        }
      }
    }
  });

  socket.on("close", function(had_error) {
    console.log("CLOSED: " + had_error);
  });
});

// Fire up the server bound to port 4000 on localhost.
server.listen(4000, "0.0.0.0");
console.log("TCP server listening on port 4000.");