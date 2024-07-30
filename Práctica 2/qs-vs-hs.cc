// NOEL MARTÍNEZ POMARES 48771960T
/*
ADA. 2023-24
Práctica 2:"Empirical analysis by means of program-steps account of two sorting algorithms: Middle-Quicksort and Heapsort."
*/
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

double STEPS = 0;

//--------------------------------------------------------------
// Middle Quick Sort
void middle_QuickSort(int * v, long left, long right) {
    STEPS++;
    long i,j;
    int pivot; 
	if (left<right){
		i=left; j=right;
		pivot=v[(i+j)/2];
		do{
            STEPS++;
			while (v[i]<pivot) {
                i++;
                STEPS++;
            }
			while (v[j]>pivot) {
                j--;
                STEPS++;
            }
			if (i<=j){ 
				swap(v[i], v[j]);
				i++; j--;
			}
		}while (i<=j);
		if (left < j)  middle_QuickSort(v,left,j);
		if (i < right) middle_QuickSort(v,i,right);
	}
}

//--------------------------------------------------------------                
//HEAPSORT
// Procedure sink used by Heapsort algorith
// Sink an element (indexed by i) in a tree to keep the heap property. n is the size of the heap. 

void sink(int *v, size_t n, size_t i) {
    STEPS++;
    size_t largest;
    size_t l, r; //left and right child

    do{
        largest = i;  // Initialize largest as root
        l = 2*i + 1;  // left = 2*i + 1
        r = 2*i + 2;  // right = 2*i + 2
    
        // Is left child larger than root?
        if (l < n && v[l] > v[largest])
            largest = l;
    
        // Is right child larger than largest so far
        if (r < n && v[r] > v[largest])
            largest = r;
    
        // If largest is still root then the process is done
        if (largest == i) break;
        
        // If not, swap new largest with current node i and repeat the process with childs. 
        swap(v[i], v[largest]);
        i=largest;  
        STEPS++;
   } while (true);
}
 
//--------------------------------------------------------------                
// Heapsort algorithm (ascending sorting)
void heapSort(int *v, size_t n) {
    STEPS++;
    // Build a MAX-HEAP with the input array
    for (size_t i = n / 2 - 1; true; i--){
        STEPS++;
        sink(v, n, i);
        if (i==0) break; //as size_t is unsigned type
	}	

  	// A this point we have a HEAP on the input array, let's take advantage of this to sort the array:
    // One by swap the first element, which is the largest (max-heap), with the last element of the vector and rebuild heap by sinking the new placed element on the begin of vector.  
    for (size_t i=n-1; i>0; i--)
    {
		// Move current root to the end.
        swap(v[0], v[i]);
 		// Now largest element is at the end but do not know if the element just moved to the beginning is well placed, so a sink process is required.
		// Note that, at the same time, the HEAP size is reduced one element by one.
        sink(v, i, 0);
		// The procedure ends when HEAP has only one element, the lower of the input array
        STEPS++;
    }
}

