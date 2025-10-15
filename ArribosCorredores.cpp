#include <iostream>
#include <cstring>
#include <cstdio> // Para usar printf
#include <cctype> // Para usar isprint

using namespace std;

// --- ESTRUCTURAS ---
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
    char llegada[11];
};

// --- PROTOTIPOS DE FUNCIONES ---
void reportePorLocalidad();
void reportePorCarrera();
float tiempoEnSegundos(char llegada[]);
int leerCiudades(CorredoresCiudad ciudades[]);
void imprimirTiempo(float totalSegundos);
void sanitizarCadena(char* str); // <-- NUEVA FUNCIÓN PARA LIMPIAR TEXTO

// --- MAIN ---
int main() {
    reportePorLocalidad();
    cout << "\n\n";
    reportePorCarrera();
    return 0;
}

// --- DESARROLLO DE FUNCIONES ---

/**
 * @brief Reemplaza caracteres no imprimibles o problemáticos en una cadena por espacios.
 * @param str La cadena de caracteres a "limpiar".
 */
void sanitizarCadena(char* str) {
    for (int i = 0; str[i] != '\0'; ++i) {
        // isprint() revisa si el carácter es visible (letra, número, símbolo común)
        // (unsigned char) es para manejar correctamente caracteres extendidos.
        if (!isprint((unsigned char)str[i])) {
            str[i] = ' '; // Si es un carácter raro, lo reemplaza por un espacio
        }
    }
}


float tiempoEnSegundos(char llegada[]) {
    if (strncmp(llegada, "DNF", 3) == 0 || strncmp(llegada, "NL", 2) == 0 || strncmp(llegada, "FE", 2) == 0) {
        return -1;
    }
    int horas = (llegada[0] - '0') * 10 + (llegada[1] - '0');
    int minutos = (llegada[3] - '0') * 10 + (llegada[4] - '0');
    int segundos = (llegada[6] - '0') * 10 + (llegada[7] - '0');
    return horas * 3600 + minutos * 60 + segundos;
}

void imprimirTiempo(float totalSegundos) {
    if (totalSegundos < 0) return;
    int h = totalSegundos / 3600;
    int m = ((int)totalSegundos % 3600) / 60;
    int s = (int)totalSegundos % 60;
    printf("%02d:%02d:%02d", h, m, s);
}

int leerCiudades(CorredoresCiudad ciudades[]) {
    FILE* archCiudades = fopen("4Refugios/Ciudades.bin", "rb");
    if (!archCiudades) {
        cout << "Error critico al abrir Ciudades.bin" << endl;
        return 0;
    }
    int contador = 0;
    while (fread(&ciudades[contador], sizeof(CorredoresCiudad), 1, archCiudades) && contador < 1000) {
        contador++;
    }
    fclose(archCiudades);
    return contador;
}

