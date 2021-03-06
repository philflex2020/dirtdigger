[
  {
    "id": "830c0ae7.c763d8",
    "type": "tab",
    "label": "Flow 1",
    "disabled": false,
    "info": ""
  },
  {
    "id": "9c6a19aa.0263a8",
    "type": "rpi-gpio in",
    "z": "830c0ae7.c763d8",
    "name": "",
    "pin": "12",
    "intype": "up",
    "debounce": "25",
    "read": true,
    "x": 100,
    "y": 150,
    "wires": [
      [
        "1fe4ff13.edadd1",
        "18046caa.8f1663",
        "ccb9b40f.676568"
      ]
    ]
  },
  {
    "id": "1fe4ff13.edadd1",
    "type": "debug",
    "z": "830c0ae7.c763d8",
    "name": "",
    "active": false,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "false",
    "x": 400,
    "y": 120,
    "wires": []
  },
  {
    "id": "18046caa.8f1663",
    "type": "rpi-gpio out",
    "z": "830c0ae7.c763d8",
    "name": "",
    "pin": "16",
    "set": true,
    "level": "0",
    "freq": "",
    "out": "out",
    "x": 400,
    "y": 200,
    "wires": []
  },
  {
    "id": "14e6eb30.478595",
    "type": "rpi-gpio out",
    "z": "830c0ae7.c763d8",
    "name": "",
    "pin": "18",
    "set": true,
    "level": "0",
    "freq": "",
    "out": "out",
    "x": 400,
    "y": 280,
    "wires": []
  },
  {
    "id": "af404f1a.b254a",
    "type": "ui_button",
    "z": "830c0ae7.c763d8",
    "name": "",
    "group": "85f28084.76b1",
    "order": 0,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "on",
    "tooltip": "",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "1",
    "payloadType": "num",
    "topic": "",
    "x": 90,
    "y": 220,
    "wires": [
      [
        "14e6eb30.478595"
      ]
    ]
  },
  {
    "id": "4b588936.86b188",
    "type": "ui_button",
    "z": "830c0ae7.c763d8",
    "name": "",
    "group": "85f28084.76b1",
    "order": 0,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "off",
    "tooltip": "",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "0",
    "payloadType": "num",
    "topic": "",
    "x": 90,
    "y": 260,
    "wires": [
      [
        "14e6eb30.478595"
      ]
    ]
  },
  {
    "id": "a528c42b.8a1508",
    "type": "ui_button",
    "z": "830c0ae7.c763d8",
    "name": "",
    "group": "85f28084.76b1",
    "order": 0,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "toggle",
    "tooltip": "",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "1",
    "payloadType": "num",
    "topic": "",
    "x": 90,
    "y": 300,
    "wires": [
      [
        "ccb9b40f.676568"
      ]
    ]
  },
  {
    "id": "ccb9b40f.676568",
    "type": "function",
    "z": "830c0ae7.c763d8",
    "name": "toggle",
    "func": "if (msg.payload == 1) { \n   x = context.get(\"val\") || 0;\n   if (x == 1) {\n       x = 0;\n   } else {\n       x=1;\n   }\n   msg.payload = x;\n   //x =  x+1;\n   context.set(\"val\",x);\n}\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 250,
    "y": 320,
    "wires": [
      [
        "14e6eb30.478595"
      ]
    ]
  },
  {
    "id": "3eb93d95.cd8952",
    "type": "debug",
    "z": "830c0ae7.c763d8",
    "name": "",
    "active": false,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "false",
    "x": 450,
    "y": 600,
    "wires": []
  },
  {
    "id": "8f8b34c6.628458",
    "type": "mqtt in",
    "z": "830c0ae7.c763d8",
    "name": "",
    "topic": "adc_channel",
    "qos": "2",
    "datatype": "auto",
    "broker": "11e1159.13996ea",
    "x": 100,
    "y": 420,
    "wires": [
      [
        "9d6109fa.67f358",
        "7df472f0.4a195c",
        "3eb93d95.cd8952"
      ]
    ]
  },
  {
    "id": "9d6109fa.67f358",
    "type": "json",
    "z": "830c0ae7.c763d8",
    "name": "",
    "property": "payload",
    "action": "",
    "pretty": false,
    "x": 170,
    "y": 500,
    "wires": [
      [
        "f44e8836.a48cc8"
      ]
    ]
  },
  {
    "id": "7df472f0.4a195c",
    "type": "debug",
    "z": "830c0ae7.c763d8",
    "name": "",
    "active": false,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "false",
    "x": 430,
    "y": 380,
    "wires": []
  },
  {
    "id": "f44e8836.a48cc8",
    "type": "function",
    "z": "830c0ae7.c763d8",
    "name": "",
    "func": "msg.payload=msg.payload[\"vars\"][\"var4\"];\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 290,
    "y": 500,
    "wires": [
      [
        "d4e4b050.111a6"
      ]
    ]
  },
  {
    "id": "d4e4b050.111a6",
    "type": "change",
    "z": "830c0ae7.c763d8",
    "name": "",
    "rules": [
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "$number(payload)",
        "tot": "jsonata"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 440,
    "y": 500,
    "wires": [
      [
        "67ffe9e9.6aba28"
      ]
    ]
  },
  {
    "id": "67ffe9e9.6aba28",
    "type": "ui_chart",
    "z": "830c0ae7.c763d8",
    "name": "adc44",
    "group": "85f28084.76b1",
    "order": 3,
    "width": 0,
    "height": 0,
    "label": "chart",
    "chartType": "line",
    "legend": "false",
    "xformat": "HH:mm:ss",
    "interpolate": "linear",
    "nodata": "adc4",
    "dot": false,
    "ymin": "4500",
    "ymax": "5000",
    "removeOlder": 1,
    "removeOlderPoints": "50",
    "removeOlderUnit": "3600",
    "cutout": 0,
    "useOneColor": false,
    "colors": [
      "#1f77b4",
      "#aec7e8",
      "#ff7f0e",
      "#2ca02c",
      "#98df8a",
      "#d62728",
      "#ff9896",
      "#9467bd",
      "#c5b0d5"
    ],
    "useOldStyle": false,
    "outputs": 1,
    "x": 320,
    "y": 660,
    "wires": [
      []
    ]
  },
  {
    "id": "3afc644f.c08fbc",
    "type": "ui_button",
    "z": "830c0ae7.c763d8",
    "name": "",
    "group": "85f28084.76b1",
    "order": 0,
    "width": "0",
    "height": "0",
    "passthru": false,
    "label": "{{payload}}",
    "tooltip": "",
    "color": "",
    "bgcolor": "{{colour}}",
    "icon": "",
    "payload": "press",
    "payloadType": "str",
    "topic": "",
    "x": 470,
    "y": 820,
    "wires": [
      [
        "6ff0ca1d.3d33a4"
      ]
    ]
  },
  {
    "id": "ce53a46d.2fd848",
    "type": "change",
    "z": "830c0ae7.c763d8",
    "name": "",
    "rules": [
      {
        "t": "set",
        "p": "colour",
        "pt": "msg",
        "to": "green",
        "tot": "str"
      },
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "<i class=\"fa fa-rotate-90 fa-folder-open-o\"></i> OPEN",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 280,
    "y": 820,
    "wires": [
      [
        "3afc644f.c08fbc"
      ]
    ]
  },
  {
    "id": "aff5ccb7.04f81",
    "type": "inject",
    "z": "830c0ae7.c763d8",
    "name": "",
    "topic": "",
    "payload": "",
    "payloadType": "str",
    "repeat": "",
    "crontab": "",
    "once": true,
    "onceDelay": "",
    "x": 90,
    "y": 760,
    "wires": [
      [
        "35d7b7ec.6fcaf8"
      ]
    ]
  },
  {
    "id": "6ff0ca1d.3d33a4",
    "type": "function",
    "z": "830c0ae7.c763d8",
    "name": "state",
    "func": "var state = context.state || false;\nstate = !state;\ncontext.state = state;\nif (state) { return [msg,null]; }\nelse { return [null,msg]; }\n",
    "outputs": 2,
    "noerr": 0,
    "x": 110,
    "y": 820,
    "wires": [
      [
        "ce53a46d.2fd848"
      ],
      [
        "35d7b7ec.6fcaf8"
      ]
    ]
  },
  {
    "id": "35d7b7ec.6fcaf8",
    "type": "change",
    "z": "830c0ae7.c763d8",
    "name": "",
    "rules": [
      {
        "t": "set",
        "p": "colour",
        "pt": "msg",
        "to": "red",
        "tot": "str"
      },
      {
        "t": "set",
        "p": "payload",
        "pt": "msg",
        "to": "<i class=\"fa fa-rotate-90 fa-folder-o\"></i> Closed",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 300,
    "y": 760,
    "wires": [
      [
        "3afc644f.c08fbc"
      ]
    ]
  },
  {
    "id": "fb852887.26a3f8",
    "type": "inject",
    "z": "830c0ae7.c763d8",
    "name": "",
    "topic": "",
    "payload": "false",
    "payloadType": "bool",
    "repeat": "",
    "crontab": "",
    "once": true,
    "onceDelay": "",
    "x": 90,
    "y": 920,
    "wires": [
      [
        "29c282f9.b9f84e"
      ]
    ]
  },
  {
    "id": "3a90f00f.add86",
    "type": "ui_switch",
    "z": "830c0ae7.c763d8",
    "name": "",
    "label": "{{label}}",
    "tooltip": "",
    "group": "85f28084.76b1",
    "order": 0,
    "width": 0,
    "height": 0,
    "passthru": false,
    "decouple": "true",
    "topic": "",
    "style": "",
    "onvalue": "true",
    "onvalueType": "bool",
    "onicon": "fa-sun-o",
    "oncolor": "red",
    "offvalue": "false",
    "offvalueType": "bool",
    "officon": "fa-cloud",
    "offcolor": "grey",
    "x": 480,
    "y": 920,
    "wires": [
      [
        "6f23855c.fd571c"
      ]
    ]
  },
  {
    "id": "6f23855c.fd571c",
    "type": "delay",
    "z": "830c0ae7.c763d8",
    "name": "",
    "pauseType": "delay",
    "timeout": "2",
    "timeoutUnits": "seconds",
    "rate": "1",
    "nbRateUnits": "1",
    "rateUnits": "second",
    "randomFirst": "1",
    "randomLast": "5",
    "randomUnits": "seconds",
    "drop": false,
    "x": 490,
    "y": 965,
    "wires": [
      [
        "d7172324.14f8d"
      ]
    ]
  },
  {
    "id": "29c282f9.b9f84e",
    "type": "change",
    "z": "830c0ae7.c763d8",
    "name": "",
    "rules": [
      {
        "t": "set",
        "p": "label",
        "pt": "msg",
        "to": "click for on",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 290,
    "y": 965,
    "wires": [
      [
        "3a90f00f.add86"
      ]
    ]
  },
  {
    "id": "d7172324.14f8d",
    "type": "switch",
    "z": "830c0ae7.c763d8",
    "name": "",
    "property": "payload",
    "propertyType": "msg",
    "rules": [
      {
        "t": "true"
      },
      {
        "t": "false"
      }
    ],
    "checkall": "true",
    "repair": false,
    "outputs": 2,
    "x": 90,
    "y": 965,
    "wires": [
      [
        "409e817d.56217"
      ],
      [
        "29c282f9.b9f84e"
      ]
    ]
  },
  {
    "id": "409e817d.56217",
    "type": "change",
    "z": "830c0ae7.c763d8",
    "name": "",
    "rules": [
      {
        "t": "set",
        "p": "label",
        "pt": "msg",
        "to": "now turn off",
        "tot": "str"
      }
    ],
    "action": "",
    "property": "",
    "from": "",
    "to": "",
    "reg": false,
    "x": 290,
    "y": 920,
    "wires": [
      [
        "3a90f00f.add86"
      ]
    ]
  },
  {
    "id": "32c58de7.40c822",
    "type": "ui_slider",
    "z": "830c0ae7.c763d8",
    "name": "",
    "label": "slider",
    "tooltip": "",
    "group": "85f28084.76b1",
    "order": 6,
    "width": 0,
    "height": 0,
    "passthru": true,
    "outs": "all",
    "topic": "",
    "min": "300",
    "max": "500",
    "step": 1,
    "x": 160,
    "y": 1080,
    "wires": [
      [
        "8863df94.4100c"
      ]
    ]
  },
  {
    "id": "bb7b3d9b.7c6d3",
    "type": "mqtt out",
    "z": "830c0ae7.c763d8",
    "name": "",
    "topic": "/servo/drive",
    "qos": "",
    "retain": "",
    "broker": "7c845b73.38ac14",
    "x": 420,
    "y": 1080,
    "wires": []
  },
  {
    "id": "8863df94.4100c",
    "type": "function",
    "z": "830c0ae7.c763d8",
    "name": "",
    "func": "msg.payload = \"{\\\"data\\\":{\\\"3\\\":\" +msg.payload+\"}}\";\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 280,
    "y": 1040,
    "wires": [
      [
        "a414c165.11f08",
        "bb7b3d9b.7c6d3"
      ]
    ]
  },
  {
    "id": "5b733c07.ebafa4",
    "type": "inject",
    "z": "830c0ae7.c763d8",
    "name": "",
    "topic": "",
    "payload": "300",
    "payloadType": "num",
    "repeat": "",
    "crontab": "",
    "once": false,
    "onceDelay": 0.1,
    "x": 90,
    "y": 1040,
    "wires": [
      [
        "8863df94.4100c"
      ]
    ]
  },
  {
    "id": "a414c165.11f08",
    "type": "debug",
    "z": "830c0ae7.c763d8",
    "name": "",
    "active": true,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "false",
    "x": 450,
    "y": 1040,
    "wires": []
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
    "id": "11e1159.13996ea",
    "type": "mqtt-broker",
    "z": "",
    "name": "",
    "broker": "pizero3",
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
    "id": "bb4338c9.d3bf18",
    "type": "ui_tab",
    "z": "",
    "name": "Home",
    "icon": "dashboard",
    "disabled": false,
    "hidden": false
  }
]
