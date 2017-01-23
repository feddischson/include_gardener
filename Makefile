CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -fPIC -Iinc -std=c++11
LDFLAGS=-g 
LDLIBS= 

SRCS=src/_template_.cpp \
	 src/main.cpp


OBJS=$(subst .cpp,.o,$(SRCS))

all: include_gardener 

include_gardener: $(OBJS) inc/*.h
	$(CXX) $(LDFLAGS) -o include_gardener $(OBJS) 

clean:
	$(RM) $(OBJS) *.a *.so *_wrap.cxx *.o

distclean: clean
	$(RM) include_gardener 

