const hostname = '0.0.0.0';
const port = 3000;

//wemos_ip = '192.168.1.24';
wemos_ip = '';
const wemos_port = 4210;

const express = require('express');
var dgram = require('dgram');
const path = require('path');
var ip = require('ip');
const pug = require('pug');
const app = express();
//const router = express.Router();
var client = dgram.createSocket('udp4');

old_valueX = 511;
old_valueY = 511;

const scale = (num, in_min=-100, in_max=100, out_min=1023, out_max=0) => {
    return (num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

const scaleX = (num, in_min=-100, in_max=100, out_min=0, out_max=1023) => {
return (num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


app.set('view engine', 'pug');

app.get('/',function(req,res){
  res.render('index', {ip:ip.address()});
});

app.get('/setIp',function(req,res){
  wemos_ip = req.query.wemos_ip;
  console.log("Wemos ip: " + wemos_ip)
  res.redirect('http://' + req.headers.host)
});


//app.get('/',function(req,res){
//  res.sendFile(path.join(__dirname+'/web_joystick.html'));
//  //__dirname : It will resolve to your project folder.
//});

app.get('/joy.js',function(req,res){
    res.sendFile(path.join(__dirname+'/joy.js'));
    //__dirname : It will resolve to your project folder.
  });

  app.get('/sendData',function(req,res){

    if(wemos_ip != ''){
      var valueX = parseInt(scaleX(req.query.valueX));
      var valueY = parseInt(scale(req.query.valueY));
      if (valueX !=old_valueX){

          old_valueX = valueX;
          messageX = {"code":0,"value":valueX}
          client.send(JSON.stringify(messageX), wemos_port, wemos_ip);
          console.log("Sending data")
          return res.json({ success: 'OK' })        
          
      }else if( valueY != old_valueY){

          old_valueY = valueY;
          messageY = {"code":1,"value":valueY}
          client.send(JSON.stringify(messageY), wemos_port, wemos_ip);
          console.log("Sending data")
          return res.json({ success: 'OK' })

      }else{
          return res.json({ success: 'No data' })
      }
    }
    return res.json({success: 'KO'})
});

//add the router
//app.use('/', router);
app.listen(port, hostname, () => {
    console.log("Server running at http://" + ip.address() + ":" + port.toString());
  });

console.log('Running at Port 3000');