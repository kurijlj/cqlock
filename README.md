cqlock
======

Terminal application that displays current time in the qlocktwo fashion.
It is C implementation of script written for conky
(http://pastebin.com/wK7JmG9H) with some improvements :-). It currently supports
three color schemes for displaying data. Color schemes were inspired by
[Solarized](http://ethanschoonover.com/solarized) color scheme.


Screenshots
===========
![xfce terminal emulator mid-high](./xfce_terminal_emulator_mid-high.png)
Xfce terminal emulator, color scheme: mid-high.

![xfce terminal emulator dark-high](./xfce_terminal_emulator_dark-high.png)
Xfce terminal emulator, color scheme: dark-high.

![xfce terminal emulator mid-high bkg #002B36](./xfce_terminal_emulator_mid-high_with_bkg_#002B36.png)
Xfce terminal emulator, color scheme: mid-high with background color set to
\#002B36.

![xfce terminal emulator light-high bkg #FDF6E3](./xfce_terminal_emulator_light-high_with_bkg_#FDF6E3.png)
Xfce terminal emulator, color scheme: mid-high with background color set to
\#FDF6E3.


###### Notes

Compile with:
```bash
$ RELEASE=true make cqlock
```

install with (as root):
```bash
$ strip --strip-unneeded ./cqlock
$ install -m 755 ./cqlock /usr/bin/
```
