// NOEL MARTÍNEZ POMARES

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <tuple>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;

unsigned VISITADOS = 1 /*Se inicializa en 1 porque antes de llamar a mcp_bt se explora el [0][0]*/, EXPLORADOS = 0, HOJAS = 0, NO_FACTIBLES = 0, NO_PROMETEDORES = 0; 

enum Orientacion { N, NE, E, SE, S, SW, W, NW }; // Posibles orientaciones de los movimientos

map<Orientacion, tuple<int, int>> movimientos = { // Los movimientos posibles y sus desplazamientos en x e y, comenzando por E y en sentido horario
    {E, make_tuple(0, 1)}, {SE, make_tuple(1, 1)}, {S, make_tuple(1, 0)}, {SW, make_tuple(1, -1)},
    {W, make_tuple(0, -1)}, {NW, make_tuple(-1, -1)}, {N, make_tuple(-1, 0)}, {NE, make_tuple(-1, 1)}
};

// Función para encontrar el camino más corto usando programación dinámica iterativa
int mcp_it_vector(int n, int m, vector<vector<int>> matriz_datos, vector<vector<bool>> &camino_pd, vector<Orientacion> &orientacion_pd) {
    vector<int> anterior(m+1, 0);
    vector<int> actual(m+1, 0);
    vector<vector<Orientacion>> direccion(n+1, vector<Orientacion>(m+1)); // Vector de vectores para guardar las orientaciones

    anterior[0] = matriz_datos[0][0];
    for (int j = 1; j <= m; j++) {
        anterior[j] = anterior[j-1] + matriz_datos[0][j];
        direccion[0][j] = E; // viene de la izquierda (E)
    }

    for (int i = 1; i <= n; i++) {
        actual[0] = anterior[0] + matriz_datos[i][0];
        direccion[i][0] = S; // viene de arriba (S)

        for (int j = 1; j <= m; j++) {
            int minimo = min(min(anterior[j], actual[j-1]), anterior[j-1]);
            actual[j] = matriz_datos[i][j] + minimo;

            if (minimo == anterior[j-1]) {
                direccion[i][j] = SE; // viene de diagonal (SE)
            } else if (minimo == actual[j-1]) {
                direccion[i][j] = E; // viene de la izquierda (E)
            } else {
                direccion[i][j] = S; // viene de arriba (S)
            }
        }

        swap(anterior, actual);
    }

    // Reconstruir el camino y las orientaciones a partir del vector direccion
    int x = n, y = m;
    vector<Orientacion> orientaciones_inversas; // Para mantener el orden correcto al final

    while (x > 0 || y > 0) {
        camino_pd[x][y] = true;
        orientaciones_inversas.push_back(direccion[x][y]); // guarda la orientación

        if (direccion[x][y] == SE) { // diagonal
            x--;
            y--;
        } else if (direccion[x][y] == E) { // izquierda
            y--;
        } else { // arriba
            x--;
        }
    }
    camino_pd[0][0] = true; // Para incluir el punto inicial

    // Hay que dar la vuelta a las orientaciones para que estén en el orden de inicio a fin
    reverse(orientaciones_inversas.begin(), orientaciones_inversas.end());
    orientacion_pd = orientaciones_inversas;

    return anterior[m];
}

// Función para comprobar si una casilla está dentro de la matriz
bool es_valida(int x, int y, int n, int m) {
    return x >= 0 && x < n && y >= 0 && y < m;
}

// Función recursiva para encontrar el camino más corto usando backtracking
void mcp_bt(int n, int m, int x, int y, int valor_actual, vector<vector<bool>> &visitados,
            int &mejor_valor, vector<vector<bool>> &camino_optimo, vector<vector<int>> &matriz_datos,
            vector<vector<int>> &coste_minimo, vector<Orientacion> &orientacion_actual, vector<Orientacion> &orientacion_optima) {
    VISITADOS++;

    // Poda basada en memoria que evita explorar caminos que no mejoran el coste mínimo encontrado hasta el momento para una casilla
    if (coste_minimo[x][y] <= valor_actual) { 
        NO_PROMETEDORES++;
        return;
    }

    coste_minimo[x][y] = valor_actual; // Si se ha llegado a la casilla con un coste menor, se actualiza el coste mínimo

    // Poda basada en la cota optimista
    int distancia_restante = max(n - 1 - x, m - 1 - y); // Distancia Chebyshev hasta la esquina inferior derecha
    int cota_optimista = valor_actual + distancia_restante;

    if (cota_optimista >= mejor_valor || valor_actual >= mejor_valor) { 
        NO_PROMETEDORES++;
        return;
    }

    // Caso base: si es una hoja
    if (x == n-1 && y == m-1) {
        HOJAS++;
        mejor_valor = valor_actual;
        camino_optimo = visitados;
        orientacion_optima = orientacion_actual;
        return;
    }

    EXPLORADOS++;

    for (auto &movimiento : movimientos) {
        int dx, dy;
        tie(dx, dy) = movimiento.second;
        int new_x = x + dx;
        int new_y = y + dy;

        if (es_valida(new_x, new_y, n, m) && !visitados[new_x][new_y]) {
            visitados[new_x][new_y] = true; // Marcar la casilla como visitada para evitar ciclos
            orientacion_actual.push_back(movimiento.first);
            mcp_bt(n, m, new_x, new_y, valor_actual + matriz_datos[new_x][new_y], visitados, mejor_valor, camino_optimo, matriz_datos, coste_minimo, orientacion_actual, orientacion_optima);
            orientacion_actual.pop_back();
            visitados[new_x][new_y] = false; // Desmarcar la casilla para explorar otras posibilidades
        } else {
            NO_FACTIBLES++;
        }
    }
}

