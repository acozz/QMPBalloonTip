# QMPBalloonTip
Balloon Notification plug-in for the defunct Quintessential Media Player. It was written for Windows XP with the latest release (version 0.3) on July 13, 2005. 

# Compilation and Linking
Compilers have become better and more stringent, so this project does not compile in its current state when targeting modern platforms. The solution has been set up to target the Windows XP Platform Toolset (Visual Studio 2017 - Windows XP (v141_xp)) using Visual Studio 2019. MFC support is also required (for resources).

See https://web.archive.org/web/20210315184252/https://docs.microsoft.com/en-us/cpp/build/configuring-programs-for-windows-xp?view=msvc-160 for information on targeting Windows XP.

Unfortunately, this support for Windows XP is deprecated, so it may not be possible to compile in the future without many changes.

# Original Description
https://web.archive.org/web/20101220021517/http://quinnware.com/list_plugins.php?plugin=148
![QMPBalloonTip example](https://web.archive.org/web/20101219135914im_/http://quinnware.com/img/plugin_screens/BalloonNotification.gif)

Displays current track information on track change in a balloon from the tray icon. Works on Windows 2000/XP and above.

Track info to display is customizable in the plugin's options.

New for version 0.3: Corrected behavior for CDs and streams. Disabled the popup balloon noise on XP.

New for version 0.2: You can now set the number of milliseconds before the balloon disappears. Check the configuration.
