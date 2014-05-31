CC=g++ -std=c++11
CFLAGS=-O0 -g
LIBS=-lX11 $(shell freetype-config --libs) -lXft
INCLUDE=-Iinclude -Iinclude/xlibimp $(shell freetype-config --cflags)

all: init \
	 main

init:
	@echo "Creating the building environment"
	@-rm -fr build
	@mkdir build

main: keymap.o helper.o xlibdesktop.o xlibwindow.o
	$(CC) $(CFLAGS) $(INCLUDE) src/main.cpp \
		build/xlibdesktop.o build/helper.o build/keymap.o \
		build/xlibwindow.o \
		$(LIBS) -o main

xlibdesktop.o: src/xlibimp/xlibdesktop.cpp include/xlibimp/xlibdesktop.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/xlibimp/xlibdesktop.cpp -o build/xlibdesktop.o

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
	DISPLAY=:1 valgrind --tool=memcheck --leak-check=full  \
		--track-origins=yes --suppressions=supp/xlib.supp \
		--suppressions=supp/libxft.supp \
		--show-reachable=yes ./main
		#--gen-suppressions=all --log-file=libxft.supp \
		#--show-reachable=yes xinit ./main -- :1

todo:
	find . \( -name \*.h -o -name \*.cpp \) -exec grep TODO {} + \
		| sed "s/^\(.\+.[cpp|h]:\).*\(TODO.*\)$$/\1 \2/" > TODO.list
