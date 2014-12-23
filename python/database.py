#!/usr/bin/env python

import sqlite3


def add_multi_reading (temp, humidity, lux):

    curs.execute("INSERT INTO stats values(date('now'), time('now'), (?), (?), (?))", (temp, humidity, lux,))

    # commit the changes
    conn.commit()



conn=sqlite3.connect('statslog.db')

curs=conn.cursor()

add_multi_reading(12.0, 68.3, 450.0)

print "\nEntire database contents:\n"
for row in curs.execute("SELECT * FROM stats"):
    print row

conn.close()
