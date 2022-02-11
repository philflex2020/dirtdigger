import json

with open("moves.json", "r") as read_file:
    data = json.load(read_file)

json.dumps(data)

for servo in data["move"]["servos"]:
    #print(servo)
    s = servo["name"]
    d = servo["dest"]
    print(s, d, sep="-->")
    #print(d)
#    print(f"servo {s} moveto ")

