cqlock
======
Terminal application that displays current time in the qlocktwo fashion.
It is C implementation of script written for conky
(http://pastebin.com/wK7JmG9H) with some improvements :-). It currently supports
three color schemes for displaying data. Color schemes were inspired by
[Solarized](http://ethanschoonover.com/solarized) color scheme.



Screenshots
===========
Xfce terminal emulator, color scheme: mid-high.
![xfce terminal emulator mid-high](./screenshots/xfce_terminal_emulator_mid-high.png)


![xfce terminal emulator dark-high](./screenshots/xfce_terminal_emulator_dark-high.png)

Xfce terminal emulator, color scheme: dark-high.


![xfce terminal emulator mid-high bkg #002B36](./screenshots/xfce_terminal_emulator_mid-high_with_bkg_002B36.png)

Xfce terminal emulator, color scheme: mid-high with background color set to
\#002b36.


![xfce terminal emulator light-high bkg #FDF6E3](./screenshots/xfce_terminal_emulator_light-high_with_bkg_FDF6E3.png)

Xfce terminal emulator, color scheme: mid-high with background color set to
\#fdf6e3.



Installation
============
Compile with:
```bash
$ RELEASE=true make cqlock
```

install with (as root):
```bash
$ strip --strip-unneeded ./cqlock
$ install -m 755 ./cqlock /usr/bin/
```
