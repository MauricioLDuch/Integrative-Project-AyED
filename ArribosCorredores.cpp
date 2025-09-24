#include<iostream>
#include<cstring>

using namespace std;

/* Structs */

struct CorredoresCiudad {
    int numero;
    char nombreApellido[50];
    char localidad[40];
    char ciudad[11];
};

struct RegCorredores {
    int numero;
    char nombreApellido[50];
    char categoria[50];
    char genero;
    char localidad[40];
    char llegada[11]; // formato HH:MM:SS
};

/* -------------------------------------------------------------------------------------------- */

/* Functions */

void posicioGeneral(); 
void posicionesPorGenero();
void posicionesPorCategoria();

float tiempoEnSegundos(char llegada[]); // Con esta funcion convierto el tiempo en segundos para poder comparar los tiempos y sacar las posiciones.
float segundosATiempo(float segundos); // Con esta funcion convierto los segundos a tiempo en formato HH:MM:SS.

/* -------------------------------------------------------------------------------------------- */

/* Main */

int main(){

    FILE * archivoCorredores = fopen("\4Refugios\Archivo corredores 4Refugios.bin", "rb");
    FILE * archivoDeCiudades = fopen("\4Refugios\Ciudades.bin", "rb");


    if(archivoCorredores == NULL || archivoDeCiudades == NULL){
        cout << "Error al abrir los archivos." << endl;
        return 1;
    };

    bool archivo1 = fread(archivoCorredores, sizeof(RegCorredores), 1000, archivoCorredores);
    bool archivo2 = fread(archivoDeCiudades, sizeof(CorredoresCiudad), 100, archivoDeCiudades);
    



};

/* -------------------------------------------------------------------------------------------- */

/* Functional development */

float tiempoEnSegundos(char llegada[]){
    
    float segundos = 0;
    int tiempo = 0;
    
    if (strncmp(llegada, "DNF", 3) == 0 || strncmp(llegada, "NL", 2) == 0 || strncmp(llegada, "FE", 2) == 0){
        return -1;
    };

    tiempo = (llegada[0] - '0') * 10 + (llegada[1] - '0') * 3600; // Horas a segundos
    tiempo += (llegada[3] - '0') * 10 + (llegada[4] - '0') * 60; // Minutos a segundos
    tiempo += (llegada[6] - '0') * 10 + (llegada[7] - '0') ; // Segundos

    llegada[8] == '.' && llegada[9] != '10' ? segundos = (llegada[9] - '0') * 0.1 : segundos = 0; // Decimas de segundo

    return tiempo + segundos;
};

float segundosATiempo(float segundos){
    int horas = segundos / 3600;
    segundos = segundos - (horas * 3600);
    int minutos = segundos / 60;
    segundos = segundos - (minutos * 60);
    float decimas = (segundos - (int)segundos) * 10;
    int seg = (int)segundos;

    return horas * 1000000 + minutos * 10000 + seg * 100 + decimas;
};




