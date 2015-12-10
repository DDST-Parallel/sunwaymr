.DEFAULT_GOAL := all

CXX = gcc

INCLUDES = -Iheaders -Iinclude

CXXFLAGS = -O2 -g -Wall -fmessage-length=0

LDFLAGS = -shared -fPIC

EXAMPLESRCS = $(wildcard examples/*.cpp)

TESTSSRCS = $(wildcard tests/*.cpp)

LIBHEADERS = $(wildcard headers/*.h)

LIBINCLUDES = $(wildcard include/*.hpp)

SRCS = $(LIBSRCS) $(EXAMPLESRCS) $(TESTSSRCS)

LIBOBJS = $(LIBSRCS:.cpp=.o)

OBJS = $(SRCS:.cpp=.o)

LIBS = -pthread -lstdc++ -lm

EXAMPLETARGETS = $(EXAMPLESRCS:.cpp= )

TESTSTARGETS = $(TESTSSRCS:.cpp= )

LIBNAME = sunwaymr

LIBTARGET = lib$(LIBNAME).so

LDSONAME = -Wl,-soname=$(LIBTARGET)

$(EXAMPLETARGETS), $(TESTSTARGETS): $(LIBHEADERS) $(LIBINCLUDES)
	$(CXX) $(CXXFLAGS) $(addsuffix .cpp,$@) -o $@ $(INCLUDES) $(LIBS)

TARGETS = $(EXAMPLETARGETS) $(TESTSTARGETS)

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