void reportePorLocalidad() {
    FILE* archCorredores = fopen("4Refugios/Archivo corredores 4Refugios.bin", "rb");
    if (!archCorredores) {
        cout << "Error al abrir Archivo corredores 4Refugios.bin" << endl;
        return;
    }
    CorredoresCiudad ciudades[1000];
    int numCiudades = leerCiudades(ciudades);
    if (numCiudades == 0) {
        fclose(archCorredores);
        return;
    }
    struct Stat {
        char localidad[40], ciudad[11];
        int cantidad;
        float sumaTiempos;
    };
    Stat stats[500];
    int numStats = 0;
    RegCorredores corredor;
    while (fread(&corredor, sizeof(RegCorredores), 1, archCorredores)) {
        float tiempo = tiempoEnSegundos(corredor.llegada);
        if (tiempo > 0) {
            for (int i = 0; i < numCiudades; i++) {
                if (ciudades[i].numero == corredor.numero) {
                    bool encontrado = false;
                    for (int j = 0; j < numStats; j++) {
                        if (strcmp(stats[j].localidad, ciudades[i].localidad) == 0 &&
                            strcmp(stats[j].ciudad, ciudades[i].ciudad) == 0) {
                            stats[j].cantidad++;
                            stats[j].sumaTiempos += tiempo;
                            encontrado = true;
                            break;
                        }
                    }
                    if (!encontrado && numStats < 500) {
                        strcpy(stats[numStats].localidad, ciudades[i].localidad);
                        strcpy(stats[numStats].ciudad, ciudades[i].ciudad);
                        stats[numStats].cantidad = 1;
                        stats[numStats].sumaTiempos = tiempo;
                        numStats++;
                    }
                    break;
                }
            }
        }
    }
    
    printf("+------------------------+----------------------+--------------------------+-------------------+\n");
    printf("| %-22s | %-20s | %-24s | %-17s |\n", "Localidad", "Ciudad", "Cantidad de Corredores", "Tiempo Promedio");
    printf("+------------------------+----------------------+--------------------------+-------------------+\n");

    if (numStats > 0) {
        char localidadActual[40];
        strcpy(localidadActual, stats[0].localidad);
        int totalLocalidad = 0;
        float totalTiempo = 0;

        for (int i = 0; i < numStats; i++) {
            if (strcmp(localidadActual, stats[i].localidad) != 0) {
                sanitizarCadena(localidadActual); // Limpiamos antes de imprimir
                char totalLabel[40];
                snprintf(totalLabel, sizeof(totalLabel), "Total %s", localidadActual);
                
                printf("| %-22.22s | %-20s | %-24d | ", totalLabel, "", totalLocalidad);
                imprimirTiempo(totalTiempo / totalLocalidad);
                printf("%9s |\n", "");
                printf("+------------------------+----------------------+--------------------------+-------------------+\n");
                
                totalLocalidad = 0;
                totalTiempo = 0;
                strcpy(localidadActual, stats[i].localidad);
            }

            // --- LLAMADA A LA NUEVA FUNCIÓN ---
            sanitizarCadena(stats[i].localidad);
            sanitizarCadena(stats[i].ciudad);
            //------------------------------------

            printf("| %-22.22s | %-20.20s | %-24d | ", stats[i].localidad, stats[i].ciudad, stats[i].cantidad);
            imprimirTiempo(stats[i].sumaTiempos / stats[i].cantidad);
            printf("%9s |\n", "");

            totalLocalidad += stats[i].cantidad;
            totalTiempo += stats[i].sumaTiempos;
        }

        sanitizarCadena(localidadActual); // Limpiamos el último total también
        char totalLabel[40];
        snprintf(totalLabel, sizeof(totalLabel), "Total %s", localidadActual);
        printf("| %-22.22s | %-20s | %-24d | ", totalLabel, "", totalLocalidad);
        imprimirTiempo(totalTiempo / totalLocalidad);
        printf("%9s |\n", "");
        printf("+------------------------+----------------------+--------------------------+-------------------+\n");

    }
    fclose(archCorredores);
}

