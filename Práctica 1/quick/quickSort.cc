// NOEL MARTÍNEZ POMARES 48771960T

/*
ADA 2023-24
Práctica 1: "Empirical analysis of Quicksort average-time complexity."
*/

// #include <unistd.h> // Para getpid()
#include <iostream>
#include <math.h> // Para pow()
#include <ctime> // Para clock() y CLOCKS_PER_SEC
#include <cstdlib> // Para srand() y exit()

using namespace std;

//--------------------------------------------------------------
// Middle QuickSort
// right es el size del vector - 1
// left es 0
void middle_QuickSort(int * v, long left, long right){

    long i,j;
    int pivot,aux; 
    if (left<right)
    {
        i=left; j=right;
        pivot=v[(i+j)/2];
        do
        {
            while (v[i]<pivot) i++;
            while (v[j]>pivot) j--;
            if (i<=j)
            {
                aux=v[i]; v[i]=v[j]; v[j]=aux;
                i++; j--;
            }
       } while (i<=j);
       if (left<j)  middle_QuickSort(v,left,j);
       if (i<right) middle_QuickSort(v,i,right);
    }
}

int main(void){

    srand(0);

    cout << "# QuickSort CPU times in milliseconds:"
    << endl
    << "# Size \t CPU time (ms.)"
    << endl
    << "# ----------------------------"
    << endl;

    for (int exp = 15; exp <= 20; exp++) {

        size_t size = size_t( pow(2,exp) );
        int* v = new int [size];

        double tiempoMedio = 0.0;

        if (!v){
            cerr << "Error, not enough memory!" << endl;
            exit (EXIT_FAILURE);  
        }

        cout << size << "\t\t" << std::flush; // flush vacia el buffer y manda todo a mostrar por pantalla
        
        for(int i = 0; i < 30; i++){

            for (size_t j = 0; j < size; j++){ 
                v[j] = std::rand(); 
            }

            auto start = std::clock(); 
            middle_QuickSort(v,0,size-1);
            auto end = std::clock();

            tiempoMedio = tiempoMedio + 1000.0 * (end - start) / CLOCKS_PER_SEC;

            // Bucle que comprueba que el array está ordenado
            for (size_t j = 1; j < size; j++){
                if (v[j] < v[j-1]){ 
                    cerr << "Panic, array not sorted! " << j << endl; 
                    exit(EXIT_FAILURE);            
                }
            }
        }
        
        cout << tiempoMedio / 30.0 << endl ;

        delete[] v; 
    }
}
