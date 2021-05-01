'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');
const mysql = require('mysql2/promise');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { json } = require('express');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  	res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
	//Feel free to change the contents of style.css to prettify your Web app
	res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
	fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
		const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
		res.contentType('application/javascript');
		res.send(minimizedContents._obfuscatedCode);
	});
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {

	if(!req.files) {
		return res.status(400).send('No files were uploaded.');
	}
	
	let uploadFile = req.files.uploadFile;
	
	// Use the mv() method to place the file somewhere on your server
	uploadFile.mv('uploads/' + uploadFile.name, function(err) {
		if(err) {
		return res.status(500).send(err);
		}

		res.redirect('/');
	});
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){

	fs.stat('uploads/' + req.params.name, function(err, stat) {
		if(err == null) {
		res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
		} else {
		console.log('Error in file downloading route: '+err);
		res.send('');
		}
	});
});

//******************** Your code goes here ******************** 

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

let sharedLib = ffi.Library('./libgpxparser', {
	'addToFileLog': ['string', ['string']],
	'addToRouteDisplay': ['string', ['string']],
	'addToTrackDisplay': ['string', ['string']],
	'renameSelectedRoute': ['void', ['string', 'string', 'int']],
	'renameSelectedTrack': ['void', ['string', 'string', 'int']],
	'displayRoutesBetween': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
	'displayTracksBetween': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
	'createGPXFromInput': ['string', ['string', 'float', 'string']],
	'getOtherDataTrack': ['string', ['string', 'int']],
	'getOtherDataRoute': ['string', ['string', 'int']],
});

//File Log Endpoint
app.get('/filelog', function(req , res){

	var fileInfo = [];
		
	let files = fs.readdirSync('./uploads');

	for(var i = 0; i < files.length; i++){

		let jsonStr = sharedLib.addToFileLog(files[i]);
		
		if(jsonStr == "Invalid"){
		fileInfo[i] = JSON.stringify(jsonStr);
		continue;
		}

		let str = JSON.parse(jsonStr);
		str["fileName"] = files[i];
		fileInfo[i] = JSON.stringify(str);
	}

	res.send(fileInfo);

});

// Route display endpoint
app.get('/routeDisplay/:fileName', function(req , res){

	let fileName = req.params.fileName;
	let jsonStr = sharedLib.addToRouteDisplay(fileName);

	let str = JSON.parse(jsonStr);
	let fileInfo = JSON.stringify(str);
	
	res.send(fileInfo);

});

// Track display endpoint
app.get('/trackDisplay/:fileName', function(req , res){

	let fileName = req.params.fileName;
	let jsonStr = sharedLib.addToTrackDisplay(fileName);

	let str = JSON.parse(jsonStr);
	let fileInfo = JSON.stringify(str);
	
	res.send(fileInfo);

});

// Track Data display endpoint
app.get('/trackDisplayData/', function(req , res){

	let index = req.query.index;
	let fileName = req.query.fileName;
	let jsonStr = sharedLib.getOtherDataTrack(fileName, index);

	let fileInfo = JSON.stringify(jsonStr);
	
	res.send(fileInfo);
});

// Route Data display endpoint
app.get('/routeDisplayData/', function(req , res){

	let index = req.query.index;
	let fileName = req.query.fileName;
	let jsonStr = sharedLib.getOtherDataRoute(fileName, index);
	
	let info = JSON.stringify(jsonStr);

	res.send(info);

});

// Rename route endpoint
app.get('/renameRoute', function(req, res){

	// Gets the parameter values
	let fileName = req.query.fileName;
	let name = req.query.name;
	let index = req.query.index;

	// Renames the route
	sharedLib.renameSelectedRoute(fileName, name, index);

});

// Rename track endpoint
app.get('/renameTrack', function(req, res){

	// Gets the parameter values
	let fileName = req.query.fileName;
	let name = req.query.name;
	let index = req.query.index;

	// Renames the track
	sharedLib.renameSelectedTrack(fileName, name, index);

});

// Create new GPX file endpoint
app.get('/createGPX/', function(req, res){

	let fileName = req.query.fileName;
	let version = req.query.version;
	let creator = req.query.creator;

	let info = sharedLib.createGPXFromInput(fileName, version, creator);

	let jsonStr = sharedLib.addToFileLog(fileName);

	res.send(info);

});

// Find routes between coordinates endpoint
app.get('/routesBetween', function(req , res){

	var fileInfo = [];
		let files = fs.readdirSync('./uploads');

	for(var i = 0; i < files.length; i++){
		let jsonStr = sharedLib.displayRoutesBetween(files[i], req.query.latitudeStart, req.query.longitudeStart, req.latitudeEnd, req.longitudeEnd, req.comp);
		let str = JSON.parse(jsonStr);
		fileInfo[i] = JSON.stringify(str);
	}

	res.send(fileInfo);

});

