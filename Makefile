.DEFAULT_GOAL := all

CXX = g++

INCLUDES = -Iinclude

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 $(INCLUDES) 

EXAMPLESRCS = $(wildcard examples/*.cpp)

TESTSSRCS = $(wildcard tests/*.cpp)

LIBSRCS = $(wildcard src/*.cpp)

SRCS = $(sort $(LIBSRCS) $(EXAMPLESRCS) $(TESTSSRCS))

LIBOBJS = $(LIBSRCS:.cpp=.o)

OBJS = $(SRCS:.cpp=.o)

LIBS =

EXAMPLETARGETS = $(EXAMPLESRCS:.cpp= )

TESTSTARGETS = $(TESTSSRCS:.cpp= )

#LIBTARGET = SunwayMR.so
#
#$(LIBTARGET) : $(LIBOBJS)
#	$(CXX) $(LIBS) $(LIBOBJS) -o $(LIBTARGET)

$(EXAMPLETARGETS):
	$(CXX) $(CXXFLAGS) $(LIBS) $(addsuffix .o,$@) -o $@

$(TESTSTARGETS): 
	$(CXX) $(CXXFLAGS) $(LIBS) $(addsuffix .o,$@) -o $@

TARGETS =	$(OBJS) $(LIBTARGET) $(EXAMPLETARGETS) $(TESTSTARGETS)

all:	$(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean