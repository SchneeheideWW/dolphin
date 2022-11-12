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

## The Editor
![Editor](https://user-images.githubusercontent.com/118022732/201481675-28205678-b506-40f1-9f3d-6d2aa597af99.jpg)
* Editor Functionality
  * Right-click to set/unset inputs in current column
  * Hold CTRL while dragging to toggle every 2nd input in current column
  * Select cells with left-click
  * DEL to delete selected cells' input
  * R to insert a perfect roll if a cell in the R column is selcted
  * R to insert a full FMC sequence if B,L and R cells are selected (1 cell per column)
  * CTRL + C to copy selected cells' inputs
  * CTRL + V to paste inputs
  * Savestate Layer value to shift the corresponding slots of F1 to F9 by 10 each layer
  * F11 decrements the layer; F12 increments it (the layer feature requires the Editor to have focus)  

The editor comes with built-in hotkeys for Frame Advance (F) and your typical saving&loading states

## The Concept
It is worth noting that the editor does not use the dtm format.  
Actually, it uses plain text files filled with 0 and 1.  
Each line contains 20 zeros, and any set input changes the 0 to 1.  
Since there is no dtm involved, we do not utilize the recording mode at all.  
Instead, we use a savestate created after boot that functions as anchor to guarantee determinism.  
Optionally, you could also create a short recording with empty inputs with the sole purpose to recreate the same starting conditions.  
