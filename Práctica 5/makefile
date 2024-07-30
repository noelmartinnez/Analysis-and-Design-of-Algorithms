# NOEL MARTÍNEZ POMARES 48771960T
OPTS = -Wall -Wextra -O3 -std=c++0x

all: pow2 pow2.Msteps graphs

pow2: pow2.cc
	g++ ${OPTS} -o pow2 pow2.cc 

pow2.Msteps: pow2
	./pow2 > pow2.Msteps

graphs: pow2.Msteps pow2.gpi
	gnuplot pow2.gpi

tar: pow2.cc pow2.gpi makefile
	tar cvfz 48771960T.tar.gz pow2.cc pow2.gpi makefile

clean: 
	rm -f pow2 pow2.Msteps pow2.png *.tar.gz