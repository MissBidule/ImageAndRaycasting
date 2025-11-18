GXX=clang++
CC=clang
CXXFLAGS=-Wall -pedantic -std=c++14 -O3 -ffast-math -Xpreprocessor -fopenmp
CPPFLAGS = -g $(INCPATH_JPEG)
LDLIBS=-lomp -ljpeg -lpthread
DEL=rm

# Adaptez ces deux variables avec vos propres chemins s'ils sont différents :
INCPATH_JPEG=-I/opt/homebrew/opt/jpeg/include -I/opt/homebrew/Cellar/libomp/21.1.5/include
LDPATH_JPEG=-L/opt/homebrew/opt/jpeg/lib -L/opt/homebrew/Cellar/libomp/21.1.5/lib

SOURCES=main.cpp Utils.cpp Color.cpp ColorImage.cpp GreyImage.cpp Primitive.cpp Circle.cpp Plane.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HEADERS=Image.hpp Utils.hpp Color.hpp ColorImage.hpp GreyImage.hpp Primitive.hpp Circle.hpp Misc.hpp Plane.hpp

.PHONY: all clean

all: IM_DUT.exe

%.o : %.cpp $(HEADERS)
	$(GXX) $(CXXFLAGS) $(CPPFLAGS) -c $<
	
main.o : main.cpp $(HEADERS)
	$(GXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

IM_DUT.exe : $(OBJECTS)
	$(GXX) -g -o $@ $^ $(LDPATH_JPEG) $(LDLIBS)

clean:
	$(DEL) *.o
	$(DEL) IM_DUT.exe
