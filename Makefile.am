bin_PROGRAMS = main

AM_CPPFLAGS= \
			 -Wall \
			 -Iinclude \
			 -Iinclude/xlibimp \
			 $(ZWM_INCLUDES) \
			 -std=c++11

CXXFLAGS=-g3 -O0

main_SOURCES = \
	src/keymap.cpp \
	src/helper.cpp \
	src/xlibimp/xlibwindow.cpp \
	src/xlibimp/xlibstatuswindow.cpp \
	src/xlibimp/xlibdesktop.cpp \
	src/main.cpp
