// io is imported via <script> tag in index.html
// furhtermore path to socket.io is linked via a symlink to static/include folder
// which allowed this simple project to be done without need for additional 
// module handling dependencies  
export const socket = io('your-ip-address');
socket.on('connect', function () {
  socket.emit('message', 'I\'m connected!');
  console.log("Client side socketio connection established");
});
socket.on("my response", function (data) {
  console.log("Recieved message: " + data['data']);
});