'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

var sharedLib = ffi.Library('./libgpxparser.so', {
  'GPXTableInfo':['string',['string','string']],
  'addingValid':['int',['string','string']],
  'listTrackJSON':['string',['string']],
  'listRouteJSON':['string',['string']]
});

var schema = "gpx.xsd"; 
var fileDir = "./uploads/";

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`
const portNum = process.argv[2];

// Send HTML at root
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style
//need to restyle css to make more pretty on frontend
app.get('/style.css',function(req,res){
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

/**upload checks for duplicates, my ajax function checks for validation */
//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;

  
  var checkFiles = fs.readdirSync (fileDir); 

  for (let i = 0; i < checkFiles.length; i++){ //if file already exists in folder, return error and delete file 
    if (uploadFile.name.localeCompare(checkFiles[i]) == 0){
      return res.status(400).send('DUplicate File Not Uploaded'); 
    }
  }

  // Use the mv() method to place the file in uploads directory on server-side
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }
    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ director
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) { //willl have to pass in additional parameter as object argumenets 
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name)); //trys to download file from server to local disk 
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

/**
 * only test for validity when creating or modifying a GPX file 
 * otherwise, just use "createGPX() from C lib to convert gpx file to gpx structure for internal use"
 */

//TODO: rename endpoints to represent functionality

var foundFiles = null; 

//endpoint3
app.get ('/endpoint3', function (req, res){
    foundFiles =fs.readdirSync (fileDir);
    let numVal = foundFiles.length; 
    let strVal = numVal.toString(); 
    res.send(strVal); 
});

//accept index from caller, return a GSON string that can fill the table of files in GUI 
app.get('/endpoint2', function (req, res){
    foundFiles =fs.readdirSync (fileDir);

    if (sharedLib.addingValid((fileDir + foundFiles[req.query.dataIdx]), schema) == 0){
      console.log (foundFiles[req.query.dataIdx] + "is not a valid GPX file. It has been deleted"); 
      fs.unlinkSync(fileDir + foundFiles[req.query.dataIdx]); 
      res.send (NULL);
    }

    let tmp = sharedLib.GPXTableInfo(foundFiles[req.query.dataIdx] ,schema);

    if (tmp == null){
      console.log (foundFiles[req.query.dataIdx] + "is not a valid GPX file. It has been deleeted"); //pass in current file name from where? 
      fs.unlinkSync(fileDir + foundFiles[req.query.dataIdx]); 
      res.send (NULL); //i.e. check for NULL on client side 
    }
    else {
      console.log (foundFiles[req.query.dataIdx] + "is a valid GPX file, following GSON ADDED:"); 
      console.log (tmp); 
      res.send (tmp); 
    }

});

//receive file name from client, pass back two strings with track/route lists -> maybe just one actually 
app.get ('/emdPoint5', function(req, res){
  var returnStr = sharedLib.listRouteJSON (req.query.dataIdx);

  if (returnStr == NULL){
    console.log ("Failed to Convert Track Lists"); 
  }
  else {
    res.send (returnStr); 
  }
  
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);