# NOEL MARTÍNEZ POMARES 48771960T
OPTS = -Wall -Wextra -O3 -std=c++0x

all: mcp

mcp: mcp.cc
	g++ ${OPTS} -o mcp mcp.cc 

tar: mcp.cc makefile
	tar cvfz 48771960T.tar.gz mcp.cc makefile

clean: 
	rm -f mcp *.tar.gz