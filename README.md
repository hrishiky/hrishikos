# hrishikos

**Information:**
 - an x86 operating system written from scratch with assembly and C
 - currently contains a basic shell
 - memory management, file system, and process scheduling will be implemented in the future
<br></br>

**Requirements:**
 - QEMU package for emulation
 - GDB for debugging

**Build/Run Instructions:**
 - to clean the build: `make clean`
 - to build the disk image: `make`
 - to emulate the disk image: `make qemu-no-debug`
 - to emulate and debug the disk image: `make qemu`
<br></br>

**Credits:**
 - bootloader and early build setup from [Operating Systems: From 0 to 1](https://raw.githubusercontent.com/tuhdo/os01/master/Operating_Systems_From_0_to_1.pdf)

