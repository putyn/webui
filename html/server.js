var express = require('express');
var argv = require('minimist')(process.argv.slice(2));
var app = express();

var bodyParser = require('body-parser');
app.use(bodyParser.json()); // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies


app.use(express.static(__dirname + '/dist'));
var status_code = argv['c'] ? argv['c'] : 200;
var random_wait = argv['w'] ? argv['w'] : 100;


app.get("/overview", function(req,res) {
	var  json_response = {};
	json_response["ChipID"] = "000001";
	json_response["CPU frequency"] = "80 Mhz";
	json_response["Last reset reason"] = "Power On";
	json_response["Uptime"] = "2 days";
	json_response["Memory size"] = "1024 bytes";
	json_response["Free heap"] = "2048 bytes";
	json_response["Firmware size"] = "4096 byte";
	json_response["Free firmware space"] = "8192 byte";

	//insert some random wait
	setTimeout( function () {
		//simulate .fail on front end side via http code
		res.status(status_code);
		res.json(json_response);
	}, random_wait * Math.random());

	console.log("[GET] /overview");
});


app.get("/wifi", function(req,res) {

	var json_response = {};

	//wifi status
	json_response["status"] = {"Mode": "Station", "Hostname": "esp_webui_000001", "SSID": "c86b6e2bbd8fba08421b9a710bb56756", "IP":"192.168.88.88"};
	//wifi networks
	//json_response["networks"] = [];
	json_response["networks"] = [{"ssid":"5a105e8b9d40e1329780d62ea2265d8a", "auth":1, "quality": 90}, {"ssid":"ad0234829205b9033196ba818f7a872b", "auth":1, "quality": 85}, {"ssid":"8ad8757baa8564dc136c1e07507f4a98", "auth":0, "quality": 90}];

	//insert some random wait
	setTimeout( function () {
		//simulate .fail on front end side via http code
		res.status(status_code);
		res.json(json_response);
	}, random_wait * Math.random());

	console.log("[GET] /wifi");
});

app.get("/time", function(req, res) {

	var json_response = {"time_server": "eu.ntp.pool.org", "time_zone": '+7200', "time_dst": 3600};

	//insert some random wait
	setTimeout( function () {
		//simulate .fail on front end side via http code
		res.status(status_code);
		res.json(json_response);
	}, random_wait * Math.random());

	console.log("[GET] /time");
});
/*
 * handler reboot, redirect to index
 */
app.get("/reboot", function (req,res) {
	res.redirect(301, '/');
	console.log("[GET] /reboot");
});

/*
 *	each post request should at least return
 *  error: false/true
 *  message: text - which should contain relevant information to the post request
 */

app.post("/wifi", function(req, res) {

	var ssid = req.body.ssid;
	var pass = req.body.pass;
	var json_response;

	//fail response
	if( pass.length == 0 || ssid.length == 0)
		json_response = {"error": true, "message": "Password can't be empty!"};
	else {
		//success response
		json_response = {"error": false, "message": "http://localhost:8080/#wifi"};
	}

	//insert some random wait
	setTimeout( function () {
		//simulate .fail on front end side via http code
		res.status(status_code);
		res.json(json_response);
	}, random_wait * Math.random());

	console.log("[POST] /wifi - SSID: " + ssid + ", password: " + pass);
});

app.post("/time", function(req,res) {

	//POST variables from request
	var ntp_server = req.body.time_server;
	var time_zone = req.body.time_zone;
	var time_dst = req.body.time_dst;
	var time_system_time = req.body.time_system_time
	var json_response;

	//fail response
	json_response = {"error": true, "message": "can't save config file on flash"};
	//success response, not sure if sending data back is required
	json_response = {"error": false, "message": "", "time_server": ntp_server, "time_zone": time_zone, "time_dst": time_dst};

	//insert some random wait
	setTimeout( function () {
		//simulate .fail on front end side via http code
		res.status(status_code);
		res.json(json_response);
	}, random_wait * Math.random());
	console.log("[POST] /time ntp server: "+ntp_server+", time zone: "+time_zone+", dst: "+time_dst+", system time:" + time_system_time);
});

app.get("/hw", function(req,res) {
	var json_response = {"hw_brightness": 8, "hw_acp_time":6, "hw_nightmode_start": 20, "hw_nightmode_stop":6, "hw_suppress_acp": 1,"is_nightmode": 0};
	
	//insert some random wait
	setTimeout( function () {
		//simulate .fail on front end side via http code
		res.status(status_code);
		res.json(json_response);
	}, random_wait * Math.random());
	console.log("[GET] /hw");
});

app.post("/hw", function (req, res) {
	
  var hw_brightness = req.body.hw_brightness;
  var hw_acp_time = req.body.hw_acp_time;
  var hw_nightmode_start = req.body.hw_nightmode_start;
  var hw_nightmode_stop = req.body.hw_nightmode_stop;
  var hw_suppress_acp = req.body.hw_suppress_acp;
  
	var json_response = {"error": false, "message": "", "hw_brightness": hw_brightness, "hw_acp_time" : hw_acp_time, "hw_nightmode_start": hw_nightmode_start, "hw_nightmode_stop": hw_nightmode_stop, "hw_suppress_acp" : hw_suppress_acp};
	
	//insert some random wait
	setTimeout( function () {
		//simulate .fail on front end side via http code
		res.status(status_code);
		res.json(json_response);
	}, random_wait * Math.random());
	
	console.log("[POST] /hw brightness: "+hw_brightness+", hw_acp_time: "+hw_acp_time+", hw_nightmode_start: "+hw_nightmode_start+", hw_nightmode_stop:" + hw_nightmode_stop + ", hw_suppress_acp: " + hw_suppress_acp);
});

app.listen(8080);
console.log('Application server up and running on port 8080');
