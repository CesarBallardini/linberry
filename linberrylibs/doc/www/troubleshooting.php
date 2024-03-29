<? include ("linberry.inc"); ?>

<? createHeader("Troubleshooting"); ?>

<? createSubHeader("Common Issues When Working With Linberry"); ?>


<p><b>Q:</b> I have one of the newer Pearl devices, such as the 8120.
When I try to run a backup using the GUI, the device resets itself.</p>

<p><b>A:</b> This is a known problem, and is likely a bug in the
Blackberry firmware which gets triggered when both the usb_storage kernel
module and Linberry access the device at the same time.  When Linberry closes
its USB connection, the device gets confused and resets itself.</p>

<p>The only known workaround is to unload the usb_storage module when
accessing the database (such as when syncing or doing backups).</p>


<hr/>


<p><b>Q:</b> When I run btool, it hangs for 30 seconds and then times out on
the USB read call.</p>

<p><b>A:</b> From reports on the mailing list, this appears most likely to be
a bug in the BlackBerry firmware.  Sometimes upgrading your
Blackberry will fix this issue.</p>

<p>This is not conclusive, and may be a kernel issue as well.</p>

<p>To verify that you are experiencing the right issue, run
"btool -vl" to get a verbose USB log.  At the beginning
the USB configuration and endpoint information is printed.
If the last set of endpoints have 0's in the values, you are
experiencing the problem.  The proper set of endpoints will not
be displayed with "lsusb -v" in this case either.</p>

<p>One workaround is to run bcharge twice like this:</p>

<pre>
	bcharge
	bcharge -o
</pre>

<p>After the second bcharge, btool should work normally.</p>


<hr/>


<p><b>Q:</b> I've installed Linberry, and bcharge is set to run automatically,
but as soon as I plug my device into the USB port, the device reboots.</p>

<p><b>A:</b>This is a bug in the udev script, which causes the bcharge program
to be run 7 to 10 times back to back.  This confuses the BlackBerry
enough to reset it.  As of 2007/07/28, there is a corrected
udev script example in Linberry CVS.  This corrected script
was included in the Linberry 0.8 release, and newer versions.</p>

<p>Alternative issue: You may have the berry_charge kernel module
running, in which case you need to choose which method you wish
to use.  Do not run both berry_charge and bcharge.</p>


<hr/>


<p><b>Q:</b> btool is giving me errors, saying it can't find my device, or
can't access it!</p>

<p><b>A:</b> If you have one of the newer BlackBerry devices, such as the Pearl
or the Curve, these devices default to USB Mass Storage mode
when you first plug them in.  The Linux kernel detects this,
and on most systems loads the usb_storage module automatically,
sometimes mounting the device as a drive.</p>

<p>If you are running an older version of Linberry, please upgrade, as
this issue has been fixed so usb_storage and database access can
coexist in peace.</p>


<hr/>


<p><b>Q:</b> One of the Linberry applications crashed, and now I keep getting
a Desktop error message in hex.</p>

<p><b>A:</b> If a Linberry application crashes, that means the Desktop mode was
not shutdown properly.  Your device will likely appear to still
be "busy" as well.  Currently the only way to fix this is to
reset the device, either by unplugging/replugging it, or by
running the program "breset".</p>


<hr/>


<p><b>Q:</b> Everytime I restore a backup with the backup GUI, the database
I need does not get restored.  What gives?</p>

<p><b>A:</b> Both the backup and the restore are filtered based on your
configuration.  Check your Edit | Config... settings and make
sure the needed database is turned on in <i>both</i> backup and restore.</p>


<hr/>


<p><b>Q:</b> I ran the backup, but can't find my backup files!
Where did they go?</p>

<p><b>A:</b> The backup program saves all backup files as gzip'd tarballs
under your home directory:  ~/.linberry/backup/pin_number/</p>

<p> Each backup session creates a new file, named with the
pin number, date, and time.</p>