// Función para imprimir el camino óptimo en 2D
void p2D(vector<vector<int>> &matriz_datos, vector<vector<bool>> &camino_optimo, int n, int m) {
    int valor_camino = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (camino_optimo[i][j]) {
                cout << 'x';
                valor_camino += matriz_datos[i][j];
            } else {
                cout << '.';
            }
        }
        cout << '\n';
    }

    cout << valor_camino << endl;
}

// Función para codificar el camino óptimo
string codificar_camino(vector<Orientacion> &orientacion_optima) {
    string resultado = "<";
    for (auto direccion : orientacion_optima) {
        resultado += to_string(static_cast<int>(direccion) + 1);
    }
    resultado += ">";
    return resultado;
}

// Función para imprimir el uso correcto de los argumentos
void uso_correcto_argumentos(){
    cerr << "Usage:\nmcp_bt [-p] [--p2D] -f file" << endl;
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
void gestion_argumentos(int argc, char *argv[], string &fichero_entrada, bool &camino_2D, bool &camino_codificado) {
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
            camino_2D = true;
        } else if (argv[i] == string("-p")) {
            camino_codificado = true;
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
    bool camino_2D = false, camino_codificado = false;
    int n, m;

    if (argc == 1) { // Si no se ha introducido ningún argumento
        uso_correcto_argumentos();
        return 1;
    }

    gestion_argumentos(argc, argv, fichero_entrada, camino_2D, camino_codificado);
    lectura_archivo(fichero_entrada, matriz_datos, n, m);

    vector<vector<bool>> visitados(n, vector<bool>(m, false)), camino_optimo(n, vector<bool>(m, false));
    vector<Orientacion> orientacion_actual, orientacion_optima;

    // Inicialización de las matrices de programación dinámica
    vector<vector<bool>> camino_pd(n, vector<bool>(m, false));
    vector<Orientacion> orientacion_pd;

    vector<vector<int>> coste_minimo(n, vector<int>(m, numeric_limits<int>::max())); // Inicialización de la matriz de costes mínimos inicializada a +infinito

    int mejor_valor = mcp_it_vector(n-1, m-1, matriz_datos, camino_pd, orientacion_pd); // Se calcula la cota pesimista inicial con programación dinámica iterativa
    int mejor_valor_pd = mejor_valor; // Se guarda el valor del camino más corto calculado con programación dinámica iterativa

    visitados[0][0] = true; // Se marca la casilla de inicio como visitada

    auto start = chrono::high_resolution_clock::now();
    mcp_bt(n, m, 0, 0, matriz_datos[0][0], visitados, mejor_valor, camino_optimo, matriz_datos, coste_minimo, orientacion_actual, orientacion_optima);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> duracion = end - start;

    cout << mejor_valor << endl;
    cout << VISITADOS << " " << EXPLORADOS << " " << HOJAS << " " << NO_FACTIBLES << " " << NO_PROMETEDORES << endl;
    cout << fixed << setprecision(3) << duracion.count() << endl;

    if (camino_2D) {
        if (mejor_valor_pd == mejor_valor) {
            p2D(matriz_datos, camino_pd, n, m);
        }
        else {
            p2D(matriz_datos, camino_optimo, n, m);
        }
    }

    if (camino_codificado) {
        if (mejor_valor_pd == mejor_valor) {
            cout << codificar_camino(orientacion_pd) << endl;
        }
        else {
            cout << codificar_camino(orientacion_optima) << endl;
        }
    }

    return 0;
}
