function sendPassword(cb) {
	console.log("sendPassword");
	const password = document.getElementById('userPassword').value;
	if (socket.readyState === WebSocket.OPEN) {
		pw_token = "PW-" + password;
		socket.send(pw_token);
	} else {
		console.error(
			"WebSocket is not open. Ready state is:",
			socket.readyState
		);
	}
}

function handleClick(cb) {
	const DHCPCheckbox = document.getElementById("DHCP");
	IP4Addr = document.getElementById("IP4Address");
	NetMask = document.getElementById("NetMask");

	if(DHCPCheckbox.checked) {
		IP4Addr.disabled = true;
		NetMask.disabled = true;
		IP4Addr.style.backgroundColor = "#CCCCCC";
		NetMask.style.backgroundColor = "#CCCCCC";
	}
	else {
		IP4Addr.disabled = false;
		NetMask.disabled = false;
		IP4Addr.style.backgroundColor = "white";
		NetMask.style.backgroundColor = "white";
	}
}

//const socket = new WebSocket("ws://192.0.2.1/");
const socket = new WebSocket("ws://127.0.0.1/ws_echo");

socket.onopen = function (event) {
	updateConnectionStatus(true);
};

socket.onclose = function (event) {
	updateConnectionStatus(false);
};

socket.onerror = function (error) {
	updateConnectionStatus(false);
};

socket.onmessage = function (event) {
	handleMessage(event.data);
};

socket.addEventListener("message", (event) => {
	console.log("Message from server: ", event.data);
});

function sendValues() {

	const can1Checkboxes = document.querySelectorAll('input[id^="isEnabled_CAN_1"]');
	const can2Checkboxes = document.querySelectorAll('input[id^="isEnabled_CAN_2"]');

	const DHCPCheckbox = document.getElementById("DHCP");
	const IP4AdressTag = document.getElementById("IP4Address");
	const NetMaskTag = document.getElementById("NetMask")

	const valList = [DHCPCheckbox,
		IP4AdressTag, NetMaskTag];

	const JSMap = new Map();

	valList.forEach(element => {
		console.log(element.id, element.value);
		JSMap.set(element.id, element.value);
	})

	can1Checkboxes.forEach(element => {
		if(element.checked)
		{
			JSMap.set(element.id, 1);
		}
		else
		{
			JSMap.set(element.id, 0);
		}
	})

	can2Checkboxes.forEach(element => {
		if(element.checked)
		{
			JSMap.set(element.id, 1);
		}
		else
		{
			JSMap.set(element.id, 0);
		}
	})

	if (socket.readyState === WebSocket.OPEN) {
		const obj = Object.fromEntries(JSMap);
		const res = JSON.stringify(obj);
		socket.send(res);
	} else {
		console.error(
			"WebSocket is not open. Ready state is:",
			socket.readyState
		);
	}
}

function fetchValues() {
	if (socket.readyState === WebSocket.OPEN) {
		socket.send(JSON.stringify({ action: "fetch" }));
	} else {
		console.error(
			"WebSocket is not open. Ready state is:",
			socket.readyState
		);
	}
}

function handleMessage(data) {
	if (data === "send successful\u0000") {
		const sendButton = document.getElementById("sendButton");
		sendButton.style.backgroundColor = "green";
		setTimeout(() => {
			sendButton.style.backgroundColor = "";
		}, 3000);
	}
	else if(data === "PW correct\u0000") {
		const body = document.getElementById("pwItems");
		body.style.visibility = "visible";
	}
	else if(data === "PW false\u0000") {
		const body = document.getElementById("pwItems");
		body.style.visibility = "hidden";
	}
}

function updateConnectionStatus(isConnected) {
	const led = document.getElementById("connectionStatus");
	const text = document.getElementById("connectionText");

	if (isConnected) {
		led.classList.remove("off");
		led.classList.add("on");
		text.textContent = "Connected";
	} else {
		led.classList.remove("on");
		led.classList.add("off");
		text.textContent = "Disconnected";
	}
}