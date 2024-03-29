const PORT = 3484;

var http = require('http')
var socketio = require('socket.io')

var ip = require('ip');
var app = http.createServer();
var io = socketio(app);

app.listen(PORT);
console.log("Server nodejs: " + ip.address() + ":" + PORT)

var count = 0;

io.on('connection', function(socket) {
    console.log("connected");
	
	socket.on("event", function(data) {
		console.log(data);
	});
	
	socket.on("message", function(data) {
		console.log(data);
	});
	
	socket.emit("event", { hello: "socketIo client" });
	
	socket.on('disconnect', function() {
		console.log("disconnect")
	})
});