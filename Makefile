osx :
	gcc -arch i386 -arch x86_64 -o osx/ais_readers_lib_tester -Llib/osx/x86 -Iinclude -D__linux__=2 src/*.c -lais_readers


