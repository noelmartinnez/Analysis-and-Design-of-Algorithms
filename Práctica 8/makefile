# NOEL MARTÍNEZ POMARES
OPTS = -Wall -Wextra -O3 -std=c++0x

all: mcp_bt

mcp_bt: mcp_bt.cc
	g++ ${OPTS} -o mcp_bt mcp_bt.cc 

tar: mcp_bt.cc makefile
	tar cvfz 48771960T.tar.gz mcp_bt.cc makefile

clean: 
	rm -f mcp_bt *.tar.gz