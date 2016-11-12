CXX = g++
CXXFLAGS=-Wall -I../graphicslib/
LIBS = -ljpeg
LDFLAGS = 

SRCS_LIB = ../graphicslib/displayimage.cpp testapp.cpp testwindow.cpp nookapp.cpp nookwindow.cpp nookinput.cpp nookfont.cpp nookbutton.cpp nookmessagebox.cpp
H_LIB = $(SRCS_LIB:.cpp=.hpp)
OBJS_LIB = $(SRCS_LIB:.cpp=.o)

EXECUTABLE = nookwnd

.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS_LIB)
	$(CXX) $(LDFLAGS) $(OBJS_LIB) $(OBJS_OLED) $(LIBS) -o $@

$(OBJS_LIB): $(H_LIB)

.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLE)
