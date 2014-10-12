// Load the net module to create a tcp server.
var net = require('net');

// Creates a new TCP server. The handler argument is automatically set as a listener for the 'connection' event
var server = net.createServer(function(socket) {
  console.log("Connection from " + socket.remoteAddress);
  socket.setEncoding("ascii");

  socket.on("data", function(data) {
  	console.log("DATA " + socket.remoteAddress + ": " + data);
  });

  socket.on("close", function(had_error) {
  	console.log("CLOSED: " + had_error);
  });
});

// Fire up the server bound to port 80 on localhost
server.listen(4000, "0.0.0.0");

// Put a friendly message on the terminal
console.log("TCP server listening on port 4000.");