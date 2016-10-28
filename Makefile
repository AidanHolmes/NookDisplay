CXX = g++
CXXFLAGS=-Wall -I../../hardware/
LIBS = -ljpeg
LDFLAGS = 

SRCS_LIB = ../../hardware/displayimage.cpp testapp.cpp testwindow.cpp nookapp.cpp nookwindow.cpp
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
