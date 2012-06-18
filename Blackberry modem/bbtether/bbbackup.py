#!/usr/bin/python
'''
Israel Marrero Mayo 2012
'''
import sys

import bb_backup

# MAIN
(options, args) = bb_backup.parse_cmd(sys.argv[1:])
bbbackup=bb_backup.BBBackup()
bbbackup.start(options, args)
