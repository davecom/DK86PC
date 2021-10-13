# DK86PC
This is a work-in-progress Intel 8086 and IBM PC 5150 emulator. Currently, it successfully runs some programs.

It's at the point where it gets through booting the BIOS and gets to the IBM Casette BASIC (I haven't made much progress on the floppy disk controller to boot DOS). It can successfully run some code in BASIC:

![Casette BASIC Booting](CasetteBASIC.png)

If someone wants to help me finish it, I'd be happy for the help. Right now most of the 8086 (8088 technically but no difference at this point) is done, memory works, CGA has text mode support, and a basic implementation of most of the essential support chips is done (PIC, PIT, etc.). The only dependencies are SDL and SDL_TTF so it can easily be ported to other platforms.

There is an Xcode project file in the main directory, and a Visual Studio solution in the `Visual Studio` directory. I've left the SDL libraries in the repository, since I had to custom build the ones for Apple Silicon (and I just find it more convenient).

Currently I am testing on Big Sur on Apple Silicon (ARM64), and on Windows 10 on x64.

I've run some automated CPU tests and the CPU is reasonably good, but is still missing a few instructions. I wrote it all by hand using Intel/IBM/online documentation, only looking at other emulators when I got really stuck for a particular specific item.

The emulators I've found most helpful as a reference is [this Java one](https://github.com/NeatMonster/Intel8086) by @NeatMonster and [this C one](https://github.com/mikechambers84/XTulator) by @mikechambers84.
