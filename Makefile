# -*- mode: Makefile; -*-

OSNAME := $(shell uname)

ifeq ($(OSNAME),Darwin)
include ./Makefile.darwin
endif

ifeq ($(OSNAME),Linux)
include ./Makefile.linux
endif

########################################################################

BENCHMARK_CPPFLAGS = -I/usr/local/include
BENCHMARK_LDFLAGS = -L/usr/local/lib

BENCHMARK_LIBS = -lbenchmark

########################################################################

CXXFLAGS = $(MTFLAGS) -O -g -std=c++1z

CPPFLAGS = -I. -I.. $(BENCHMARK_CPPFLAGS)
LDFLAGS  = $(MTFLAGS) $(BENCHMARK_LDFLAGS)

LIBS = $(BENCHMARK_LIBS)

DEPENDDIR = ./.deps
DEPENDFLAGS = -M

SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

TARGETS = test

########################################################################

all: $(TARGETS)

DEPS = $(patsubst %.o,$(DEPENDDIR)/%.d,$(OBJS))
-include $(DEPS)

$(DEPENDDIR)/%.d: %.cc $(DEPENDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(DEPENDFLAGS) $< >$@

$(DEPENDDIR):
	@[ ! -d $(DEPENDDIR) ] && mkdir -p $(DEPENDDIR)

test: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

%: %.cc

.PHONY: clean

clean:
	rm -rf $(OBJS) $(TARGETS) $(DEPENDDIR) *.gcda *.gcno
