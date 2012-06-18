<? include ("linberry.inc"); ?>

<? createHeader("USB Capture Log Archive"); ?>

<? createSubHeader("USB Capture Log Archive"); ?>

<p>Volunteers have contributed USB logs to help with reverse engineering
the low level protocol.  If you would like to contribute a log that is
not already available here, please
<? createLink("contact", "contact the developers"); ?>.</p>

<hr/>

<p><b><i>Please note:</i></b> These logs are very dense and highly
technical, and are not intended for end users, but instead intended
for developers who want to help reverse engineer the low level
USB protocol.</p>

<hr/>

<p>Some of the following log contributors are active on the
<a href="http://sourceforge.net/mail/?group_id=153722">linberry-devel mailing list</a>,
in case you wish to get in touch with them.

<p>Contributors in alphabetical order by last name:
<ul>

<li> <p>Josh Kropf</p>
	<ul>
	<p>Josh notes: The following captures where retreived using
	SniffUsb.exe on Windows XP. For each capture I allowed the device
	to settle for a few seconds before executing the single javaloader
	command. Then after copying the log file I trimmed the initial
	USB IO leading up to the request for javaloader mode. This should
	get rid of any unrelated noise from the head of the log file.</p>

	<p>To create the files below I ran each log through the awk
	script in linberry called convo.awk, and then through btranslate
	tool:</p>

	<p>awk -f convo.awk raw.log | btranslate &gt; filtered.usb</p>
	</ul>

	<ul>
		<li><? createPureFileLink("kropf/deviceinfo.usb.gz"); ?> - device info</li>
		<li><? createPureFileLink("kropf/deviceinfo.txt"); ?> - output from device info command</li>
		<li><? createPureFileLink("kropf/erase.usb.gz"); ?> - erase module</li>
		<li><? createPureFileLink("kropf/erase-error-inuse.usb.gz"); ?> - attempt to erase module that is in use resulting in an error</li>
		<li><? createPureFileLink("kropf/erase-inuse.usb.gz"); ?> - forcefully erase module in use</li>
		<li><? createPureFileLink("kropf/javaloader-reset.txt"); ?> - annotated portion of the javaloader USB dump relating to device reset with descriptions of the various packets</li>
		<li><? createPureFileLink("kropf/load.usb.gz"); ?> - load module</li>
		<li><? createPureFileLink("kropf/load-inuse.usb.gz"); ?> - load and replace in use module</li>
		<li><? createPureFileLink("kropf/save.usb.gz"); ?> - save module</li>
		<li><? createPureFileLink("kropf/save-large.usb.gz"); ?> - save large module (107748 bytes) with siblings</li>
		<li><? createPureFileLink("kropf/save-very-large.usb.gz"); ?> - save very large module (655796 bytes) with siblings</li>
		<li><? createPureFileLink("kropf/eventlog.usb.gz"); ?> - dump eventlog</li>
		<li><? createPureFileLink("kropf/eventlog-with-exception.usb.gz"); ?> - dump eventlog that contains an uncaught exception</li>
		<li><? createPureFileLink("kropf/cleareventlog.usb.gz"); ?> - clear eventlog</li>
		<li><? createPureFileLink("kropf/screenshot.usb.gz"); ?> - save screenshot as bmp</li>
	</ul>

	<ul>
	<p>Full firmware load:</p>
	</ul>

	<ul>
		<li><? createPureFileLink("kropf/8800-os-load-usb.log.bz2"); ?> - (large! about 33MB) USB logs generated by VMWare Fusion as per the instructions in <a href="http://vusb-analyzer.sourceforge.net/tutorial.html">this tutorial</a>.</li>

		<li><? createPureFileLink("kropf/after.info.gz"); ?> - CFP info output before the OS install.</li>
		<li><? createPureFileLink("kropf/before.info.gz"); ?> - CFP info output after the OS install.</li>
	</ul>

	</li>

<li> <p>Robert Yaklin</p>
	<ul>
		<li><? createPureFileLink("yaklin/cfp.hashbootrom.output.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/cfp.hashbootrom.sniff.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/cfp.load.8700-v4.2.1.101_P2.3.0.81.sfi.notes.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/cfp.load.8700-v4.2.1.101_P2.3.0.81.sfi.programOutput.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/cfp.load.8700-v4.2.1.101_P2.3.0.81.sfi.sniff.txt.bz2"); ?> (5MB)</li>
		<li><? createPureFileLink("yaklin/javaloader.erase.net_rim_bb_browser.sniff.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/javaloader.erase.net_rim_bb_search.net_rim_bb_timezones.net_rim_bb_voice.sniff.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/javaloader.force.erase.net_rim_bb_browser.sniff.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/javaloader.force.erase.net_rim_bb_search.net_rim_bb_timezones.net_rim_bb_voice.sniff.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/javaloader.wipe.sniff.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/javaload_multiple_files.txt.bz2"); ?> (large! about 20MB)</li>
		<li><? createPureFileLink("yaklin/1-LoadMFI.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/3-LoadVSM.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/2-RetrieveSomeInfo.txt.bz2"); ?></li>
		<li><? createPureFileLink("yaklin/4-LoadSFI.txt.bz2"); ?></li>
	</ul>
	</li>

</ul>
</p>

