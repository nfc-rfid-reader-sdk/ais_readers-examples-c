####################################################################
#makefile for build tester
#srkos 
####################################################################
$(VERBOSE).SILENT:
####################################################################
LIBVER = testing/ver.4.9.2
####################################################################

help : info_ver
	echo "* For compile tester on Linux,   type : make linux   or make lin"
	echo "* For compile tester on Windows, type : make windows or make win"
	echo "* For compile tester on OS X,    type : make osx"

OUTDIR = Out
LIBPATH = lib/$(LIBVER)

info_ver :
	echo --------------------------------------------------------------------
	echo Compile tester for AIS READERS library : $(LIBVER)
	echo --------------------------------------------------------------------

test_folder:
	-mkdir -p $(OUTDIR)/windows/x86
	-mkdir -p $(OUTDIR)/linux/x86
	-mkdir -p $(OUTDIR)/linux/arm
	-mkdir -p $(OUTDIR)/linux/arm-hf
	-mkdir -p $(OUTDIR)/osx/x86

rm_obj :
	-rm -f src/*.o

pre_test : test_folder rm_obj

win windows : info_ver
	make win32
	make win64
	make win_arm
	make win_armhf
	make rm_obj

lin linux : info_ver
	make lin32
	make lin64
	make rm_obj

osx : info_ver osx_ rm_obj

####################################################################

osx_ : pre_test
	gcc -arch i386 -arch x86_64 -o $(OUTDIR)/osx/x86/ais_readers_lib_tester -I$(LIBPATH)/include -L$(LIBPATH)/osx/x86 -D__linux__=2 src/*.c -lais_readers
	echo Tester for Intel OSX is created in $(OUTDIR)/osx/x86

lin32 : pre_test
	gcc -m32 -o $(OUTDIR)/linux/x86/ais_readers_lib_tester-x86 -I$(LIBPATH)/include -L$(LIBPATH)/linux/x86 src/*.c -lais_readers-x86 # -static-libgcc -static-libstdc++
	echo Tester for 32 bit Linux is created in $(OUTDIR)/linux/x86

lin64 : pre_test
	gcc -m64 -o $(OUTDIR)/linux/x86/ais_readers_lib_tester-x86_64 -I$(LIBPATH)/include -L$(LIBPATH)/linux/x86 src/*.c -lais_readers-x86_64 # -static-libgcc -static-libstdc++
	echo Tester for 64 bit Linux is created in $(OUTDIR)/linux/x86

#echo Warning: resolving _AIS_List_AddDeviceForCheck@8 by linking to _AIS_List_AddDeviceForCheck

win32 : pre_test
	cd src && i686-w64-mingw32-gcc -I../$(LIBPATH)/include -O3 -g0 -Wall -c -fmessage-length=0 *.c
	i686-w64-mingw32-gcc -o $(OUTDIR)/windows/x86/ais_readers_lib_tester-x86.exe -Wl,--enable-stdcall-fixup -L$(LIBPATH)/windows/x86 src/*.o -lais_readers-x86 -static-libgcc -static-libstdc++
	echo Tester for 32 bit Windows is created in $(OUTDIR)/windows/x86

win64 : pre_test
#	cd src && x86_64-w64-mingw32-gcc -o win64/ais_readers_lib_tester-x86_64.exe -Iinclude -L$(LIBPATH)/windows/x86 src/*.c -lais_readers-x86_64 -static-libgcc -static-libstdc++
	cd src && x86_64-w64-mingw32-gcc -I../$(LIBPATH)/include -O3 -g0 -Wall -c -fmessage-length=0 *.c
	x86_64-w64-mingw32-gcc -o $(OUTDIR)/windows/x86/ais_readers_lib_tester-x86_64.exe -L$(LIBPATH)/windows/x86 src/*.o -lais_readers-x86_64 -static-libgcc -static-libstdc++
	echo Tester for 64 bit Windows is created in $(OUTDIR)/windows/x86

####################################################################

win_arm : pre_test
	arm-angstrom-linux-gnueabi-gcc -o $(OUTDIR)/linux/arm/ais_readers_lib_tester-arm -I$(LIBPATH)/include -L$(LIBPATH)/linux/arm src/*.c -lais_readers-arm # -static-libgcc -static-libstdc++
	echo Tester for ARM is created in $(OUTDIR)/linux/arm

win_armhf : pre_test
	arm-linux-gnueabihf-gcc -o $(OUTDIR)/linux/arm-hf/ais_readers_lib_tester-armhf -I$(LIBPATH)/include -L$(LIBPATH)/linux/arm-hf src/*.c -lais_readers-armhf # -static-libgcc -static-libstdc++
	echo Tester for ARM-HF is created in $(OUTDIR)/linux/arm-hf
