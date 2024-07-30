// NOEL MARTÍNEZ POMARES

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

const int FUERA_MATRIZ = numeric_limits<int>::max();
const int CENTINELA = -1;

int mcp_naive(int n, int m, const vector<vector<int>> &matriz_datos) {
    if(n < 0 || m < 0) { return FUERA_MATRIZ; }
    if(n == 0 && m == 0) { return matriz_datos[0][0]; }

    int arriba = mcp_naive(n-1, m, matriz_datos);
    int izquierda = mcp_naive(n, m-1, matriz_datos);
    int diagonal = mcp_naive(n-1, m-1, matriz_datos);

    return matriz_datos[n][m] + min(min(arriba, izquierda), diagonal);
}

int mcp_memo(int n, int m, const vector<vector<int>> &matriz_datos, vector<vector<int>> &matriz_memo) {
    if(n < 0 || m < 0) { return FUERA_MATRIZ; }
    if(matriz_memo[n][m] != CENTINELA) { return matriz_memo[n][m]; } // Se devuelve el valor almacenado si ya se ha calculado antes
    if(n == 0 && m == 0) { return matriz_memo[n][m] = matriz_datos[0][0]; }

    int arriba = mcp_memo(n-1, m, matriz_datos, matriz_memo);
    int izquierda = mcp_memo(n, m-1, matriz_datos, matriz_memo);
    int diagonal = mcp_memo(n-1, m-1, matriz_datos, matriz_memo);    

    return matriz_memo[n][m] = matriz_datos[n][m] + min(min(arriba, izquierda), diagonal);
}

int mcp_it_matrix(int n, int m, vector<vector<int>> matriz_datos, vector<vector<int>> &matriz_memo_iter) {
    matriz_memo_iter[0][0] = matriz_datos[0][0];  // Coste inicial en la esquina superior izquierda

    for (int i = 1; i <= n; i++) { // Rellenar la primera columna
        // Se suma el coste de la celda actual (datos) con el valor de la celda superior (memo) y se almacena el resultado en la celda actual (memo)
        matriz_memo_iter[i][0] = matriz_datos[i][0] + matriz_memo_iter[i-1][0];
    }

    for (int j = 1; j <= m; j++) { // Rellenar la primera fila
        matriz_memo_iter[0][j] = matriz_datos[0][j] + matriz_memo_iter[0][j-1];
    }

    // Rellenar el resto de la matriz hasta llegar a la esquina inferior derecha, que contendrá el coste mínimo
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            // Se suma el coste de la celda actual (datos) con el valor mínimo de las celdas adyacentes (memo) y se almacena el resultado en la celda actual (memo)
            matriz_memo_iter[i][j] = matriz_datos[i][j] + min(min(matriz_memo_iter[i-1][j], matriz_memo_iter[i][j-1]), matriz_memo_iter[i-1][j-1]);
        }
    }

    return matriz_memo_iter[n][m];
}

int mcp_it_vector(int n, int m, vector<vector<int>> matriz_datos) {
    vector<int> anterior(m+1, 0); // +1 para utilizar índices que empiezan en 1
    vector<int> actual(m+1, 0);

    // Inicializar la primera fila (anterior) basándose en la primera fila de la matriz de datos
    anterior[0] = matriz_datos[0][0];
    for (int j = 1; j <= m; j++) { anterior[j] = anterior[j-1] + matriz_datos[0][j]; }

    for (int i = 1; i <= n; i++) {
        actual[0] = anterior[0] + matriz_datos[i][0]; // Solo puede ser alcanzado desde la celda superior, que es anterior[0]
        
        for (int j = 1; j <= m; j++) { actual[j] = matriz_datos[i][j] + min(min(anterior[j], actual[j-1]), anterior[j-1]); }

        // Intercambiar anterior y actual para la siguiente iteración
        swap(anterior, actual);
    }

    // El último elemento de anterior; que contiene los resultados de la última fila, contiene el coste mínimo
    return anterior[m];
}

int mcp_parser(int n, int m, vector<vector<int>> matriz_datos, vector<vector<int>> matriz_memo) {
    vector<vector<char>> camino(n, vector<char>(m, '.')); // Matriz inicializada con '.' en todas las celdas
    int i = n - 1, j = m - 1; // Índices para recorrer la matriz
    int coste_total = 0;

    while (i > 0 || j > 0) { // Empieza en la esquina inferior derecha
        camino[i][j] = 'x'; // Marcar la celda actual
        coste_total += matriz_datos[i][j]; // Sumar el valor de la celda actual al coste total

        if (i == 0) {
            j--; // Mover hacia la izquierda
        } else if (j == 0) {
            i--; // Mover hacia arriba
        } else {
            // Mover hacia la celda con el valor mínimo
            int arriba = matriz_memo[i-1][j];
            int izquierda = matriz_memo[i][j-1];
            int diagonal = matriz_memo[i-1][j-1];

            if (arriba <= izquierda && arriba <= diagonal) {
                i--; 
            } else if (izquierda <= arriba && izquierda <= diagonal) {
                j--;
            } else { i--; j--; } // Mover en diagonal
        }
    }

    camino[0][0] = 'x'; // Marcar la celda de inicio
    coste_total += matriz_datos[0][0]; // Sumar el valor de la celda de inicio al coste total

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) { cout << camino[i][j]; }
        cout << endl;
    }

    return coste_total;
}

// Función para imprimir la matriz de memoización iterativa
void imprimir_matriz_iterativa(int n, int m, vector<vector<int>> matriz_memo_iter) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) { cout << matriz_memo_iter[i][j] << " "; }
        cout << endl;
    }
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

    vector<vector<int>> matriz_memo(n, vector<int>(m, CENTINELA));
    vector<vector<int>> matriz_memo_iter(n, vector<int>(m, 0));

    int resultado_naive = ignore_naive ? -1 : mcp_naive(n-1, m-1, matriz_datos);
    int resultado_memo = mcp_memo(n-1, m-1, matriz_datos, matriz_memo);
    int resultado_it_matrix = mcp_it_matrix(n-1, m-1, matriz_datos, matriz_memo_iter);
    int resultado_it_vector = mcp_it_vector(n-1, m-1, matriz_datos);
    
    if (ignore_naive) {
        cout << "-";
    } else {
        cout << resultado_naive;
    }
    
    cout << " " << resultado_memo << " " << resultado_it_matrix << " " << resultado_it_vector << endl; 
    
    if (camino_solucion) {
        cout << mcp_parser(n, m, matriz_datos, matriz_memo) << endl; 
    }

    if (tabla_solucion) {
        imprimir_matriz_iterativa(n, m, matriz_memo_iter);
    }    
    
    return 0;
}
