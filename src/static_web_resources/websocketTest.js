// Create a new WebSocket connection
const socket = new WebSocket("ws://127.0.0.1/");
// Event listener for when the connection is opened
socket.onopen = function(event) {
    console.log('WebSocket is open now.');
    // Send a message to the server
    socket.send('Hello Server!');
};

// Event listener for when a message is received from the server
socket.onmessage = function(event) {
    console.log('Message from server: ', event.data);
};

// Event listener for when the connection is closed
socket.onclose = function(event) {
    console.log('WebSocket is closed now.');
};

// Event listener for when an error occurs
socket.onerror = function(error) {
    console.error('WebSocket error: ', error);
};