int main(void){
    
    srand(0);

    cout << "#QUICKSORT VERSUS HEAPSORT"
         << endl   
         << "#Average processing Msteps (millions of program steps)"
         << endl
         << "#Number of samples (arrays of integer): 30"
         << endl
         << "#\t\t RANDOM ARRAYS \t\t SORTED ARRAYS \t\t REVERSE SORTED ARRAYS"
         << endl
         << "# \t\t ------------- \t\t ------------- \t\t ---------------------"
         << endl
         << "#Size \t\t QuickSort HeapSort \t QuickSort HeapSort \t QuickSort HeapSort"
         << endl
         << "#======================================================================================="
    << endl;
    
    int pasosQuickSortAleatorio, pasosHeapSortAleatorio, pasosQuickSortOrdenado, pasosHeapSortOrdenado, pasosQuickSortInverso, pasosHeapSortInverso;

    // Bucle que se hace para ir cambiando el tamaño del array
    for(int exp = 15; exp <= 20; exp++){

        pasosQuickSortAleatorio = 0;
        pasosHeapSortAleatorio = 0;
        pasosQuickSortOrdenado = 0;
        pasosHeapSortOrdenado = 0;
        pasosQuickSortInverso = 0;
        pasosHeapSortInverso = 0;
        
        size_t size = size_t(pow(2,exp));

        int* v = new int [size]; // array con datos aleatorios
        int* v_copia = new int [size]; // copia del array v que se utiliza para ordenar los datos
        int* v_inverso = new int[size]; // array con los datos en orden inverso
        int* v_inverso_copia = new int[size]; // copia del array inverso que se utiliza para que HeapSort opere con los mismos datos que QuickSort
        
        if(!v || !v_copia || !v_inverso || !v_inverso_copia){
            cerr << "Error, not enough memory!" << endl;
            exit(EXIT_FAILURE);
        }

        cout << size << "\t" << std::flush;

        for(int i = 0; i < 30; i++){
            for(size_t j = 0; j < size; j++){
                v[j] = rand();
                v_copia[j] = v[j]; // copia de v
            }
            
            // Obtenemos los pasos de cada algoritmo con el array aleatorio
            STEPS = 0;
            middle_QuickSort(v, 0, size);
            pasosQuickSortAleatorio += STEPS;

            STEPS = 0;
            heapSort(v_copia, size);
            pasosHeapSortAleatorio += STEPS;
            
            // Obtenemos los pasos de cada algoritmo con el array ordenado
            // Aunque el array era aletorio, QuickSort y HeapSort ya lo han ordenado en el paso anterior
            STEPS = 0;
            middle_QuickSort(v, 0, size);
            pasosQuickSortOrdenado += STEPS;

            STEPS = 0;
            heapSort(v_copia, size);
            pasosHeapSortOrdenado += STEPS;

            // Invertimos el array ordenado
            long pivote = size;

            for (size_t i = 0; i < size; i++){
                v_inverso[i] = v[pivote-1];
                v_inverso_copia[i] = v_inverso[i]; // copia de v_inverso
                pivote--;
            }

            // Obtenemos los pasos de cada algoritmo con el array ordenado en orden inverso
            STEPS = 0;
            middle_QuickSort(v_inverso, 0, size);
            pasosQuickSortInverso += STEPS;
            
            STEPS = 0;                                                                                                                                              
            heapSort(v_inverso_copia, size);
            pasosHeapSortInverso += STEPS;
        }

        cout << fixed << setprecision(3) << "\t";
        cout << " " <<  (pasosQuickSortAleatorio / (30 * 1000000.0)) << "\t";
        cout <<  (pasosHeapSortAleatorio / (30 * 1000000.0)) << "\t \t";
        cout << " " <<  (pasosQuickSortOrdenado / 30) / 1000000.0 << "\t";
        cout <<  (pasosHeapSortOrdenado / (30 * 1000000.0)) << "\t \t";
        cout << " " <<  (pasosQuickSortInverso / (30 * 1000000.0)) << "\t";
        cout << (pasosHeapSortInverso / (30 * 1000000.0)) << endl;

        // Comprobamos que los arrays estén ordenados
        for(size_t i = 1; i < size; i++){
            if(v[i] < v[i-1]){
                cerr << "Panic, array not sorted!" << i << endl;
                exit(EXIT_FAILURE);
            }

            if(v_copia[i] < v_copia[i-1]){
                cerr << "Panic, array not sorted!" << i << endl;
                exit(EXIT_FAILURE);
            }

            if(v_inverso[i] < v_inverso[i-1]){
                cerr << "Panic, array not sorted!" << i << endl;
                exit(EXIT_FAILURE);
            }

            if(v_inverso_copia[i] < v_inverso_copia[i-1]){
                cerr << "Panic, array not sorted!" << i << endl;
                exit(EXIT_FAILURE);
            }
        }

        delete[] v;
        delete[] v_copia;
        delete[] v_inverso;
        delete[] v_inverso_copia;
    }
}