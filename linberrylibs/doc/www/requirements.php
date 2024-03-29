<? include ("linberry.inc"); ?>

<? createHeader("linberry - Requirements"); ?>

<? createSubHeader("::::::: Linux ::::::::::::::"); ?>

<? createSubHeader("Charging and the Kernel"); ?>

<p>One of the main features of Linberry is the ability to control the charging
modes of the Blackberry, as well as changing configuration modes on
Pearl-like devices.</p>

<p>In order to achieve proper charging, udev is setup to run the bcharge
program every time you plug in your Blackberry.</p>

<p>Recent kernels have a module called berry_charge, which does similar
things from the kernel level.  These two methods can conflict if both
run at the same time.</p>

<p>Due to this conflict, the binary packages are setup to install
a blacklist file under /etc/modprobe.d, which will disable berry_charge
as long as you have the linberry-util package installed.</p>

<p>If you are not using the binary packages, you can use the sample
blacklist file that comes with the source tarball.</p>




<? createSubHeader("Power and the Kernel"); ?>

<p>Recent kernels also have the ability to put the USB bus and its devices
into suspend mode.  Kernels included in Ubuntu 7.04 and
Fedora 7 have this turned on by default.</p>

<p>When bcharge runs, it successfully changes the Blackberry to use 500mA
(its normal power level for charging), but then the kernel puts the device
into suspend mode.  This can have various undefined effects, such as
the charge icon disappearing on the device, or having your device lose
its charge in an accelerated manner.</p>

<p>Bcharge attempts to work around this by writing to the
control files under <code>/sys/class/usb_device/.../device/power/</code>
to turn autosuspend off.  Depending on your kernel version or kernel
config, these files may not be available, but in most cases at
least one of the needed files are there.</p>

<p>If you continue to experience trouble charging your Blackberry:
<ul>
	<li> consider upgrading your kernel (Ubuntu 7.10 Gutsy
		patches their 2.6.22 kernel to fix this,
		for example) </li>
	<li> recompile your kernel with CONFIG_USB_SUSPEND disabled </li>
</ul>
</p>


<? createSubHeader("Device Ownership and Permissions"); ?>

<p>The Linberry toolset performs all its actions through the /proc and/or
/sysfs filesystems, using the libusb library.  This requires that you
have permissions to write to the USB device files setup by the kernel.</p>

<p>This is handled differently on various systems:</p>

<ul>
	<li>On Debian based systems, there is a group called plugdev, which
		is used to control permissions for pluggable devices.
		When the linberry-util deb package is installed, udev is
		configured to set Blackberry device permissions to
		the plugdev group.  Make sure your user is in the plugdev
		group.</li>
	<li>On Fedora based systems, ownership is controlled by the
		ConsoleKit package.  This changes ownership of pluggable
		devices to the user currently logged into the console,
		on the theory that anyone at the console should have
		control of the devices he plugs in.  No special
		support is needed by Linberry if you have this package
		installed.</li>
</ul>



<? createSubHeader("::::::: OpenBSD ::::::::::::::"); ?>

<? createSubHeader("Uberry Module and Ugen"); ?>

<p>The uberry kernel module conflicts with the ugen interface that
libusb uses to talk to the device.  To work around this, you will
need to boot your kernel with "boot -c" and disable the uberry module.<p>

<p>Suggestions for better ways to work around this conflict are welcome.</p>

