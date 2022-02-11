import psutil
foo = psutil.sensors_temperatures()

for name,entries in foo.items():
    for entry in entries:
        os = (" %-20s %s DegC " %(entry.label or name, entry.current))
        print os
 
