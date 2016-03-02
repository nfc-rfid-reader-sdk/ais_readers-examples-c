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

win :
	make win32
	make win64

lin :
	make lin32
	make lin64

osx : pre_test
	gcc -arch i386 -arch x86_64 -o osx/ais_readers_lib_tester -Iinclude -Llib/osx/x86 -D__linux__=2 src/*.c -lais_readers

lin32 : pre_test
	gcc -m32 -o Out/linux/x86/ais_readers_lib_tester-x86 -Ilib/include -Llib/linux/x86 src/*.c -lais_readers-x86 # -static-libgcc -static-libstdc++

lin64 : pre_test
	gcc -m64 -o Out/linux/x86/ais_readers_lib_tester-x86_64 -Ilib/include -Llib/linux/x86 src/*.c -lais_readers-x86_64 # -static-libgcc -static-libstdc++

win32 : pre_test
	cd src && gcc -I../lib/include -O3 -g0 -Wall -c -fmessage-length=0 *.c
	gcc -o $(OUTDIR)/windows/x86/ais_readers_lib_tester-x86.exe -Llib/windows/x86 src/*.o -lais_readers-x86

win64 : pre_test
	gcc -o win64/ais_readers_lib_tester-x86_64.exe -Iinclude -Llib/windows/x86 src/*.c -lais_readers-x86_64
