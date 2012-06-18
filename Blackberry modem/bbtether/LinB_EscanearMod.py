
'''
Israel Marrero


http://www.wxpython.org/docs/api/
'''
import os
import sys
import time

import base64
import bb_messenging
import bb_prefs
import bb_tether
import bb_usb
import threading


berry = bb_usb.find_berry(None, None)
if berry != None:
	if berry.handle==None:
		berry.open_handle()
	prefs=bb_prefs.get_prefs()
	prefs.remove_section(bb_prefs.SECTION_SCANNED_EP)
	#Note: will replace prefs with sanedd values
	bb_usb.read_bb_endpoints(berry, None)


if self.prefs != None:
	try:
		self.prefs.Destroy()
	except:
		pass
try:
	self.Destroy()
except:
	pass

		


