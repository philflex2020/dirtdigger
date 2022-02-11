import sqlite3 as lite
import sys

con = lite.connect('sensorsData.db')

with con:
    cur = con.cursor() 
    cur.execute("INSERT INTO DHT_data VALUES(datetime('now'), 20.5, 30)")
    cur.execute("INSERT INTO DHT_data VALUES(datetime('now'), 25.8, 40)")
    cur.execute("INSERT INTO DHT_data VALUES(datetime('now'), 30.3, 50)")
    
