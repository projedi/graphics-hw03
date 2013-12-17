CXX=clang++
CXXFLAGS=-g -Wall -std=c++11
#CXXFLAGS=-O2 -std=c++11

LIBS=-lGL -lGLEW -lglfw -lfreeimageplus -lAntTweakBar

executable = hw03
sources = main.cpp gl_context.cpp camera.cpp model.cpp controller.cpp objfile.cpp sphere.cpp
objects = $(sources:.cpp=.o)
depends = $(sources:.cpp=.d)

all: $(executable)

include $(depends)

$(executable): $(objects)
	$(CXX) $^ -o $@ $(LIBS)

%.o: %.cpp %.d
	$(CXX) $(CXXFLAGS) $< -c

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MF"$@" -MG -MM -MP -MT"$(<:%.cpp=%.o)" "$<"

clean:
	rm -f $(executable) $(objects) $(depends)