// Find tracks between coordinates endpoint
app.get('/tracksBetween', function(req , res){

	var fileInfo = [];
		let files = fs.readdirSync('./uploads');

	for(var i = 0; i < files.length; i++){
		let jsonStr = sharedLib.displayTracksBetween(files[i], req.query.latitudeStart, req.query.longitudeStart, req.latitudeEnd, req.longitudeEnd, req.comp);
		let str = JSON.parse(jsonStr);
		fileInfo[i] = JSON.stringify(str);
	}

	res.send(fileInfo);

});


/********************** Final Assignment Code ***************************/

// When the program excutes, these tables are created in the database

let connection;

let file = `CREATE TABLE IF NOT EXISTS FILE (

	gpx_id INT AUTO_INCREMENT PRIMARY KEY,
	file_name VARCHAR(60) NOT NULL,
	ver DECIMAL(2,1) NOT NULL,
	creator VARCHAR(256) NOT NULL)`;

let route = `CREATE TABLE IF NOT EXISTS ROUTE (

	route_id INT AUTO_INCREMENT PRIMARY KEY,
	route_name VARCHAR(256),
	route_len FLOAT(15,7) NOT NULL,
	gpx_id INT NOT NULL,
	FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)`;

let point = `CREATE TABLE IF NOT EXISTS POINT (

	point_id INT AUTO_INCREMENT PRIMARY KEY,
	point_index INT NOT NULL,
	latitude DECIMAL(11,7) NOT NULL,
	longitude DECIMAL(11,7) NOT NULL,
	point_name VARCHAR(256),
	route_id INT NOT NULL,
	FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)`;

/***************************** Login Functionality *************************************/
app.get('/login', async function(req, res){

	var message;

	let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : req.query.username,
	password : req.query.password,
	database : req.query.database
	}

	try{

		// create the connection
		connection = await mysql.createConnection(dbConf);

		//Populate the table
		await connection.execute(file);
		await connection.execute(route);
		await connection.execute(point);

		message = "Connected";

	}catch(e){

		console.log("Query error: "+ e);
		message = "Disconnected";

	}finally {

		if (connection && connection.end) connection.end();

	}

	res.send(JSON.stringify(message));
});

/***************************** Store All Files Functionality *************************************/
app.get('/storeAllFiles', async function(req, res){

	let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : req.query.username,
	password : req.query.password,
	database : req.query.database
	}

	let files = fs.readdirSync('./uploads');

	try{

		// create the connection
		connection = await mysql.createConnection(dbConf);

		for(var i = 0; i < files.length; i++){

			/* Adds to FILE table */
			let jsonStr = sharedLib.addToFileLog(files[i]);
			
			if(jsonStr == "Invalid"){
				fileInfo[i] = JSON.stringify(jsonStr);
				continue;
			}
	
			let str = JSON.parse(jsonStr);
			str["fileName"] = files[i];

			let [check, details] = await connection.execute('SELECT EXISTS (SELECT * from FILE WHERE file_name=\'' + str["fileName"] + '\') AS status;');

			// Checks that file is not already in the database
			if (check[0].status === 0){
				let fileSql = 'INSERT IGNORE INTO FILE (file_name, ver, creator) VALUES (\'' + str["fileName"] + '\', \'' + str["version"] + '\', \'' + str["creator"] + '\');';
				await connection.execute(fileSql);
			}

			/* Adds to ROUTE table */
			let jsonStrRoute = sharedLib.addToRouteDisplay(files[i]);

			let strRoute = JSON.parse(jsonStrRoute);

			for(var j = 0; j < strRoute.length; j++){

				let [rows, info] = await connection.execute('SELECT (SELECT gpx_id FROM FILE WHERE file_name=\''+ files[i] + '\') AS name;');
				let [check, details] = await connection.execute('SELECT EXISTS (SELECT * from ROUTE WHERE route_name=\'' + strRoute[j]["name"] + '\') AS status;');

				if (check[0].status === 0){
					let routeSql = 'INSERT IGNORE INTO ROUTE (route_name, route_len, gpx_id) VALUES (\'' + strRoute[j]["name"] + '\', \'' + strRoute[j]["len"] + '\', \'' + rows[0].name + '\');';
					await connection.execute(routeSql);
				}
			}

			/* Adds to POINT table */
			/*let jsonStrPoint = sharedLib.addToPointsDisplay(files[i]);

			let strPoint = JSON.parse(jsonStrPoint);

			for(var j = 0; j < strRoute.length; j++){

				let [rows, info] = await connection.execute('SELECT (SELECT route_id FROM ROUTE WHERE route_name=\''+ strPoint[j]["route_name"] + '\') AS name;');
				

				for(var k = 0; k < strPoint[j][""]; k++){
					let pointSql = 'INSERT IGNORE INTO POINT (point_index, longitude, latitude, point_name, route_id) VALUES (\'' + strPoint[j]["name"] + '\', \'' + strRoute[j]["len"] + '\', \'' + rows[0].name + '\');';
					await connection.execute(pointSql);
				}
			}
			*/
		}


	}catch(e){

		console.log("Query error: "+ e);

	}finally {

		if (connection && connection.end) connection.end();

	}

	res.send(JSON.stringify("Stored"));
});

