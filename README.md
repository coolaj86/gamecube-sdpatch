# gamecube-sdpatch

[sdload.zip](https://www.wiibrew.org/wiki/Running_GameCube_homebrew_on_the_Wii) [(archive.org alternative)](https://web.archive.org/web/*/http://wii-news.dcemu.co.uk/sdload.zip)

-   SDpatch.dol
-   readme.txt
-   sdpatch.exe
-   SDLOADER.BIN
-   install.txt
-   SDIPDATA.CFG
-   testdemo.dol
-   src/
    -   SDpatch.cpp
    -   StdAfx.cpp
    -   StdAfx.h

# sdpatch.c

sdpatch.c - Patch a FAT16 SD Card for use with GameCube Console

Taken from The Web Archive snapshots of [http://www.redfelineninja.dsl.pipex.com/software/sdpatch.c](https://web.archive.org/web/20080601000000*/http://www.redfelineninja.dsl.pipex.com/software/sdpatch.c):

-   [2007 May 4 13:21:15](https://web.archive.org/web/20070504132115/http://www.redfelineninja.dsl.pipex.com/software/sdpatch.c)
-   [2008 Dec 28 15:28:37](https://web.archive.org/web/20081228152837/http://www.redfelineninja.dsl.pipex.com/software/sdpatch.c)

How to compile on Linux:

```bash
git clone https://github.com/solderjs/gamecube-sdpatch.git

pushd gamecube-sdpatch/

gcc -Wall sdpatch.c -o sdpatch
```

How to create an SD Card:

```bash
mkdosfs -F16 -nSDLoad /dev/sdcard
./sdpatch /dev/sdcard
mkdir -p /media/sdcard
mount -t vfat /dev/sdcard /media/sdcard
cp SDLOADER.BIN /media/sdcard
```
