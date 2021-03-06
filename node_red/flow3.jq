[
  {
    "id": "9cfdd04d.a8f89",
    "type": "tab",
    "label": "Flow 3",
    "disabled": false,
    "info": ""
  },
  {
    "id": "e153abb3.847a48",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m={};\nm.labels = [\"January\", \"February\", \"March\", \"April\", \"May\", \"June\", \"July\"];\nm.series = ['Series A', 'Series B', 'Series C'];\nm.data = [\n    [65, 59, 80, 81, 56, 55, 40],\n    [28, 48, 40, 19, 86, 27, 90],\n    [38, 28, 20, 49, 45, 60, 20]\n  ];\nreturn {payload:[m]};",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 400,
    "wires": [
      [
        "1f7c4159.6a21cf",
        "3218caed.a2b446",
        "b4535005.8f4fa",
        "e41ef24a.0014c"
      ]
    ]
  },
  {
    "id": "1f7c4159.6a21cf",
    "type": "ui_chart",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "c46a7054.a8211",
    "order": 1,
    "width": 0,
    "height": 0,
    "label": "line chart",
    "chartType": "line",
    "legend": "false",
    "xformat": "auto",
    "interpolate": "linear",
    "nodata": "No Data",
    "dot": false,
    "ymin": "",
    "ymax": "",
    "removeOlder": 1,
    "removeOlderPoints": "",
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
    "outputs": 2,
    "x": 480,
    "y": 140,
    "wires": [
      [],
      []
    ]
  },
  {
    "id": "9133cade.a78a78",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var chart = [{\n    \"series\":[\"A\",\"B\",\"C\"],\n    \"data\":[[{\"x\":1504029632890,\"y\":5},{\"x\":1504029636001,\"y\":4},{\"x\":1504029638656,\"y\":2}],[{\"x\":1504029633514,\"y\":6},{\"x\":1504029636622,\"y\":7},{\"x\":1504029639539,\"y\":6}],[{\"x\":1504029634400,\"y\":7},{\"x\":1504029637959,\"y\":9},{\"x\":1504029640317,\"y\":7}]],\n    \"labels\":[\"\"]\n}];\nmsg.payload = chart;\n\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 240,
    "wires": [
      [
        "1f7c4159.6a21cf"
      ]
    ]
  },
  {
    "id": "d54144b6.47cf18",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 1,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "Time Line 1",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 70,
    "y": 240,
    "wires": [
      [
        "9133cade.a78a78"
      ]
    ]
  },
  {
    "id": "3218caed.a2b446",
    "type": "ui_chart",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "c46a7054.a8211",
    "order": 2,
    "width": 0,
    "height": 0,
    "label": "bar chart",
    "chartType": "bar",
    "legend": "false",
    "xformat": "HH:mm:ss",
    "interpolate": "linear",
    "nodata": "No Data",
    "dot": false,
    "ymin": "",
    "ymax": "",
    "removeOlder": 1,
    "removeOlderPoints": "",
    "removeOlderUnit": "3600",
    "cutout": 0,
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
    "outputs": 2,
    "x": 480,
    "y": 200,
    "wires": [
      [],
      []
    ]
  },
  {
    "id": "bf16b4b0.df7bd8",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 5,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "array",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 50,
    "y": 400,
    "wires": [
      [
        "e153abb3.847a48"
      ]
    ]
  },
  {
    "id": "cf537c84.eb356",
    "type": "random",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "low": "1",
    "high": "10000000",
    "inte": "true",
    "x": 240,
    "y": 600,
    "wires": [
      [
        "1f7c4159.6a21cf"
      ]
    ]
  },
  {
    "id": "96939bce.164438",
    "type": "inject",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "topic": "",
    "payload": "",
    "payloadType": "str",
    "repeat": "",
    "crontab": "",
    "once": false,
    "x": 70,
    "y": 600,
    "wires": [
      [
        "cf537c84.eb356"
      ]
    ]
  },
  {
    "id": "75d8f458.baa6ac",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m={};\nm.labels = [10,20,30,40,50,60,70];\nm.series = ['Series A', 'Series B', 'Series C', 'Series D'];\nm.data = [\n    [65, 59, 80, 81, 56, 55, 40],\n    [28, 48, 40, 19, 86, 27, 90],\n    [38, 28, 20, 49, 45, 60, 20],\n    [58, 18, 40, 29, 15, 30, 60]\n  ];\nreturn {payload:[m]};",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 440,
    "wires": [
      [
        "1f7c4159.6a21cf",
        "3218caed.a2b446",
        "b4535005.8f4fa",
        "e41ef24a.0014c"
      ]
    ]
  },
  {
    "id": "2eb1fc4.3d18b04",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 6,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "array2",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 50,
    "y": 440,
    "wires": [
      [
        "75d8f458.baa6ac"
      ]
    ]
  },
  {
    "id": "d3208d48.539a",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var chart = [{\n    \"series\":[\"A\",\"B\",\"C\"],\n        \"data\":[[{\"x\":1,\"y\":5},{\"x\":2,\"y\":4},{\"x\":3,\"y\":2}],[{\"x\":4,\"y\":6},{\"x\":5,\"y\":7},{\"x\":6,\"y\":6}],[{\"x\":7,\"y\":7},{\"x\":8,\"y\":9},{\"x\":9,\"y\":7}]],\n    \"labels\":[\"\"]\n}];\nmsg.payload = chart;\n\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 280,
    "wires": [
      [
        "1f7c4159.6a21cf"
      ]
    ]
  },
  {
    "id": "a399b3c2.e82f1",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 2,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "Time Line 2",
    "tooltip": "",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 70,
    "y": 280,
    "wires": [
      [
        "d3208d48.539a"
      ]
    ]
  },
  {
    "id": "63753840.980758",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m = {};\nm.labels = [\"Download\", \"In-Store\", \"Mail-Order\"];\nm.data = [[300, 500, 100]];\nm.series = [\"Sales\"];\nreturn {payload:[m],topic:msg.topic};",
    "outputs": 1,
    "noerr": 0,
    "x": 230,
    "y": 640,
    "wires": [
      [
        "e41ef24a.0014c",
        "3218caed.a2b446",
        "1f7c4159.6a21cf",
        "39fcd5da.64b56a",
        "d5b4d0b0.60729",
        "b4535005.8f4fa"
      ]
    ]
  },
  {
    "id": "e41ef24a.0014c",
    "type": "ui_chart",
    "z": "9cfdd04d.a8f89",
    "name": "pie chart",
    "group": "c46a7054.a8211",
    "order": 3,
    "width": 0,
    "height": 0,
    "label": "pie chart",
    "chartType": "pie",
    "legend": "true",
    "xformat": "HH:mm:ss",
    "interpolate": "linear",
    "nodata": "",
    "dot": false,
    "ymin": "",
    "ymax": "",
    "removeOlder": 1,
    "removeOlderPoints": "",
    "removeOlderUnit": "3600",
    "cutout": "30",
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
    "outputs": 2,
    "x": 520,
    "y": 380,
    "wires": [
      [],
      []
    ]
  },
  {
    "id": "cd8a0e95.f584c",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 10,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "array for pie",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "Pie 4 T",
    "x": 70,
    "y": 640,
    "wires": [
      [
        "63753840.980758"
      ]
    ]
  },
  {
    "id": "2ef43e0f.92b812",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var chart = [{\n    \"series\":[\"A\",\"B\",\"C\"],\n        \"data\":[[{\"x\":1000000,\"y\":5},{\"x\":2000000,\"y\":4},{\"x\":3000000,\"y\":2}],[{\"x\":4000000,\"y\":6},{\"x\":5000000,\"y\":7},{\"x\":6000000,\"y\":6}],[{\"x\":7000000,\"y\":7},{\"x\":8000000,\"y\":9},{\"x\":9000000,\"y\":7}]],\n    \"labels\":[\"\"]\n}];\nmsg.payload = chart;\n\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 320,
    "wires": [
      [
        "1f7c4159.6a21cf"
      ]
    ]
  },
  {
    "id": "c5cd1c4d.a0eda",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 3,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "Time Line 3",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 70,
    "y": 320,
    "wires": [
      [
        "2ef43e0f.92b812"
      ]
    ]
  },
  {
    "id": "b3a0c4b1.7f85e8",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var chart = [{\n    \"series\":[\"A\",\"B\",\"C\"],\n        \"data\":[[{\"x\":1000000000,\"y\":5},{\"x\":2000000000,\"y\":4},{\"x\":3000000000,\"y\":2}],[{\"x\":4000000000,\"y\":6},{\"x\":5000000000,\"y\":7},{\"x\":6000000000,\"y\":6}],[{\"x\":7000000000,\"y\":7},{\"x\":8000000000,\"y\":9},{\"x\":9000000000,\"y\":7}]],\n    \"labels\":[\"\"]\n}];\nmsg.payload = chart;\n\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 360,
    "wires": [
      [
        "1f7c4159.6a21cf"
      ]
    ]
  },
  {
    "id": "19098cdf.afd4e3",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 4,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "Time Line 4",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 70,
    "y": 360,
    "wires": [
      [
        "b3a0c4b1.7f85e8"
      ]
    ]
  },
  {
    "id": "6cda9f8d.7a2ac",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m={\n    \"series\":[\"X\",\"Y\",\"Z\"],\n    \"data\":[[5,6,9],[3,8,5],[6,7,2]],\n    \"labels\":[\"Jan\",\"Feb\",\"Mar\"]\n};\nreturn {payload:[m]};",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 560,
    "wires": [
      [
        "1f7c4159.6a21cf",
        "3218caed.a2b446",
        "e41ef24a.0014c",
        "39fcd5da.64b56a",
        "d5b4d0b0.60729",
        "b4535005.8f4fa"
      ]
    ]
  },
  {
    "id": "88447ffb.a6683",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 9,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "array3c",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 60,
    "y": 560,
    "wires": [
      [
        "6cda9f8d.7a2ac"
      ]
    ]
  },
  {
    "id": "316a4fac.b8511",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 11,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "array 2 for pie",
    "tooltip": "",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "Pie Hole",
    "x": 80,
    "y": 680,
    "wires": [
      [
        "a6549c3f.4f8d4"
      ]
    ]
  },
  {
    "id": "a6549c3f.4f8d4",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m = {};\nm.labels = ['2006', '2007', '2008', '2009', '2010', '2011', '2012'];\nm.data = [[28, 48, 40, 19, 86, 27, 90]];\nm.series = ['Series A'];\nreturn {payload:[m],topic:msg.topic};",
    "outputs": 1,
    "noerr": 0,
    "x": 230,
    "y": 680,
    "wires": [
      [
        "e41ef24a.0014c",
        "3218caed.a2b446",
        "d5b4d0b0.60729",
        "39fcd5da.64b56a",
        "1f7c4159.6a21cf",
        "b4535005.8f4fa",
        "3bc1a4b8.4701ac"
      ]
    ]
  },
  {
    "id": "9f8dee11.c75e7",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "topic",
    "func": "var m = [\n    {topic:\"X\", payload:22},\n    {topic:\"Y\", payload:66},\n    {topic:\"Z\", payload:42}\n    ];\nreturn [m];",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 200,
    "wires": [
      [
        "3218caed.a2b446",
        "e41ef24a.0014c",
        "1f7c4159.6a21cf",
        "b4535005.8f4fa"
      ]
    ]
  },
  {
    "id": "51b2ecd1.855174",
    "type": "inject",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "topic": "",
    "payload": "",
    "payloadType": "date",
    "repeat": "",
    "crontab": "",
    "once": false,
    "x": 60,
    "y": 200,
    "wires": [
      [
        "9f8dee11.c75e7"
      ]
    ]
  },
  {
    "id": "a8aafecd.828b",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "label",
    "func": "var m = [\n    {label:\"A\", payload:22},\n    {label:\"B\", payload:66},\n    {label:\"C\", payload:42},\n    ];\nreturn [m];",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 160,
    "wires": [
      [
        "3218caed.a2b446",
        "e41ef24a.0014c",
        "1f7c4159.6a21cf",
        "b4535005.8f4fa"
      ]
    ]
  },
  {
    "id": "adaa46a9.b6a5d8",
    "type": "inject",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "topic": "",
    "payload": "",
    "payloadType": "date",
    "repeat": "",
    "crontab": "",
    "once": false,
    "x": 60,
    "y": 160,
    "wires": [
      [
        "a8aafecd.828b"
      ]
    ]
  },
  {
    "id": "6dd832d2.4337fc",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m={\n    \"series\":[\"X\",\"Y\",\"Z\"],\n    \"data\":[[5],[3],[6]],\n    \"labels\":[\"Jan\"]\n};\nreturn {payload:[m]};",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 480,
    "wires": [
      [
        "3218caed.a2b446",
        "39fcd5da.64b56a",
        "d5b4d0b0.60729",
        "1f7c4159.6a21cf",
        "b4535005.8f4fa",
        "e41ef24a.0014c"
      ]
    ]
  },
  {
    "id": "9337a4a6.f82a48",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 7,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "array3a",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 60,
    "y": 480,
    "wires": [
      [
        "6dd832d2.4337fc"
      ]
    ]
  },
  {
    "id": "d7926c53.23787",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m={\n    \"series\":[\"X\"],\n    \"data\":[[5,3,6]],\n    \"labels\":[\"Jan\",\"Feb\",\"Mar\"]\n};\nreturn {payload:[m]};",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 520,
    "wires": [
      [
        "3218caed.a2b446",
        "1f7c4159.6a21cf",
        "d5b4d0b0.60729",
        "39fcd5da.64b56a",
        "b4535005.8f4fa",
        "e41ef24a.0014c"
      ]
    ]
  },
  {
    "id": "6160b64c.2958f8",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 8,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "array3b",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "",
    "payloadType": "str",
    "topic": "",
    "x": 60,
    "y": 520,
    "wires": [
      [
        "d7926c53.23787"
      ]
    ]
  },
  {
    "id": "b4535005.8f4fa",
    "type": "ui_chart",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "6a8a2d68.af1c14",
    "order": 2,
    "width": 0,
    "height": 0,
    "label": "horizontal bar",
    "chartType": "horizontalBar",
    "legend": "false",
    "xformat": "HH:mm:ss",
    "interpolate": "linear",
    "nodata": "",
    "dot": false,
    "ymin": "",
    "ymax": "",
    "removeOlder": 1,
    "removeOlderPoints": "",
    "removeOlderUnit": "3600",
    "cutout": "30",
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
    "outputs": 2,
    "x": 500,
    "y": 260,
    "wires": [
      [],
      []
    ]
  },
  {
    "id": "39fcd5da.64b56a",
    "type": "ui_chart",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "6a8a2d68.af1c14",
    "order": 3,
    "width": 0,
    "height": 0,
    "label": "polar chart",
    "chartType": "polar-area",
    "legend": "false",
    "xformat": "HH:mm:ss",
    "interpolate": "linear",
    "nodata": "",
    "dot": false,
    "ymin": "",
    "ymax": "",
    "removeOlder": 1,
    "removeOlderPoints": "",
    "removeOlderUnit": "3600",
    "cutout": "30",
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
    "outputs": 2,
    "x": 530,
    "y": 500,
    "wires": [
      [],
      []
    ]
  },
  {
    "id": "d5b4d0b0.60729",
    "type": "ui_chart",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "6a8a2d68.af1c14",
    "order": 4,
    "width": 0,
    "height": 0,
    "label": "radar chart",
    "chartType": "radar",
    "legend": "false",
    "xformat": "HH:mm:ss",
    "interpolate": "linear",
    "nodata": "",
    "dot": false,
    "ymin": "",
    "ymax": "",
    "removeOlder": 1,
    "removeOlderPoints": "",
    "removeOlderUnit": "3600",
    "cutout": "30",
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
    "outputs": 2,
    "x": 530,
    "y": 560,
    "wires": [
      [],
      []
    ]
  },
  {
    "id": "fcefdc7.fd4e72",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "label",
    "func": "var m = [\n    {label:\"A\", payload:22, series:\"X\"},\n    {label:\"B\", payload:66, series:\"X\"},\n    {label:\"C\", payload:42, series:\"X\"},\n    {label:\"A\", payload:33, series:\"Y\"},\n    {label:\"B\", payload:72, series:\"Y\"},\n    {label:\"C\", payload:12, series:\"Y\"},\n    {label:\"A\", payload:44, series:\"Z\"},\n    {label:\"B\", payload:32, series:\"Z\"},\n    {label:\"C\", payload:80, series:\"Z\"}\n    ];\nreturn [m];",
    "outputs": 1,
    "noerr": 0,
    "x": 210,
    "y": 120,
    "wires": [
      [
        "e41ef24a.0014c",
        "3218caed.a2b446",
        "1f7c4159.6a21cf",
        "b4535005.8f4fa"
      ]
    ]
  },
  {
    "id": "9aa89082.0af9",
    "type": "inject",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "topic": "",
    "payload": "",
    "payloadType": "date",
    "repeat": "",
    "crontab": "",
    "once": false,
    "x": 60,
    "y": 120,
    "wires": [
      [
        "fcefdc7.fd4e72"
      ]
    ]
  },
  {
    "id": "2d04df71.98ec6",
    "type": "ui_button",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "group": "1aece159.33c47f",
    "order": 11,
    "width": 0,
    "height": 0,
    "passthru": false,
    "label": "Clear All",
    "color": "",
    "bgcolor": "",
    "icon": "",
    "payload": "[]",
    "payloadType": "json",
    "topic": "",
    "x": 200,
    "y": 80,
    "wires": [
      [
        "d5b4d0b0.60729",
        "39fcd5da.64b56a",
        "e41ef24a.0014c",
        "b4535005.8f4fa",
        "3218caed.a2b446",
        "1f7c4159.6a21cf"
      ]
    ]
  },
  {
    "id": "11bcfce9.ccee73",
    "type": "mqtt in",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "topic": "/sensor/data",
    "qos": "2",
    "datatype": "auto",
    "broker": "7c845b73.38ac14",
    "x": 100,
    "y": 780,
    "wires": [
      [
        "4c570d60.270b14"
      ]
    ]
  },
  {
    "id": "3bc1a4b8.4701ac",
    "type": "debug",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "active": true,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "false",
    "x": 380,
    "y": 780,
    "wires": []
  },
  {
    "id": "4c570d60.270b14",
    "type": "json",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "property": "payload",
    "action": "",
    "pretty": false,
    "x": 260,
    "y": 860,
    "wires": [
      [
        "d5b4d0b0.60729",
        "d9872e63.11d28",
        "39fcd5da.64b56a"
      ]
    ]
  },
  {
    "id": "a9fa66e.e9db898",
    "type": "function",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "func": "var m = {};\nm.labels = ['2006', '2007', '2008', '2009', '2010', '2011', '2012'];\nm.data = [[28, 48, 40, 19, 86, 27, 90]];\nm.series = ['Series A'];\nreturn {payload:[m],topic:msg.topic};",
    "outputs": 1,
    "noerr": 0,
    "x": 230,
    "y": 720,
    "wires": [
      []
    ]
  },
  {
    "id": "d9872e63.11d28",
    "type": "debug",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "active": false,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "false",
    "x": 430,
    "y": 860,
    "wires": []
  },
  {
    "id": "1e0700b.8c37eff",
    "type": "inject",
    "z": "9cfdd04d.a8f89",
    "name": "",
    "topic": "pie test",
    "payload": "",
    "payloadType": "str",
    "repeat": "",
    "crontab": "",
    "once": false,
    "onceDelay": 0.1,
    "x": 70,
    "y": 720,
    "wires": [
      [
        "a6549c3f.4f8d4"
      ]
    ]
  },
  {
    "id": "c46a7054.a8211",
    "type": "ui_group",
    "z": "",
    "name": "Charts",
    "tab": "526b7c44.c8a534",
    "order": 2,
    "disp": false,
    "width": "6"
  },
  {
    "id": "1aece159.33c47f",
    "type": "ui_group",
    "z": "",
    "name": "Inputs",
    "tab": "526b7c44.c8a534",
    "order": 1,
    "disp": false,
    "width": "6"
  },
  {
    "id": "6a8a2d68.af1c14",
    "type": "ui_group",
    "z": "",
    "name": "Group 3",
    "tab": "526b7c44.c8a534",
    "order": 3,
    "disp": false,
    "width": "6"
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
    "id": "526b7c44.c8a534",
    "type": "ui_tab",
    "z": "",
    "name": "Charts",
    "icon": "dashboard",
    "order": 2
  }
]
