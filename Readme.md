# Dolphin GBA TAS Editor Custom Build

[Based on Dolphin 5.0-16795](https://de.dolphin-emu.org/download/dev/7321802b4bfc6bc2777203d08151570e28baf1f4/)  

[Homepage](https://dolphin-emu.org/) | [Project Site](https://github.com/dolphin-emu/dolphin) | [Buildbot](https://dolphin.ci) | [Forums](https://forums.dolphin-emu.org/) | [Wiki](https://wiki.dolphin-emu.org/) | [Issue Tracker](https://bugs.dolphin-emu.org/projects/emulator/issues) | [Coding Style](https://github.com/dolphin-emu/dolphin/blob/master/Contributing.md) | [Transifex Page](https://www.transifex.com/projects/p/dolphin-emu/)

Dolphin is an emulator for running GameCube and Wii games on Windows,
Linux, macOS, and recent Android devices. It's licensed under the terms
of the GNU General Public License, version 2 or later (GPLv2+).

Please read the [FAQ](https://dolphin-emu.org/docs/faq/) before using Dolphin.

## About this Build

The special feature of this build is the GBA TAS Input Editor, designed for 2 players.
The goal is to create a Co-op TAS of The Legend of Zelda: Four Swords Adventures.
Therefore, most of its functionality is highly tailored (and hardcoded) to very specific needs.
It should also work for a 2P TAS of Final Fantasy: Crystal Chronicles, but the internal memory watch does not apply then of course.

The main reason I published this on github is to comply with the open source laws.
As for the programming aspect, this was done off my limited programming knowledge from back in the day, plus learning Qt.
If you, for some reason, happen to look at the code and question things, then that's probably why.

That being said, I am very proud of this (hopefully) functional build!
Special thanks to dragonbane0, for bearing with all my silly questions and helping me out in manyfold ways.
Also to Clément Gallet, the creator of libTAS, who helped me alot with Qt tableviews.
Plus it really helps to look at others' code, haha.
Lastly, thank you to JosJuice and Trex for helping me out as well!
