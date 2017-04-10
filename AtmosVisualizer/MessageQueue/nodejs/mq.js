var mq = require('./build/Release/a3MQExports.node')

var result = mq.init("Whos Your Daddy", true, 100, 1024)
console.log(result);

let exit = false;

function update() {
	var result = mq.getMessage();
	//console.log(result);

	// successfully get msg
	if (result[0].success == true) {
		var msg = result[1];
		console.log(msg);

		if (msg.msgType == 'exit') {
			exit = true;
			console.log("program exited");
		}
	}
	if (!exit)
		setTimeout(update, 16.67);
}

update();