void reportePorCarrera() {
    FILE* archCorredores = fopen("4Refugios/Archivo corredores 4Refugios.bin", "rb");
    if (!archCorredores) {
        cout << "Error al abrir Archivo corredores 4Refugios.bin" << endl;
        return;
    }
    CorredoresCiudad ciudades[1000];
    int numCiudades = leerCiudades(ciudades);
    if (numCiudades == 0) {
        fclose(archCorredores);
        return;
    }
    struct Corredor {
        int numero;
        char nombre[50], categoria[50], localidad[40];
        char genero;
        float segundos;
        char tiempo[20];
        int posGral, posGen, posCat;
        char diffPrimero[20], diffAnterior[20];
    };
    Corredor clasica[1000], nonStop[1000];
    int numClasica = 0, numNonStop = 0;
    RegCorredores corredor;
    while (fread(&corredor, sizeof(RegCorredores), 1, archCorredores)) {
        char localidad[40] = "N/D";
        for (int i = 0; i < numCiudades; i++) {
            if (ciudades[i].numero == corredor.numero) {
                strcpy(localidad, ciudades[i].localidad);
                break;
            }
        }
        Corredor* lista;
        int* count;
        if (strstr(corredor.categoria, "Clasica") || strstr(corredor.categoria, "Clásica")) {
            lista = clasica; count = &numClasica;
        } else if (strstr(corredor.categoria, "NonStop") || strstr(corredor.categoria, "Nonstop") || strstr(corredor.categoria, "Non Stop")) {
            lista = nonStop; count = &numNonStop;
        } else {
            continue;
        }
        lista[*count].numero = corredor.numero;
        strcpy(lista[*count].nombre, corredor.nombreApellido);
        strcpy(lista[*count].categoria, corredor.categoria);
        lista[*count].genero = corredor.genero;
        strcpy(lista[*count].localidad, localidad);
        lista[*count].segundos = tiempoEnSegundos(corredor.llegada);
        if (lista[*count].segundos < 0) {
            strcpy(lista[*count].tiempo, "No Termino");
        } else {
            int h = lista[*count].segundos / 3600;
            int m = ((int)lista[*count].segundos % 3600) / 60;
            int s = (int)lista[*count].segundos % 60;
            snprintf(lista[*count].tiempo, sizeof(lista[*count].tiempo), "%02d:%02d:%02d", h, m, s);
        }
        (*count)++;
    }
    auto procesarCarrera = [](Corredor corredores[], int count) {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                bool swap = false;
                if (corredores[j].segundos < 0 && corredores[j + 1].segundos >= 0) {
                    swap = true;
                } else if (corredores[j].segundos >= 0 && corredores[j + 1].segundos >= 0) {
                    if (corredores[j].segundos > corredores[j + 1].segundos) swap = true;
                }
                if (swap) {
                    Corredor temp = corredores[j];
                    corredores[j] = corredores[j + 1];
                    corredores[j + 1] = temp;
                }
            }
        }
        int posM = 1, posF = 1;
        float primerTiempo = -1, tiempoAnterior = -1;
        for(int i = 0; i < count; i++){
            if(corredores[i].segundos < 0){
                corredores[i].posGral = 0;
                corredores[i].posGen = 0;
                strcpy(corredores[i].diffPrimero, "-");
                strcpy(corredores[i].diffAnterior, "-");
            } else {
                corredores[i].posGral = i + 1;
                corredores[i].posGen = (corredores[i].genero == 'M') ? posM++ : posF++;
                if (primerTiempo < 0) primerTiempo = corredores[i].segundos;
                if (corredores[i].posGral == 1) {
                    strcpy(corredores[i].diffPrimero, "-");
                    strcpy(corredores[i].diffAnterior, "-");
                } else {
                    float diffP = corredores[i].segundos - primerTiempo;
                    snprintf(corredores[i].diffPrimero, sizeof(corredores[i].diffPrimero), "%02d:%02d:%02d", (int)diffP/3600, ((int)diffP%3600)/60, (int)diffP%60);
                    float diffA = corredores[i].segundos - tiempoAnterior;
                    snprintf(corredores[i].diffAnterior, sizeof(corredores[i].diffAnterior), "%02d:%02d:%02d", (int)diffA/3600, ((int)diffA%3600)/60, (int)diffA%60);
                }
                tiempoAnterior = corredores[i].segundos;
            }
        }
    };
    procesarCarrera(clasica, numClasica);
    procesarCarrera(nonStop, numNonStop);
    auto mostrarCarrera = [](Corredor corredores[], int count, const char* nombre) {
        cout << "\n" << nombre << endl;
        printf("===================================================================================================================================\n");
        printf("%-5s %-5s %-5s %-4s %-23s %-38s %-7s %-15s %-12s %-12s %-12s\n", "Pos.", "Pos.", "Pos.", "Nro", "Nombre", "Categoria", "Genero", "Localidad", "Total", "Dif.", "Dif.");
        printf("%-5s %-5s %-5s %-4s %-23s %-38s %-7s %-15s %-12s %-12s %-12s\n", "Gral", "Gen", "Cat", "", "", "", "", "", "", "Primero", "Anterior");
        printf("===================================================================================================================================\n");
        for (int i = 0; i < count; i++) {
            // if (corredores[i].segundos < 0) continue;
            
            // Sanitizamos los datos de esta tabla también por las dudas
            sanitizarCadena(corredores[i].nombre);
            sanitizarCadena(corredores[i].categoria);
            sanitizarCadena(corredores[i].localidad);

            char posG[10], posGe[10];
            if (corredores[i].posGral == 0) {
                strcpy(posG, "-"); strcpy(posGe, "-");
            } else {
                snprintf(posG, sizeof(posG), "%d", corredores[i].posGral);
                snprintf(posGe, sizeof(posGe), "%d", corredores[i].posGen);
            }
            printf("%-5s %-5s %-5s %-4d %-23.23s %-38.38s %-7c %-15.15s %-12s %-12s %-12s\n",
                   posG, posGe, "-", corredores[i].numero, corredores[i].nombre, corredores[i].categoria,
                   corredores[i].genero, corredores[i].localidad, corredores[i].tiempo,
                   corredores[i].diffPrimero, corredores[i].diffAnterior);
        }
        printf("===================================================================================================================================\n");
    };
    mostrarCarrera(clasica, numClasica, "CARRERA CLASICA");
    mostrarCarrera(nonStop, numNonStop, "CARRERA NONSTOP");
    fclose(archCorredores);
}