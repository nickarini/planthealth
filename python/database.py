#!/usr/bin/env python

import sqlite3

conn=sqlite3.connect('statslog.db')

curs=conn.cursor()

print "\nEntire database contents:\n"
for row in curs.execute("SELECT * FROM stats"):
    print row

conn.close()
