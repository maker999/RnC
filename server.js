const express = require('express');
const app = express();

const SerialPort = require("serialport");
const serialport = new SerialPort("/dev/ttyACM0", {baudRate: 9600});
const Readline = SerialPort.parsers.Readline;
const parser = serialport.pipe(new Readline({ delimiter: '\r\n' }));

var received = new String;
var reqFlag = 0;

app.set('view engine', 'ejs');

app.get(['/', '/index.html'], function(req, res){
    res.render('index.ejs',{mnem: "mnom here", seed:"76896f8ds97gd9sg7s", ncc: "node chainz", npub: "666", mprk: "555"});
});

app.get(['/requestkeys','index.html'], function(req,res){
    serialport.write("2");
    parser.on('data', function(data){
      var str = data.toString('utf8');
      var s = str.indexOf("seed:"); //5
      var nc = str.indexOf("Node Chain:"); //11
      var npk = str.indexOf("Node Pub Key:"); //13
      var mpk = str.indexOf("Mainnet Private Key:"); //20
      console.log(data);
      res.render('index.ejs',{mnem: str.substring(0,s), seed: str.substring(s+5,s+133), ncc: str.substring(nc+11,nc+75), npub: str.substring(npk+13, npk+79), mprk: str.substring(mpk+20)});
    });
});

/*
app.get('/css/main.css', function(req, res){
  res.sendFile((__dirname + '/public/css/main.css'));
});

app.get('/css/main.js', function(req, res){
  res.sendFile((__dirname + '/public/css/main.js'));
 */

app.listen(8080, '127.0.0.1', function(){
  console.log('Example app listening on port 8080')
})
