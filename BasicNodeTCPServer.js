// Load the net module to create a tcp server.
var net = require('net');

// Creates a new TCP server. The handler argument is automatically set as a listener for the 'connection' event
var server = net.createServer(function(socket) {
  console.log("Connection from " + socket.remoteAddress);

  socket.on("data", function(data) {
  	console.log("DATA " + sock.remoteAddress + ": " + data);
  });

  sock.on("close", function(data) {
  	console.log("CLOSED: " + sock.remoteAddress + " " + sock.remotePort);
  });
});

// Fire up the server bound to port 7000 on localhost
server.listen(7000, "localhost");

// Put a friendly message on the terminal
console.log("TCP server listening on port 7000 at localhost.");