        // Create JoyStick object into the DIV 'joyDiv'
        
        
        var joyX = new JoyStick('joyX');
        var joyY = new JoyStick('joyY');
        var old_valueX = 0;
        var old_valueY = 0;
        setInterval(
            function(){
                valueX=joyX.GetX();
                valueY=joyY.GetY();
                if(old_valueX != valueX || old_valueY != valueY){
                    old_valueX = valueX;
                    old_valueY = valueY;
                    console.log("valueX: " + valueX);
                    console.log("valueY: " + valueY);
                    var oReq = new XMLHttpRequest();
                    oReq.open("GET", "http://" + ip + ":3000/sendData?valueX=" + valueX + "&valueY=" + valueY);
                    oReq.send();
                }
            },
            20);