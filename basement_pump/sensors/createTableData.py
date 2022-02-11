#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  createTableDats.py
#  
# Developed by Marcelo Rovai, MJRoBot.org @ 9Jan18
#
# made int to getneric data system
# Create a table "DHT_data" to store DHT temp and hum data

import sqlite3 as lite
import sys

con = lite.connect('genericData.db')

with con:
    
    cur = con.cursor() 
    cur.execute("DROP TABLE IF EXISTS Gen_data")
    cur.execute("CREATE TABLE Gen_data(timestamp DATETIME, idx NUMERIC, value1 NUMERIC, value2 NUMERIC)")

    
