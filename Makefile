CC=g++ -std=c++11
CFLAGS=-O0 -g
LIBS=-lX11
INCLUDE=-Iinclude -Iinclude/xlibimp

all: init \
	 main

init:
	@echo "Creating the building environment"
	@-rm -fr build
	@mkdir build

main: keymap.o helper.o xlibpp.o xlibwindow.o
	$(CC) $(CFLAGS) $(INCLUDE) src/main.cpp \
		build/xlibpp.o build/helper.o build/keymap.o \
		build/xlibwindow.o \
		$(LIBS) -o main

xlibpp.o: src/xlibpp.cpp include/xlibpp.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/xlibpp.cpp -o build/xlibpp.o

helper.o: src/helper.cpp include/helper.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/helper.cpp -o build/helper.o

keymap.o: src/keymap.cpp include/keymap.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/keymap.cpp -o build/keymap.o

xlibwindow.o: src/xlibimp/xlibwindow.cpp include/xlibimp/xlibwindow.h include/iwindow.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/xlibimp/xlibwindow.cpp -o build/xlibwindow.o

clean:
	@echo "Cleaning binaries"
	@rm -fr build
	@-rm main

deploy: clean all
	#Xephyr -screen 800x600 -br :1
	xinit ./main -- :1

valgrind:
	valgrind --tool=memcheck --leak-check=full  \
		--track-origins=yes \
		--show-reachable=yes xinit ./main -- :1
