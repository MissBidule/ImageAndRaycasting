GXX=g++
CXXFLAGS=-Wall -pedantic -std=c++14
DEL=rm -f

# Adaptez ces deux variables avec vos propres chemins s'ils sont différents :
INCPATH_JPEG=
LDPATH_JPEG=

.PHONY: all clean

all: IM_DUT.exe

Image.o : Image.cpp Image.hpp
	$(GXX) -g $(CXXFLAGS) $(INCPATH_JPEG) -c Image.cpp

IM_DUT.exe : main.cpp Image.hpp Image.o
	$(GXX) $(CXXFLAGS) $(INCPATH_JPEG) -g -o IM_DUT.exe.exe main.cpp Image.o $(LDPATH_JPEG) -ljpeg

clean:
	$(DEL) *.o IM_DUT.exe
