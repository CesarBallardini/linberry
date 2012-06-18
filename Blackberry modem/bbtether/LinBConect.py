#!/usr/bin/python
'''
Israel Marrero
25/12/2010
'''
import sys
import os

import LinB_Conect

# MAIN
os.system('clear')
print "LinBerryModem_conect 1.0a3  (http://linberry.webcindario.com)"


if len(sys.argv) < 3:
	print "	Ejecutado solo desde LinBerry, saliendo...."
	print " 00"
	sys.exit()

#aqui va la clave
if sys.argv[2].find("MICLAVE") == -1:
	print "	Ejecutado solo desde LinBerry, saliendo...."
	print " 01"
	sys.exit()


(options, args) = LinB_Conect.parse_cmd(sys.argv[1:])
LinBConect=LinB_Conect.BBTether()
LinBConect.start(options, args)

'''
Fin
'''
