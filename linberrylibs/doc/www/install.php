<? include ("linberry.inc"); ?>

<? createHeader("Installing Linberry Via Binary Packages"); ?>

<? createSubHeader("Dependencies"); ?>

<p>See the <? createLink("dependencies", "software dependencies"); ?> page for
detailed information on the software that Linberry needs.</p>


<? createSubHeader("Binary Packages"); ?>

<p>Download the matching packages for your system from the
<a href="http://sourceforge.net/project/showfiles.php?group_id=153722">Sourceforge download page</a>.</p>

<p>On Sourceforge, packages are available for multiple distros, and are
indicated by the filenames:
<ul>
	<li><b>ubuntu710</b> - Ubuntu Gutsy Gibbon, 7.10</li>
	<li><b>ubuntu804</b> - Ubuntu Hardy Heron, 8.04</li>
	<li><b>ubuntu904</b> - Ubuntu Jaunty, 9.04</li>
	<li><b>fc7</b> - Fedora Core 7</li>
	<li><b>fc8</b> - Fedora Core 8</li>
	<li><b>fc9</b> - Fedora Core 9</li>
	<li><b>f11</b> - Fedora Core 11</li>
	<li>the non-tagged deb files are for Debian Stable 5.0, Lenny</li>
</ul>
</p>

<p>Linberry is split up into multiple binary packages.  For example,
if you want the GUI backup program, you will also need the Linberry library.
For non-development systems, you will need:
<ul>
	<li>liblinberry0</li>
	<li>linberry-util</li>
	<li>linberrybackup-gui</li>
	<li>linberry-opensync (libopensync-plugin-linberry on Debian systems)</li>
</ul>
</p>

<p>For development systems, you will need the following additional
packages:
<ul>
	<li>liblinberry-dev</li>
</ul>
</p>

<p>Note that there are only binary packages available for opensync 0.22,
since opensync 0.4x is not yet officially released.</p>

<p>You will also notice on the above Sourceforge download site, that there
is a separate section for debug packages.  These packages are only
necessary if you run into a bug that causes one of the above programs
to crash, and you wish to help developers in tracking down the error.</p>

