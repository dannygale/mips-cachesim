# Makefile
# for ECEN 4593 cache simulator project
# Fall 2010
# Danny Gale, Chris Messick
#
# Revision History:
# 11/1/2010	created	Danny Gale
#

OBJS = main.o input.o cxxcache.o cacheline.o set.o

cachesim : $(OBJS)
	g++ -Wall -o cachesim $(OBJS)

set.o : set.h set.cxx cacheline.h defines.h
	g++ -Wall -c set.cxx

cxxcache.o : cxxcache.cxx cacheline.h set.h defines.h
	g++ -Wall -c cxxcache.cxx

cacheline.o : cacheline.cxx cacheline.h defines.h
	g++ -Wall -c cacheline.cxx

main.o : main.cxx input.cxx cxxcache.h defines.h
	g++ -Wall -c main.cxx

input.o : input.cxx defines.h
	g++ -Wall -c input.cxx

clean :
	rm $(OBJS)
