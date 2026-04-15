# hrishikos

**Information:**
 - an operating system targeting x86_64 architecture and legacy hardware devices
 - written with assembly and C, using a Make build system
 - currently contains a basic shell
 - TODO: file system, process scheduling
<br></br>

**Requirements:**
 - QEMU package for emulation
 - GDB (not needed if not using debugger)

**Build/Run Instructions:**
 - to clean the build: `make clean`
 - to build the disk image: `make`
 - to emulate the disk image: `make qemu-no-debug`
 - to emulate and debug the disk image: `make qemu`
<br></br>

**Credits:**
 - bootloader and early build setup from [Operating Systems: From 0 to 1](https://raw.githubusercontent.com/tuhdo/os01/master/Operating_Systems_From_0_to_1.pdf)
 - physical memory management from [BrokenThorn Entertainment](https://brokenthorn.com/Resources/OSDevIndex.html)
