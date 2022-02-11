import sqlite3
import sys

conn=sqlite3.connect('sensorsData.db')
curs=conn.cursor()

# function to insert data on a table
def add_data (temp, hum):
    curs.execute("INSERT INTO DHT_data values(datetime('now'), (?), (?))", (temp, hum))
    conn.commit()

# call the function to insert data
add_data (20.5, 30)
add_data (25.8, 40)
add_data (30.3, 50)

# print database content
print ("\nEntire database contents:\n")
for row in curs.execute("SELECT * FROM DHT_data"):
    print (row)

# close the database after use
conn.close()
