#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

struct Equipo {
    string nombre;
    int PJ=0, PG=0, PE=0, PP=0;
    int GF=0, GC=0, DG=0, PTS=0;
};

struct Partido {
    string fecha;
    string local;
    string visitante;
    int golesLocal;
    int golesVisitante;
};

struct Config {
    string nombreLiga;
    int puntosGanar;
    int puntosEmpatar;
    int puntosPerder;
    vector<string> equipos;
};


bool leerConfig(Config &config) {
    ifstream file("data/config.txt");
    if (!file) return false;

    string linea;
    while(getline(file, linea)) {
        if (linea[0] == '#') continue;

        string clave, valor;
        stringstream ss(linea);
        getline(ss, clave, '=');
        getline(ss, valor);

        if (clave == "liga") config.nombreLiga = valor;
        else if (clave == "victoria") config.puntosGanar = stoi(valor);
        else if (clave == "empate") config.puntosEmpatar = stoi(valor);
        else if (clave == "derrota") config.puntosPerder = stoi(valor);
        else if (clave == "equipo") config.equipos.push_back(valor);
    }
    return true;
}

vector<Partido> leerPartidos() {
    vector<Partido> partidos;
    ifstream file("data/partidos.txt");

    if (!file) return partidos;

    string linea;
    while(getline(file, linea)) {
        stringstream ss(linea);
        Partido p;
        string temp;

        getline(ss, p.fecha, ',');
        getline(ss, p.local, ',');
        getline(ss, p.visitante, ',');
        getline(ss, temp, ',');
        p.golesLocal = stoi(temp);
        getline(ss, temp, ',');
        p.golesVisitante = stoi(temp);

        partidos.push_back(p);
    }
    return partidos;
}


void guardarPartido(Partido p) {
    ofstream file("data/partidos.txt", ios::app);
    file << p.fecha << "," << p.local << "," << p.visitante << ","
         << p.golesLocal << "," << p.golesVisitante << endl;
}


void actualizarEquipo(Equipo *eq, int gf, int gc, Config config) {
    eq->PJ++;
    eq->GF += gf;
    eq->GC += gc;
    eq->DG = eq->GF - eq->GC;

    if (gf > gc) {
        eq->PG++;
        eq->PTS += config.puntosGanar;
    } else if (gf == gc) {
        eq->PE++;
        eq->PTS += config.puntosEmpatar;
    } else {
        eq->PP++;
        eq->PTS += config.puntosPerder;
    }
}


vector<Equipo> construirTabla(vector<Partido> partidos, Config config) {
    vector<Equipo> tabla;

    for (auto nombre : config.equipos) {
        Equipo eq;
        eq.nombre = nombre;
        tabla.push_back(eq);
    }

    for (auto p : partidos) {
        for (auto &eq : tabla) {
            if (eq.nombre == p.local)
                actualizarEquipo(&eq, p.golesLocal, p.golesVisitante, config);
            if (eq.nombre == p.visitante)
                actualizarEquipo(&eq, p.golesVisitante, p.golesLocal, config);
        }
    }

    return tabla;
}


bool comparar(Equipo a, Equipo b) {
    if (a.PTS != b.PTS) return a.PTS > b.PTS;
    if (a.DG != b.DG) return a.DG > b.DG;
    return a.GF > b.GF;
}


void mostrarTabla(vector<Equipo> tabla) {
    sort(tabla.begin(), tabla.end(), comparar);

    cout << "\n# Equipo PJ PG PE PP GF GC DG PTS\n";

    int pos = 1;
    for (auto eq : tabla) {
        cout << pos++ << " "
             << eq.nombre << " "
             << eq.PJ << " "
             << eq.PG << " "
             << eq.PE << " "
             << eq.PP << " "
             << eq.GF << " "
             << eq.GC << " "
             << eq.DG << " "
             << eq.PTS << endl;
    }
}


void registrarPartido(Config config) {
    Partido p;

    cout << "Fecha: ";
    cin >> p.fecha;

    cout << "Equipos:\n";
    for (int i = 0; i < config.equipos.size(); i++)
        cout << i << ". " << config.equipos[i] << endl;

    int l, v;
    cout << "Local: "; cin >> l;
    cout << "Visitante: "; cin >> v;

    if (l == v) {
        cout << "Error: mismo equipo\n";
        return;
    }

    p.local = config.equipos[l];
    p.visitante = config.equipos[v];

    cout << "Goles local: "; cin >> p.golesLocal;
    cout << "Goles visitante: "; cin >> p.golesVisitante;

    guardarPartido(p);
    cout << "Partido guardado\n";
}


void verPartidos() {
    ifstream file("data/partidos.txt");
    string linea;
    while(getline(file, linea))
        cout << linea << endl;
}


int menu(string liga) {
    cout << "\n=== " << liga << " ===\n";
    cout << "1. Ver tabla\n";
    cout << "2. Registrar partido\n";
    cout << "3. Ver partidos\n";
    cout << "4. Salir\n";
    cout << "Opcion: ";
    int op; cin >> op;
    return op;
}


int main() {
    Config config;

    if (!leerConfig(config)) {
        cout << "Error leyendo config\n";
        return 1;
    }

    int op;
    do {
        op = menu(config.nombreLiga);

        if (op == 1) {
            auto partidos = leerPartidos();
            auto tabla = construirTabla(partidos, config);
            mostrarTabla(tabla);
        }
        else if (op == 2)
            registrarPartido(config);
        else if (op == 3)
            verPartidos();

    } while (op != 4);

    return 0;
}
