# DK86PC
This is a work-in-progress Intel 8086 and IBM PC 5150 emulator. Currently, it successfully boots through the BIOS and launches Cassette BASIC. It can run commands in Cassette BASIC successfully:

![Casette BASIC Booting](CasetteBASIC.png)

The 8086 implementation passes automated CPU Tests (Thanks @artlav!). Many of the support chips have partial implementations. The next step is to finish enough of the floppy disk controller to be able to boot into DOS. Then I'd like to implement a CGA graphics mode (currently only text mode is implemented) to be able to run some '80s games. I'm happy to take pull requests towards those goals or fixing bugs, but I'm not really interested in turning this into a sophisticated (say 80386) emulator for more advanced hardware. It's a learning project. I could particularly use help with the support chips.

The only dependencies are SDL and SDL-ttf.

There is an Xcode project file in the main directory, and a Visual Studio solution in the `Visual Studio` directory. I've left the SDL libraries in the repository, since I had to custom build the ones for Apple Silicon (and I just find it more convenient).

Currently I am testing on Big Sur on Apple Silicon (ARM64), and on Windows 10 on x64. I'm using the clang and MSVC compilers.

I wrote it all by hand using Intel/IBM/online documentation, looking at other emulators for hints/quick solutions when I got really stuck on a particular item.

The emulators I've found most helpful as a reference are [this Java one](https://github.com/NeatMonster/Intel8086) by @NeatMonster, [this C one](https://github.com/mikechambers84/XTulator) by @mikechambers84, [this C one](https://github.com/andreas-jonsson/virtualxt) by @andreas-jonsson, and [this Rust one](https://github.com/dbalsom/marty) by @dbalsom.

For the CPU opcode implementations, I largely used the official Intel 8086 manual as well as this [great site by Felix Cloutier](https://www.felixcloutier.com/x86/index.html).