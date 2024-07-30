// NOEL MARTÍNEZ POMARES 48771960T

#include <iostream>
#include <cmath>
using namespace std;

unsigned long STEPS1 = 0, STEPS2 = 0, STEPS3 = 0;

//  coste: \Theta(n)
unsigned long pow2_1(unsigned n) {
	STEPS1++;

	if (n == 0) {
		return 1;
	}
	else {
		unsigned long resultado = pow2_1(n-1) * 2;
		return resultado;
	}	
}

//  coste: \Theta(2ˆn)
unsigned long pow2_2(unsigned n) {
	STEPS2++;

	if (n == 0) {
		return 1;
	}
	else {
		unsigned long resultado =  pow2_2(n-1) + pow2_2(n-1);
		return resultado;
	}	
}

// coste: \Theta(log(n))
unsigned long pow2_3(unsigned n) {
	STEPS3++;

    if (n == 0) {
		return 1;
    }
	else {
		unsigned long resultado = pow2_3(n/2);
		resultado *= resultado;

		if (n % 2 == 1) {
	        resultado *= 2;
		}

		return resultado;
    }
}

int main() {
	for(unsigned i = 0; i <= 20; i++) {
		STEPS1 = 0, STEPS2 = 0, STEPS3 = 0;

		unsigned long resultado1 = pow2_1(i);
		if (resultado1 != pow(2,i)) {
			cerr << "Ha ocurrido un error en la primera función." << endl;
			return -1;
		}
		cout << STEPS1 << " ";

		unsigned long resultado2 = pow2_2(i);
		if (resultado2 != pow(2,i)) {
			cerr << "Ha ocurrido un error en la segunda función." << endl;
			return -1;
		}
		cout << STEPS2 << " ";

		unsigned long resultado3 = pow2_3(i);
		if (resultado3 != pow(2,i)) {
			cerr << "Ha ocurrido un error en la tercera función." << endl;
			return -1;
		}
		cout << STEPS3 << endl;
	}

	return 0;
}