/***************************** Track Route Updates Functionality *************************************/
app.get('/trackRouteUpdates', async function(req, res){

	let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : req.query.username,
	password : req.query.password,
	database : req.query.database
	}

	try{

		// create the connection
		connection = await mysql.createConnection(dbConf);


	}catch(e){

		console.log("Query error: "+ e);

	}finally {

		if (connection && connection.end) connection.end();

	}

});

/***************************** Clear data functionality *************************************/
app.get('/clearData', async function(req, res){

	let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : req.query.username,
	password : req.query.password,
	database : req.query.database
	}

	try{

		// create the connection
		connection = await mysql.createConnection(dbConf);

		// Delete data from all tables in the database
		await connection.execute("DELETE FROM POINT");
		await connection.execute("DELETE FROM ROUTE");
		await connection.execute("DELETE FROM FILE");

		res.send(JSON.stringify("Cleared"));

	}catch(e){

		console.log("Query error: "+ e);
		res.send(JSON.stringify("Query error: "+ e));

	}finally {

		if (connection && connection.end) connection.end();

	}
});


/***************************** Update database status functionality *************************************/
app.get('/updateStatus', async function(req, res){

	let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : req.query.username,
	password : req.query.password,
	database : req.query.database
	}

	var numbersToSend= [];

	try{

		// create the connection
		connection = await mysql.createConnection(dbConf);

		// Gets row count for FILE table
		let [fileRows, info] = await connection.execute("SELECT (SELECT COUNT(*) FROM FILE) AS fileCount;");

		for(var fileRow of fileRows){
			numbersToSend[0] = fileRow.fileCount;
		}

		// Gets row count for ROUTE table
		let [routeRows, info2] = await connection.execute("SELECT (SELECT COUNT(*) FROM ROUTE) AS routeCount;");

		for(var routeRow of routeRows){
			numbersToSend[1] = routeRow.routeCount;
		}

		// Gets row count for POINT table
		let [pointRows, info3] = await connection.execute("SELECT (SELECT COUNT(*) FROM POINT) AS pointCount;");

		for(var pointRow of pointRows){
			numbersToSend[2] = pointRow.pointCount;
		}

	}catch(e){

		console.log("Query error: "+ e);

	}finally {

		if (connection && connection.end) connection.end();

	}

	res.send(numbersToSend);
});



/*********************************** QUERIES *****************************************/

/***************************** Query 1 *************************************/
app.get('/query1', async function(req, res){

	let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : req.query.username,
	password : req.query.password,
	database : req.query.database
	}

	let sendBack = [];

	let sortBy = 'SELECT * from ROUTE ORDER BY' + '`'+ req.query.sort +'`;';

	try{

		// create the connection
		connection = await mysql.createConnection(dbConf);
		let [rows, info] = await connection.execute(sortBy);
		let i = 0;

		// Add route name, route length, and route id to response
		for(let row of rows){

			sendBack[i] = {"name": row.route_name, "len": row.route_len, "id": row.route_id};
			i++;
		}

	}catch(e){

		console.log("Query error: "+ e);

	}finally {

		if (connection && connection.end) connection.end();

	}

	res.send(sendBack);

});

/***************************** Query 2 *************************************/
app.get('/query2', async function(req, res){

	let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : req.query.username,
	password : req.query.password,
	database : req.query.database
	}

	let sendBack = [];

	let sortBy = 'SELECT * from ROUTE WHERE gpx_id=(SELECT gpx_id FROM FILE WHERE file_name=\'' + req.query.fileName +'\''+') ORDER BY' + '`'+ req.query.sort +'`;';

	try{

		// create the connection
		connection = await mysql.createConnection(dbConf);
		let [rows, info] = await connection.execute(sortBy);
		let i = 0;

		// Add route name, route length, and route id to response
		for(let row of rows){

			sendBack[i] = {"name": row.route_name, "len": row.route_len, "id": row.route_id};
			i++;
		}

	}catch(e){

		console.log("Query error: "+ e);

	}finally {

		if (connection && connection.end) connection.end();

	}

	res.send(sendBack);

});