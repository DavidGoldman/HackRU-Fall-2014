// Load the net module to create a tcp server.
var net = require('net');

// Creates a new TCP server. The handler argument is automatically set as a listener for the 'connection' event
var server = net.createServer(function(socket) {
  console.log("Connection from " + socket.remoteAddress);
  socket.setEncoding("ascii");

  var buffer = "";

  socket.on("data", function(data) {
  	console.log("DATA " + socket.remoteAddress + ": " + data);
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
      console.log("Line: " + lines[i]);
    }
  });

  socket.on("close", function(had_error) {
  	console.log("CLOSED: " + had_error);
  });
});

// Fire up the server bound to port 80 on localhost
server.listen(4000, "0.0.0.0");

// Put a friendly message on the terminal
console.log("TCP server listening on port 4000.");