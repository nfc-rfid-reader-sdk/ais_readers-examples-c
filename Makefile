####################################################################
#makefile for build tester
#srkos 
####################################################################
$(VERBOSE).SILENT:
####################################################################

info : 
	echo Use: make lin or make win or make osx

OUTDIR = Out

test_folder:
	-mkdir -p $(OUTDIR)/windows/x86
	-mkdir -p $(OUTDIR)/linux/x86
	-mkdir -p $(OUTDIR)/linux/arm
	-mkdir -p $(OUTDIR)/linux/arm-hf
	-mkdir -p $(OUTDIR)/osx/x86

pre_test : test_folder
	-rm -f src/*.o

win windows :
	make win32
	make win64
	make win_arm
	make win_armhf

lin linux :
	make lin32
	make lin64

osx : osx_

####################################################################

osx_ : pre_test
	gcc -arch i386 -arch x86_64 -o $(OUTDIR)/osx/x86/ais_readers_lib_tester -Iinclude -Llib/osx/x86 -D__linux__=2 src/*.c -lais_readers
	echo Tester for Intel OSX is created in $(OUTDIR)/osx/x86

lin32 : pre_test
	gcc -m32 -o $(OUTDIR)/linux/x86/ais_readers_lib_tester-x86 -Ilib/include -Llib/linux/x86 src/*.c -lais_readers-x86 # -static-libgcc -static-libstdc++
	echo Tester for 32 bit Linux is created in $(OUTDIR)/linux/x86

lin64 : pre_test
	gcc -m64 -o $(OUTDIR)/linux/x86/ais_readers_lib_tester-x86_64 -Ilib/include -Llib/linux/x86 src/*.c -lais_readers-x86_64 # -static-libgcc -static-libstdc++
	echo Tester for 64 bit Linux is created in $(OUTDIR)/linux/x86

#echo Warning: resolving _AIS_List_AddDeviceForCheck@8 by linking to _AIS_List_AddDeviceForCheck

win32 : pre_test
	cd src && i686-w64-mingw32-gcc -I../lib/include -O3 -g0 -Wall -c -fmessage-length=0 *.c
	i686-w64-mingw32-gcc -o $(OUTDIR)/windows/x86/ais_readers_lib_tester-x86.exe -Wl,--enable-stdcall-fixup -Llib/windows/x86 src/*.o -lais_readers-x86 -static-libgcc -static-libstdc++
	echo Tester for 32 bit Windows is created in $(OUTDIR)/windows/x86

win64 : pre_test
#	cd src && x86_64-w64-mingw32-gcc -o win64/ais_readers_lib_tester-x86_64.exe -Iinclude -Llib/windows/x86 src/*.c -lais_readers-x86_64 -static-libgcc -static-libstdc++
	cd src && x86_64-w64-mingw32-gcc -I../lib/include -O3 -g0 -Wall -c -fmessage-length=0 *.c
	x86_64-w64-mingw32-gcc -o $(OUTDIR)/windows/x86/ais_readers_lib_tester-x86_64.exe -Llib/windows/x86 src/*.o -lais_readers-x86_64 -static-libgcc -static-libstdc++
	echo Tester for 64 bit Windows is created in $(OUTDIR)/windows/x86

####################################################################

win_arm : pre_test
	arm-angstrom-linux-gnueabi-gcc -o $(OUTDIR)/linux/arm/ais_readers_lib_tester-arm -Ilib/include -Llib/linux/arm src/*.c -lais_readers-arm # -static-libgcc -static-libstdc++
	echo Tester for ARM is created in $(OUTDIR)/linux/arm

win_armhf : pre_test
	arm-linux-gnueabihf-gcc -o $(OUTDIR)/linux/arm-hf/ais_readers_lib_tester-armhf -Ilib/include -Llib/linux/arm-hf src/*.c -lais_readers-armhf # -static-libgcc -static-libstdc++
	echo Tester for ARM-HF is created in $(OUTDIR)/linux/arm-hf
