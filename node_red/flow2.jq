[
  {
    "id": "684298c2.d27318",
    "type": "tab",
    "label": "Flow 2",
    "disabled": false,
    "info": ""
  },
  {
    "id": "f1163192.b6e8e",
    "type": "mqtt in",
    "z": "684298c2.d27318",
    "name": "",
    "topic": "robot/ultrasonic",
    "qos": "2",
    "datatype": "auto",
    "broker": "7c845b73.38ac14",
    "x": 100,
    "y": 180,
    "wires": [
      [
        "cf8221f6.1ca66",
        "936fc109.9d871"
      ]
    ]
  },
  {
    "id": "cf8221f6.1ca66",
    "type": "switch",
    "z": "684298c2.d27318",
    "name": "dist < 10",
    "property": "payload",
    "propertyType": "msg",
    "rules": [
      {
        "t": "lt",
        "v": "10",
        "vt": "num"
      },
      {
        "t": "else"
      }
    ],
    "checkall": "true",
    "repair": false,
    "outputs": 2,
    "x": 200,
    "y": 280,
    "wires": [
      [
        "78fc0e56.bf465"
      ],
      [
        "4d734354.8cbcdc"
      ]
    ]
  },
  {
    "id": "9c622746.4fee88",
    "type": "mqtt out",
    "z": "684298c2.d27318",
    "name": "",
    "topic": "",
    "qos": "",
    "retain": "",
    "broker": "7c845b73.38ac14",
    "x": 670,
    "y": 240,
    "wires": []
  },
  {
    "id": "78fc0e56.bf465",
    "type": "change",
    "z": "684298c2.d27318",
    "name": "Yes:Stop Motor",
    "rules": [
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "stop",
        "tot": "str"
      },
      {
        "t": "set",
        "p": "topic",
        "pt": "msg",
        "to": "robot/stop",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 340,
    "y": 180,
    "wires": [
      [
        "9c622746.4fee88",
        "5a84f2de.bcc51c"
      ]
    ]
  },
  {
    "id": "4d734354.8cbcdc",
    "type": "change",
    "z": "684298c2.d27318",
    "name": "No:Forward 20",
    "rules": [
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "20",
        "tot": "num"
      },
      {
        "t": "set",
        "p": "topic",
        "pt": "msg",
        "to": "robot/forward",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 360,
    "y": 380,
    "wires": [
      [
        "9c622746.4fee88",
        "5a84f2de.bcc51c"
      ]
    ]
  },
  {
    "id": "5a84f2de.bcc51c",
    "type": "debug",
    "z": "684298c2.d27318",
    "name": "Status",
    "active": true,
    "tosidebar": false,
    "console": false,
    "tostatus": true,
    "complete": "payload",
    "x": 490,
    "y": 260,
    "wires": []
  },
  {
    "id": "936fc109.9d871",
    "type": "debug",
    "z": "684298c2.d27318",
    "name": "Distance",
    "active": true,
    "tosidebar": false,
    "console": false,
    "tostatus": true,
    "complete": "payload",
    "targetType": "msg",
    "x": 300,
    "y": 120,
    "wires": []
  },
  {
    "id": "67923319.aa490c",
    "type": "inject",
    "z": "684298c2.d27318",
    "name": "",
    "topic": "",
    "payload": "{\"-45\":300,\"-25\":15,\"-5\":30}",
    "payloadType": "json",
    "repeat": "",
    "crontab": "",
    "once": false,
    "onceDelay": 0.1,
    "x": 90,
    "y": 440,
    "wires": [
      [
        "2c3877b9.2bc118"
      ]
    ]
  },
  {
    "id": "2c3877b9.2bc118",
    "type": "ui_gauge",
    "z": "684298c2.d27318",
    "name": "",
    "group": "85f28084.76b1",
    "order": 7,
    "width": 0,
    "height": 0,
    "gtype": "gage",
    "title": "gauge",
    "label": "units",
    "format": "{{value}}",
    "min": 0,
    "max": 10,
    "colors": [
      "#00b500",
      "#e6e600",
      "#ca3838"
    ],
    "seg1": "",
    "seg2": "",
    "x": 380,
    "y": 480,
    "wires": []
  },
  {
    "id": "7c845b73.38ac14",
    "type": "mqtt-broker",
    "z": "",
    "name": "pizero4",
    "broker": "pizero4",
    "port": "1883",
    "clientid": "",
    "usetls": false,
    "compatmode": true,
    "keepalive": "60",
    "cleansession": true,
    "birthTopic": "",
    "birthQos": "0",
    "birthPayload": "",
    "closeTopic": "",
    "closeQos": "0",
    "closePayload": "",
    "willTopic": "",
    "willQos": "0",
    "willPayload": ""
  },
  {
    "id": "85f28084.76b1",
    "type": "ui_group",
    "z": "",
    "name": "one",
    "tab": "bb4338c9.d3bf18",
    "disp": true,
    "width": "6",
    "collapse": false
  },
  {
    "id": "bb4338c9.d3bf18",
    "type": "ui_tab",
    "z": "",
    "name": "Home",
    "icon": "dashboard",
    "disabled": false,
    "hidden": false
  }
]
