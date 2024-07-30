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
#include <queue>

using namespace std;

unsigned VISITADOS = 1, EXPLORADOS = 0, HOJAS = 0, NO_FACTIBLES = 0, NO_PROMETEDORES = 0;
unsigned PROM_DESCARTADOS = 0, MEJOR_ACT_HOJA = 0, MEJOR_ACT_PESIMISTA = 0;
unsigned NODOS_VIVOS = 1;

enum Orientacion { N, NE, E, SE, S, SW, W, NW };

map<Orientacion, tuple<int, int>> movimientos = {
    {E, make_tuple(0, 1)}, {SE, make_tuple(1, 1)}, {S, make_tuple(1, 0)}, {SW, make_tuple(1, -1)},
    {W, make_tuple(0, -1)}, {NW, make_tuple(-1, -1)}, {N, make_tuple(-1, 0)}, {NE, make_tuple(-1, 1)}
};

// Estructura para almacenar los datos de un nodo
struct Node {
    int cota_optimista, x, y, valor_actual;
    Node(int co, int xi, int yi, int va) : cota_optimista(co), x(xi), y(yi), valor_actual(va) {} // Constructor
    bool operator<(const Node &nodo) const {
        if (cota_optimista != nodo.cota_optimista) // Cota optimista como criterio de ordenación
            return cota_optimista > nodo.cota_optimista;
        if (valor_actual != nodo.valor_actual) // Valor actual como criterio de ordenación
            return valor_actual > nodo.valor_actual;
        return (valor_actual / (x + y + 1)) > (nodo.valor_actual / (nodo.x + nodo.y + 1)); // Coste promedio por casilla como criterio de ordenación
    }
};

// Función para comprobar si una casilla está dentro de la matriz
bool es_valida(int x, int y, int n, int m) {
    return x >= 0 && x < n && y >= 0 && y < m;
}

// Función para calcular la cota optimista
int calcular_cota_optimista(int x, int y, int valor_actual, int n, int m) {
    int distancia_restante = max(n - 1 - x, m - 1 - y);
    return valor_actual + distancia_restante; // Distancia Chebyshev como cota optimista
}

// Función para reconstruir el camino óptimo a partir de la matriz de nodos padres
vector<Orientacion> reconstruir_camino(vector<vector<pair<int, int>>> &padres, int x, int y, vector<vector<bool>> &camino_optimo) {
    vector<Orientacion> camino;

    while (padres[x][y] != make_pair(x, y)) { // Mientras no se llegue al nodo inicial
        int px = padres[x][y].first;
        int py = padres[x][y].second;
        camino_optimo[x][y] = true; // Se marca la casilla como parte del camino óptimo

        for (auto &movimiento : movimientos) {
            int dx, dy;
            tie(dx, dy) = movimiento.second;
            if (px + dx == x && py + dy == y) { // Si la casilla actual es el padre de la casilla anterior, se añade la orientación al camino
                camino.push_back(movimiento.first);
                break;
            }
        }

        tie(x, y) = padres[x][y]; // Se actualiza la casilla actual a la casilla padre
    }

    camino_optimo[x][y] = true;
    reverse(camino.begin(), camino.end()); // Se invierte el camino para que esté en el orden correcto, ya que se ha construido desde el final

    return camino;
}

