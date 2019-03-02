const PORT = 3484;

var http = require('http')
var socketio = require('socket.io')

var ip = require('ip');
var app = http.createServer();
var io = socketio(app);

app.listen(PORT);
console.log("Server nodejs: " + ip.address() + ":" + PORT)

function getRandomInt(max) {
  return Math.floor(Math.random() * Math.floor(max));
}

io.on('connection', function(socket) {	
	
    console.log("connected");
	

	socket.on('repeat', function(data) {
		
		console.log(data);
		
		setTimeout(function on() {
		console.log("on");
		socket.emit("led", { status: 'on' });
		}, 2000);
		
		setTimeout(function off() {
			console.log("off");
			socket.emit("led", { status: 'off' });
		}, 4000);
		
	});

	socket.on('disconnect', function() {
		console.log("disconnect")
	})
});