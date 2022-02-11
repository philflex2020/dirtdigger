#ifndef _HOME_PAGE2
#define _HOME_PAGE2

const char home_page2[] = R"=====(
<!DOCTYPE html>
<html>
<head>
<script>

  var Socket;
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
    
    document.getElementById('rd').innerHTML = evt.data;
    //if (evt.data ==='0') 
    //{  
    //    document.getElementById('BTN_LED').innerHTML = 'Turn on the LED';  
    //    document.getElementById('LED_status').innerHTML = 'LED is off';  
    //}
    //if (evt.data ==='1') 
    //{  
    //    document.getElementById('BTN_LED').innerHTML = 'Turn off the LED'; 
    //    document.getElementById('LED_status').innerHTML = 'LED is on';   
    //}
  }

  var inData = [['hostname','esp1'],['ssid','Butter2.4'],['user','myname']];
  var jData = '{"jhost":"esp1","jssid":"Butter2.4","juser":"myname"}';
  
  function dummy(tr) {

    tr.Attributes.Add("background-color","#FFFF00");
    tr.Attribues.Add( "border-bottom", "black thin solid");

  }
  
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

  function setJTable(jD) {
    var jq = JSON.parse(jD); //(jData);
    console.log(" running setJTable");
    console.log(jq);
    
    var table1 = document.createElement("table");
    for (x in jq) {
      console.log(x);
      console.log(jq[x]);
      setTd (table1,x,jq[x])
    }
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
  
  function setJForm(jD) {
    var jq = JSON.parse(jD); //(jData);
    console.log(" running setJForm");
    console.log(jq);
    
    var table1 = document.createElement("table");
    
    var form1 = document.createElement("form");
    form1.setAttribute ('method','post');
    form1.setAttribute ('action','config');
    
    for (x in jq) {
      console.log(x);
      console.log(jq[x]);
      setFd (table1,x,jq[x])
    }
    s = document.createElement("input");
    s.setAttribute ('type','submit');
    s.setAttribute ('value','Submit');
    form1.appendChild(table1);
    form1.appendChild(s);

    console.log(" Here is the form:");
    console.log(form1);
    
    oldfoo = document.getElementById('form1');
    if(!oldfoo) {
       foo = document.getElementsByTagName('body')[0].appendChild(form1);// replaceChild(table1, oldtable)
       //inData = [['hostname','esp1'],['ssid','Butter2.4'],['user','myname'], ['newdata','newstuff1']];
    } else { 
       foo = document.getElementsByTagName('body')[0].replaceChild(form1, oldfoo);// replaceChild(table1, oldtable)
       //inData = [['hostname','esp1'],['ssid','Butter2.4'],['user','myname'], ['newdata','newstuff2']];
    }
    form1.setAttribute('id', 'form1');
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

  //setInterval(function(){
   //getEnc_2();
  //}, 1000);

  setInterval(function(){
   getTemp();
  }, 5000);

  setInterval(function(){
   getVoltage();
  }, 2000);

  function myAnin (ip,op) {
    var val = document.getElementById(ip).value
    document.getElementById(op).innerHTML =  val
  }
  
  function myPage (npage) {
    location.replace("http://" + location.hostname + "/" + npage)
  }
  
  function myButton (action) {
    if (action == "stop") {
      sendCommand('1','0');
    }
    if (action == "fwd") {
      sendCommand('1','1');
    }
    if (action == "back") {
      sendCommand('1','-1');
    }
    //location.replace("http://" + location.hostname + "/" + npage)
  }

  function myUpdate() {
    location.replace("http://" + location.hostname + ":8080/update")
  }
  
  function getConfig() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if(this.readyState == 4 && this.status == 200){
        document.getElementById("cfg").innerHTML = this.responseText;
        setJForm(this.responseText);
      }
    };
    xhttp.open("GET","config",true);
    xhttp.send();
  }

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

  //function getEnc_2() {
   // var xhttp = new XMLHttpRequest();
  //  xhttp.onreadystatechange = function() {
   //   if(this.readyState == 4 && this.status == 200){
  //      document.getElementById("encoder_2_value").innerHTML = this.responseText;
  //    }
  //  };
  //  xhttp.open("GET","readEncoder_2",true);
  //  xhttp.send();
  //}

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
  
  function sendText(data)
  {
    Socket.send(data);
  }
 
  window.onload = function(e)
  { 
    init();
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
      
      .range {
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
      table {
        border-collapse:collapse;
      }
      table, th, td {
         border: solid 1px black;
      }
    </style>
</head
<body>
    <p>Recieved data = <span id='rd'>---</span> </p>
    <p>config = <span id='cfg'>---</span> </p>
    <h2> Menu </h2>
    <table>
     <tr>
      <td><button onclick="myPage('2')">Main</button> <br /></td>
      <td><button onclick="getConfig()">Config</button> <br /></td>
      <td><button onclick="getFormData()">SendConfig</button> <br /></td>
      <td><button onclick="myPage('info')">Info</button> <br /></td>
      <td><button onclick="myPage('status')">Status</button> <br /></td>
      <td><button onclick="setTable()">Table</button> <br /></td>
      <td><button onclick="setJTable(jData)">JTable</button> <br /></td>
      <td><button onclick="myUpdate()">Update</button> <br /></td>
     </tr>
    </table>
    <div class="round">
      <h2>Motor Control</h2>
      <button type="button" class="button" onclick="sendCommand('1','1')">Motor On</button>
        <button type="button" class="button" onclick="sendCommand('1','0')">Motor Off</button>
        <br />
        <br />
        <p>Encoder Count: <span id="encoder_1_value">0</span></p>
    </div>

    <h2>System Controls</h2>
    <table>
     <tr>
       <td><label>Analog input 1:</label></td>
       <td><input type="range" class="range2" id="ain1" name="ain1" min="0" max="1024" oninput="myAnin('ain1','aout1')"></td>
       <td><button type="button" class="button2" onclick="sendCommand('1','0')">Set</button> </td>
       <td><output id="aout1"></output></td>
       
     </tr><tr>
       <td><label>Analog input 2:</label></td>
       <td><input type="range" class="range2" id="ain2" name="ain2" min="0" max="1024" oninput="myAnin('ain2','aout2')"></td>
       <td><button type="button" class="button2" onclick="sendCommand('1','0')">Set</button> </td>
       <td><output id="aout2"></output></td>
     </tr>
    </table>
    <table id="table0">
     <tr>
           <td><button onclick="myButton('stop')">Stop</button> <br /></td>
           <td><button onclick="myButton('fwd')">Fwd</button> <br /></td>
           <td><button onclick="myButton('back')">Back</button> <br /></td>
           <td><button onclick="myButton('home')">Home</button> <br /></td>
           <td><button onclick="myButton('extend')">Extend</button> <br /></td>

    </table>
    <br />
    
    <table id="table1"> </table>
    <form id="form1"> </form>

    <div class="round">
      <h2>Info</h2>
        <p>Voltage: <span id="voltage">0</span>V</p>
        <p>Temperature: <span id="temp">0</span>C</p>
    </div>
    
</body>
</html>

)=====";

#endif
