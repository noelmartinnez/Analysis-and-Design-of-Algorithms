// NOEL MARTÍNEZ POMARES

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

const int FUERA_MATRIZ = numeric_limits<int>::max();
const int CENTINELA = -1;

int mcp_naive(int n, int m, vector<vector<int>> &matriz_datos) {
    if(n < 0 || m < 0) { return FUERA_MATRIZ; }
    if(n == 0 && m == 0) { return matriz_datos[0][0]; }

    int arriba = mcp_naive(n-1, m, matriz_datos);
    int izquierda = mcp_naive(n, m-1, matriz_datos);
    int diagonal = mcp_naive(n-1, m-1, matriz_datos);

    return matriz_datos[n][m] + min(min(arriba, izquierda), diagonal);
}

// Función para inicializar la matriz de memoización con el valor -1
void inicializar_matriz_memoizacion(int n, int m, vector<vector<int>> &matriz_memo) {
    matriz_memo.resize(n); // Redimensión de la matriz para que tenga n filas

    for(int a = 0; a < n; ++a) { // Redimensión de cada fila para que tenga m columnas
        matriz_memo[a].resize(m, CENTINELA);
    }
}

int mcp_memo(int n, int m, vector<vector<int>> &matriz_datos, vector<vector<int>> &matriz_memo) {
    if(n < 0 || m < 0) { return FUERA_MATRIZ; }
    if(matriz_memo[n][m] != CENTINELA) { return matriz_memo[n][m]; } // Se devuelve el valor almacenado si ya se ha calculado antes
    if(n == 0 && m == 0) { return matriz_memo[n][m] = matriz_datos[0][0]; }

    int arriba = mcp_memo(n-1, m, matriz_datos, matriz_memo);
    int izquierda = mcp_memo(n, m-1, matriz_datos, matriz_memo);
    int diagonal = mcp_memo(n-1, m-1, matriz_datos, matriz_memo);    

    return matriz_memo[n][m] = matriz_datos[n][m] + min(min(arriba, izquierda), diagonal);
}

// Función para imprimir el uso correcto de los argumentos
void uso_correcto_argumentos(){
    cerr << "Usage:\nmcp [--p2D] [-t] [--ignore-naive] -f file" << endl;
}

// Función para almacenar los datos del archivo de entrada en la matriz de datos
void lectura_archivo(string &fichero_entrada, vector<vector<int>> &matriz_datos, int &n, int &m) {
    ifstream fichero(fichero_entrada);

    if (!fichero) { // Se comprueba si se ha podido abrir el archivo
        cerr << "ERROR: can’t open file: " << fichero_entrada << "." << endl;
        uso_correcto_argumentos();
        exit(1);
    }

    if (fichero >> n >> m) { // Dimensiones de la matriz
        matriz_datos.resize(n, vector<int>(m)); // Redimensión para que tenga n filas y m columnas

        for(int a = 0; a < n; ++a) { 
            for(int b = 0; b < m; ++b) {
                fichero >> matriz_datos[a][b]; // Se leen los valores y se almacenan en la matriz
            }
        }
    }

    fichero.close();
}

// Función para gestionar los argumentos de la línea de comandos
void gestion_argumentos(int argc, char *argv[], string &fichero_entrada, bool &ignore_naive, bool &camino_solucion, bool &tabla_solucion) {
    bool guion = false, falta_fichero = false;
    string argumento;
    
    for (int i = 1; i < argc; ++i) {        
        if (argv[i] == string("-f")) {
            if (i + 1 < argc) { // Se comprueba si hay un argumento después de -f
                if(argv[i+1][0] != '-') { // Se comprueba si el argumento no es otra opción
                    guion = false;
                    falta_fichero = false;
                    fichero_entrada = argv[++i];
                }
                else{
                    guion = true;
                    argumento = argv[i+1];
                }
            } else {
                falta_fichero = true;
            }
        } else if (argv[i] == string("--ignore-naive")) {
            ignore_naive = true;
        } else if (argv[i] == string("--p2D")) {
            camino_solucion = true;
        } else if (argv[i] == string("-t")) {
            tabla_solucion = true;
        } else {
            cerr << "ERROR: unknown option " << argv[i] << "." << endl;
            uso_correcto_argumentos();
            exit(1);
        }
    }

    if (falta_fichero) {
        cerr << "ERROR: missing filename." << endl;
        uso_correcto_argumentos();
        exit(1);
    }
    else if (guion)
    {
        cerr << "ERROR: can’t open file: " << argumento << "." << endl;
        uso_correcto_argumentos();
        exit(1); 
    }
}

int main(int argc, char *argv[]) {
    vector<vector<int>> matriz_datos;
    string fichero_entrada = "";
    bool ignore_naive = false, camino_solucion = false, tabla_solucion = false;
    int n, m;

    if (argc == 1) { // Si no se ha introducido ningún argumento
        uso_correcto_argumentos();
        return 1;
    }

    gestion_argumentos(argc, argv, fichero_entrada, ignore_naive, camino_solucion, tabla_solucion);
    lectura_archivo(fichero_entrada, matriz_datos, n, m);

    vector<vector<int>> matriz_memo;
    inicializar_matriz_memoizacion(n, m, matriz_memo);

    int resultado_naive = ignore_naive ? -1 : mcp_naive(n-1, m-1, matriz_datos);
    int resultado_memo = mcp_memo(n-1, m-1, matriz_datos, matriz_memo);
    
    if (ignore_naive) {
        cout << "-";
    } else {
        cout << resultado_naive;
    }
    
    cout << " " << resultado_memo << " " << "?" << " " << "?" << endl; 
    
    if (camino_solucion) {
        cout << "?" << endl; 
    }

    if (tabla_solucion) {
        cout << "?" << endl; 
    }    
    
    return 0;
}