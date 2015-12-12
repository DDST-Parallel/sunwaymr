.DEFAULT_GOAL := all

CXX = gcc

INCLUDES = -Itools -Iheaders -Iinclude

CXXFLAGS = -O2 -g -Wall -fmessage-length=0

LDFLAGS = -shared -fPIC

APPSRCS = $(wildcard *.cpp)

EXAMPLESRCS = $(wildcard examples/*.cpp)

TESTSSRCS = $(wildcard tests/*.cpp)

LIBHEADERS = $(wildcard headers/*.h)

LIBINCLUDES = $(wildcard include/*.hpp)

SRCS = $(LIBINCLUDES) $(EXAMPLESRCS) $(TESTSSRCS)

LIBOBJS = $(LIBINCLUDES:.hpp=.o)

OBJS = $(LIBINCLUDES:.hpp=.o)

LIBS = -pthread -lstdc++ -lm

EXAMPLETARGETS = $(EXAMPLESRCS:.cpp= )

TESTSTARGETS = $(TESTSSRCS:.cpp= )

APPTARGETS = $(APPSRCS:.cpp= )

LIBNAME = sunwaymr

LIBTARGET = lib$(LIBNAME).so

LDSONAME = -Wl,-soname=$(LIBTARGET)

$(APPTARGETS) $(EXAMPLETARGETS) $(TESTSTARGETS): $(LIBHEADERS) $(LIBINCLUDES)
	$(CXX) $(CXXFLAGS) $(addsuffix .cpp,$@) -o $@ $(INCLUDES) $(LIBS)

#$(LIBOBJS):
#	$(CXX) $(CXXFLAGS) -c $(patsubst %.o,%.hpp, $@) -o $@ $(INCLUDES) $(LIBS)

TARGETS = $(APPTARGETS) $(EXAMPLETARGETS)

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean app
