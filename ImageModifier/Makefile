GXX=g++
CXXFLAGS=-Wall -pedantic -std=c++14
DEL=rm

# Adaptez ces deux variables avec vos propres chemins s'ils sont différents :
INCPATH_JPEG=-I/opt/homebrew/opt/jpeg/include
LDPATH_JPEG=-L/opt/homebrew/opt/jpeg/lib

SOURCES=main.cpp Utils.cpp Color.cpp ColorImage.cpp GreyImage.cpp Primitive.cpp Circle.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HEADERS=Image.hpp Utils.hpp Color.hpp ColorImage.hpp GreyImage.hpp Primitive.hpp Circle.hpp

.PHONY: all clean

all: IM_DUT.exe

%.o : %.cpp $(HEADERS)
	$(GXX) -g $(CXXFLAGS) $(INCPATH_JPEG) -c $<
	
main.o : main.cpp $(HEADERS)
	$(GXX) -g $(CXXFLAGS) $(INCPATH_JPEG) -c $<

IM_DUT.exe : $(OBJECTS)
	$(GXX) $(CXXFLAGS) $(INCPATH_JPEG) -g -o $@ $^ $(LDPATH_JPEG) -ljpeg

clean:
	$(DEL) *.o
	$(DEL) IM_DUT.exe
