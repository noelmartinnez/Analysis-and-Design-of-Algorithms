// NOEL MARTÍNEZ POMARES

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <tuple>

using namespace std;
 int FUERA_MATRIZ = numeric_limits<int>::max();

int inicio_a_fin(int n, int m, vector<vector<int>> matriz_datos, vector<vector<int>> &camino_inicio_a_fin) {
    int i = 0, j = 0;
    int coste_inicio_a_fin = matriz_datos[i][j]; // Empieza con el coste de la primera celda
    camino_inicio_a_fin[i][j] = matriz_datos[i][j];

    while (i < n || j < m) {
        int abajo = (i < n) ? matriz_datos[i+1][j] : FUERA_MATRIZ;
        int derecha = (j < m) ? matriz_datos[i][j+1] : FUERA_MATRIZ; 
        int diagonal = (i < n && j < m) ? matriz_datos[i+1][j+1] : FUERA_MATRIZ;

        // Comparación para tomar la decisión voraz
        if (diagonal == abajo && diagonal == derecha) {
            coste_inicio_a_fin += diagonal;
            i++;
            j++;
            camino_inicio_a_fin[i][j] = matriz_datos[i][j];
        } else if ((diagonal == derecha && diagonal < abajo) || (diagonal == abajo && diagonal < derecha)) {
            coste_inicio_a_fin += diagonal;
            i++;
            j++;
            camino_inicio_a_fin[i][j] = matriz_datos[i][j];
        } else if (derecha == abajo && derecha < diagonal) {
            coste_inicio_a_fin += derecha;
            j++;
            camino_inicio_a_fin[i][j] = matriz_datos[i][j];
        } else {
            
            int minimo = min(min(abajo, derecha), diagonal);

            if (minimo != FUERA_MATRIZ) {
                if (minimo == abajo) {
                    coste_inicio_a_fin += abajo;
                    i++;
                    camino_inicio_a_fin[i][j] = matriz_datos[i][j];
                } else if (minimo == derecha) {
                    coste_inicio_a_fin += derecha;
                    j++;
                    camino_inicio_a_fin[i][j] = matriz_datos[i][j];
                } else {
                    coste_inicio_a_fin += diagonal;
                    i++;
                    j++;
                    camino_inicio_a_fin[i][j] = matriz_datos[i][j];
                }
            }
        }
    }

    return coste_inicio_a_fin;
}

int fin_a_inicio(int n, int m, vector<vector<int>> matriz_datos, vector<vector<int>> &camino_fin_a_inicio) {
    int a = n, b = m;
    int coste_fin_a_inicio = matriz_datos[a][b];
    camino_fin_a_inicio[a][b] = matriz_datos[a][b];

    while (a > 0 || b > 0) {
        int arriba = (a > 0) ? matriz_datos[a-1][b] : FUERA_MATRIZ;
        int izquierda = (b > 0) ? matriz_datos[a][b-1] : FUERA_MATRIZ;
        int diagonal = (a > 0 && b > 0) ? matriz_datos[a-1][b-1] : FUERA_MATRIZ;

        // Comparación para tomar la decisión voraz
        if (diagonal == arriba && diagonal == izquierda) {
            coste_fin_a_inicio += diagonal;
            a--;
            b--;
            camino_fin_a_inicio[a][b] = matriz_datos[a][b];
        } else if ((diagonal == izquierda && diagonal < arriba) || (diagonal == arriba && diagonal < izquierda)) {
            coste_fin_a_inicio += diagonal;
            a--;
            b--;
            camino_fin_a_inicio[a][b] = matriz_datos[a][b];
        } else if (izquierda == arriba && izquierda < diagonal) {
            coste_fin_a_inicio += izquierda;
            b--;
            camino_fin_a_inicio[a][b] = matriz_datos[a][b];
        } else {
            
            int minimo = min(min(arriba, izquierda), diagonal);

            if (minimo != FUERA_MATRIZ) {
                if (minimo == arriba) {
                    coste_fin_a_inicio += arriba;
                    a--;
                    camino_fin_a_inicio[a][b] = matriz_datos[a][b];
                } else if (minimo == izquierda) {
                    coste_fin_a_inicio += izquierda;
                    b--;
                    camino_fin_a_inicio[a][b] = matriz_datos[a][b];
                } else {
                    coste_fin_a_inicio += diagonal;
                    a--;
                    b--;
                    camino_fin_a_inicio[a][b] = matriz_datos[a][b];
                }
            }
        }
    }

    return coste_fin_a_inicio;
}

tuple<int, int> mcp_greedy(int n, int m, vector<vector<int>> matriz_datos, vector<vector<int>> &camino_inicio_a_fin, vector<vector<int>> &camino_fin_a_inicio) {
    return make_tuple(inicio_a_fin(n, m, matriz_datos, camino_inicio_a_fin), fin_a_inicio(n, m, matriz_datos, camino_fin_a_inicio));
}

void p2D(int n, int m, vector<vector<int>> camino) {
    int coste_total = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (camino[i][j] != -1) {
                coste_total += camino[i][j];
                cout << 'x'; 
            } else { cout << '.'; }
        }
        cout << endl;
    }

    cout << coste_total << endl;
}

// Función para imprimir el uso correcto de los argumentos
void uso_correcto_argumentos(){
    cerr << "Usage:\nmcp_greedy [--p2D] -f file" << endl;
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
void gestion_argumentos(int argc, char *argv[], string &fichero_entrada, bool &camino_solucion) {
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
        } else if (argv[i] == string("--p2D")) {
            camino_solucion = true;
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
    bool camino_solucion = false;
    int n, m;

    if (argc == 1) { // Si no se ha introducido ningún argumento
        uso_correcto_argumentos();
        return 1;
    }

    gestion_argumentos(argc, argv, fichero_entrada, camino_solucion);
    lectura_archivo(fichero_entrada, matriz_datos, n, m); 

    vector<vector<int>> camino_inicio_a_fin(n, vector<int>(m, -1));
    vector<vector<int>> camino_fin_a_inicio(n, vector<int>(m, -1));

    int coste_inicio_a_fin, coste_fin_a_inicio;
    tie(coste_inicio_a_fin, coste_fin_a_inicio) = mcp_greedy(n-1, m-1, matriz_datos, camino_inicio_a_fin, camino_fin_a_inicio);

    cout << coste_inicio_a_fin << " " << coste_fin_a_inicio << endl;

    if (camino_solucion) {
       if (coste_inicio_a_fin <= coste_fin_a_inicio) {
            p2D(n, m, camino_inicio_a_fin);
        } else {
            p2D(n, m, camino_fin_a_inicio);
        }
    }
    
    return 0;
}
