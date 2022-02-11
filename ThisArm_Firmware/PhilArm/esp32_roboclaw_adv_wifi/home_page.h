#ifndef _HOME_PAGE
#define _HOME_PAGE

const char home_page[] = R"=====(
<!DOCTYPE html>
<html>
<head>
<script>

  function init() 
  {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    Socket.onmessage = function(event) { processReceivedCommand(event); };
    console.log( "websocket setup");
  }
  
  
  function processReceivedCommand(evt) 
  {
    console.log("Got websocket message");
    console.log(evt);
    console.log(evt.data);
    
    //document.getElementById('rd').innerHTML = evt.data;
 
  }
   window.onload = function(e)
  { 
    init();
  }

  function sendCommand(chan,state) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
 
      }
    };
    xhttp.open("GET", "handleMotor?motorChannel="+chan+"&motorState="+state, true);
    xhttp.send();
  }

  setInterval(function(){
   getEnc_1();
  }, 1000);

  setInterval(function(){
   getEnc_2();
  }, 1000);

   setInterval(function(){
   getTemp();
  }, 5000);

   setInterval(function(){
   getVoltage();
  }, 2000);

  function getEnc_1() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if(this.readyState == 4 && this.status == 200){
        document.getElementById("encoder_1_value").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET","readEncoder_1",true);
    xhttp.send();
  }

  function getEnc_2() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if(this.readyState == 4 && this.status == 200){
        document.getElementById("encoder_2_value").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET","readEncoder_2",true);
    xhttp.send();
  }

  function getTemp() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if(this.readyState == 4 && this.status == 200){
        document.getElementById("temp").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET","readTemp",true);
    xhttp.send();
  }

  function getVoltage() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if(this.readyState == 4 && this.status == 200){
        document.getElementById("voltage").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET","readVoltage",true);
    xhttp.send();
  }
  // new stuff follows
    
  function getFormStr(){
    form = document.getElementById('form1').elements;
    var res = "{";
    for(var i = 0; i < form.length; i++){
      console.log(form[i]);
    
      if(form[i].type == 'text'){
        if(i > 0) {
          res += ",";
        }
        console.log(form[i].name + ": " + form[i].value);
        res += "\""+form[i].name + "\":\"" + form[i].value + "\"";
      }
    }
    res += "}";
    console.log(res);
    return res;
  }

  function getFormData(){
    var str = getFormStr();
    
    Socket.send(str);
  }

  function setTd (t,n,d){
      td0 = document.createElement('td');
      td1 = document.createElement('td');
      tr = document.createElement('tr');
      td0.setAttribute("bgcolor","#FF0000");
      td1.setAttribute("bgcolor","#8080FF");

      console.log(tr);
      console.log(td0);
      td0.appendChild(document.createTextNode(n));
      td1.appendChild(document.createTextNode(d));
      tr.appendChild(td0);
      tr.appendChild(td1);
      t.appendChild(tr);
    
  }
  
  function setFd (t,n,d){
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute("bgcolor","#FF8040");
    td.appendChild(document.createTextNode(n));
    tr.appendChild(td);
    td = document.createElement('td');
    td.setAttribute("bgcolor","#4080FF");
  
    //t.appendChild(document.createTextNode(n));
    fd = document.createElement('input');
    fd.setAttribute("bgcolor","#FF8000");
    fd.setAttribute("type","text");
    fd.setAttribute("name",n);
    fd.setAttribute("value",d);
    console.log(fd);  
    td.appendChild(fd);
    tr.appendChild(td);
    t.appendChild(tr);
    
  }

  function setTable() {
    var table1 = document.createElement("table");
  
    inData.forEach((v,i) => {
      setTd (table1,v[0],v[1]);
    });
    oldfoo = document.getElementById('table1');
    if(!oldfoo) {
       foo = document.getElementsByTagName('body')[0].appendChild(table1);// replaceChild(table1, oldtable)
       inData = [['hostname','esp1'],['ssid','Butter2.4'],['user','myname'], ['newdata','newstuff1']];
    } else { 
       foo = document.getElementsByTagName('body')[0].replaceChild(table1, oldfoo);// replaceChild(table1, oldtable)
       inData = [['hostname','esp1'],['ssid','Butter2.4'],['user','myname'], ['newdata','newstuff2']];
    }
    table1.setAttribute('id', 'table1');
  }

</script>

<style>
      .round {
        border-radius: 25px;
        background-color: #cde6f5;
        width: 25%;
        height: 25%;
        margin: 20px;
        padding: 20px;
      }

      h2 {
        color: #707078;
      }

      .button {
        background-color: #87919e;
        display: inline-block;
        border-radius: 10px;
        border: none;
        color: white;
        font-size: 15px;
        padding: 25px;
      }

      span {
        color: #707078;
      }

      p {
        color: #707078;
        font-size: 25px;
      }
    </style>
</head
<body>

    <div class="round">
      <h2>Motor Channel 1</h2>
      <button type="button" class="button" onclick="sendCommand('1','1')">Motor On</button>
        <button type="button" class="button" onclick="sendCommand('1','0')">Motor Off</button>
        <br />
        <br />
        <p>Encoder Count: <span id="encoder_1_value">0</span></p>
    </div>

    <div class="round">
      <h2>Motor Channel 2</h2>
      <button type="button" class="button" onclick="sendCommand('2','1')">Motor On</button>
        <button type="button" class="button" onclick="sendCommand('2','0')">Motor Off</button>
        <br />
        <br />
        <p>Encoder Count: <span id="encoder_2_value">0</span></p>
    </div>

    <div class="round">
      <h2>Info</h2>
        <p>Voltage: <span id="voltage">0</span>V</p>
        <p>Temperature: <span id="temp">0</span>C</p>
    </div>
    
</body>
</html>

)=====";

#endif
