# First makefile for linux only
#

EXE = DirViewer
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
SOURCES += libs/imgui/imgui.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui_widgets.cpp libs/imgui/imgui_demo.cpp

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CXXFLAGS = -Ilibs/imgui/ -Iheaders/ -g -Wall -Wformat -std=c++17
LIBS = 

# Using OpenGL loader gl3w

SOURCES += libs/gl3w/GL/gl3w.c
CXXFLAGS += -Ilibs/gl3w

# Linux specific make flags

LIBS += -lGL `pkg-config --static --libs glfw3`
CXXFLAGS += `pkg-config --cflags glfw3`
CFLAGS = $(CXXFLAGS)

# Rules
%.o:./src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:./libs/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:./libs/gl3w/GL/%.c
	$(CXX) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for Linux

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -rf $(EXE) $(OBJS)

.PHONY: clean