// Función principal del algoritmo de ramificación y poda
void mcp_bb(int n, int m, vector<vector<int>> &matriz_datos, int &mejor_valor, vector<vector<bool>> &camino_optimo, vector<vector<int>> &coste_minimo,
            vector<vector<bool>> &visitados, priority_queue<Node> &pq, vector<Orientacion> &mejor_camino_codificado, vector<vector<pair<int, int>>> &padres) {
    while (!pq.empty()) {
        Node node = pq.top(); // Se extrae el nodo con menor cota optimista
        pq.pop(); // Se elimina el nodo de la cola de prioridad

        int x = node.x, y = node.y, valor_actual = node.valor_actual;
        int cota_optimista = node.cota_optimista;

        VISITADOS++;

        // Poda si el valor actual es mayor o igual que el mejor valor encontrado hasta ahora
        if (valor_actual >= mejor_valor) {
            NO_PROMETEDORES++;
            continue;
        }

        if (cota_optimista >= mejor_valor) { // Poda por cota optimista
            NO_PROMETEDORES++;
            continue;
        }
        
        if (x == n - 1 && y == m - 1) {
            HOJAS++;
            if (valor_actual < mejor_valor) { // Si se ha encontrado un camino mejor
                MEJOR_ACT_HOJA++;
                mejor_valor = valor_actual;
                camino_optimo = visitados;
                mejor_camino_codificado = reconstruir_camino(padres, x, y, camino_optimo);
            }
            continue;
        }

        EXPLORADOS++;

        for (auto &movimiento : movimientos) {
            int dx, dy;
            tie(dx, dy) = movimiento.second;
            int new_x = x + dx;
            int new_y = y + dy;

            if (es_valida(new_x, new_y, n, m) && !visitados[new_x][new_y]) {
                visitados[new_x][new_y] = true;

                int coste_nuevo = valor_actual + matriz_datos[new_x][new_y]; // Coste acumulado hasta la nueva posición

                if (coste_nuevo < mejor_valor) { // Si es mejor que el mejor valor encontrado hasta ahora
                    if (coste_nuevo < coste_minimo[new_x][new_y]) { // Si es mejor que el coste mínimo encontrado hasta ahora para esa casilla
                        coste_minimo[new_x][new_y] = coste_nuevo;
                        pq.emplace(calcular_cota_optimista(new_x, new_y, coste_nuevo, n, m), new_x, new_y, coste_nuevo); // Se añade a la cola de prioridad
                        NODOS_VIVOS++;
                        padres[new_x][new_y] = make_pair(x, y); // Se actualiza el padre de la nueva casilla
                    }
                    else {
                        PROM_DESCARTADOS++;
                    }
                } else {
                    NO_PROMETEDORES++;
                }

                visitados[new_x][new_y] = false;
            } else {
                NO_FACTIBLES++;
            }
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
void uso_correcto_argumentos() {
    cerr << "Usage:\nmcp_bb [-p] [--p2D] -f file" << endl;
}

// Función para almacenar los datos del archivo de entrada en la matriz de datos
void lectura_archivo(string &fichero_entrada, vector<vector<int>> &matriz_datos, int &n, int &m) {
    ifstream fichero(fichero_entrada);

    if (!fichero) {
        cerr << "ERROR: can’t open file: " << fichero_entrada << "." << endl;
        uso_correcto_argumentos();
        exit(1);
    }

    if (fichero >> n >> m) {
        matriz_datos.resize(n, vector<int>(m));

        for (int a = 0; a < n; ++a) {
            for (int b = 0; b < m; ++b) {
                fichero >> matriz_datos[a][b];
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
            if (i + 1 < argc) { 
                if(argv[i+1][0] != '-') {
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

    if (argc == 1) {
        uso_correcto_argumentos();
        return 1;
    }

    gestion_argumentos(argc, argv, fichero_entrada, camino_2D, camino_codificado);
    lectura_archivo(fichero_entrada, matriz_datos, n, m);

    vector<vector<bool>> camino_optimo(n, vector<bool>(m, false)), visitados(n, vector<bool>(m, false));
    vector<vector<int>> coste_minimo(n, vector<int>(m, numeric_limits<int>::max())); // Matriz para almacenar el coste mínimo de cada casilla

    vector<Orientacion> mejor_camino_codificado; // Vector para almacenar el mejor camino codificado
    vector<vector<pair<int, int>>> padres(n, vector<pair<int, int>>(m, {-1, -1})); // Matriz para almacenar los padres de cada nodo

    priority_queue<Node> pq;
    pq.emplace(calcular_cota_optimista(0, 0, matriz_datos[0][0], n, m), 0, 0, matriz_datos[0][0]); // Se inicializa la cola de prioridad con el nodo inicial

    visitados[0][0] = true;
    padres[0][0] = {0, 0};

    int mejor_valor = numeric_limits<int>::max(); // Cota pesimista inicial

    auto start = chrono::high_resolution_clock::now();
    mcp_bb(n, m, matriz_datos, mejor_valor, camino_optimo, coste_minimo, visitados, pq, mejor_camino_codificado, padres);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> duracion = end - start;

    cout << mejor_valor << endl;
    cout << VISITADOS << " " << EXPLORADOS << " " << HOJAS << " " << NO_FACTIBLES << " " << NO_PROMETEDORES << " " << PROM_DESCARTADOS << " " << MEJOR_ACT_HOJA << " " << MEJOR_ACT_PESIMISTA << endl;
    cout << fixed << setprecision(3) << duracion.count() << endl;
    cout << "Nodos Vivos: " << NODOS_VIVOS << endl;

    if (camino_2D) {
        p2D(matriz_datos, camino_optimo, n, m);
    }

    if (camino_codificado) {
        cout << codificar_camino(mejor_camino_codificado) << endl;
    }

    return 0;
}
