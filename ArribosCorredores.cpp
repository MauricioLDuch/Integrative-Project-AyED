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
    char llegada[11];
};

/* Functions */
void reportePorLocalidad();
void reportePorCarrera();
float tiempoEnSegundos(char llegada[]);

/* Main */
int main(){
    reportePorLocalidad();
    cout << "\n\n";
    reportePorCarrera();
    return 0;
}

/* Functional development */
float tiempoEnSegundos(char llegada[]){
    if (strncmp(llegada, "DNF", 3) == 0 || strncmp(llegada, "NL", 2) == 0 || strncmp(llegada, "FE", 2) == 0){
        return -1;
    }
    int horas = (llegada[0] - '0') * 10 + (llegada[1] - '0');
    int minutos = (llegada[3] - '0') * 10 + (llegada[4] - '0');
    int segundos = (llegada[6] - '0') * 10 + (llegada[7] - '0');
    return horas * 3600 + minutos * 60 + segundos;
}

void reportePorLocalidad() {
    FILE* archCorredores = fopen("4Refugios/Archivo corredores 4Refugios.bin", "rb");
    FILE* archCiudades = fopen("4Refugios/Ciudades.bin", "rb");

    if(!archCorredores || !archCiudades){
        cout << "Error al abrir archivos" << endl;
        return;
    }

    // Leer ciudades
    CorredoresCiudad ciudades[1000];
    int numCiudades = 0;
    CorredoresCiudad ciudad;
    
    fseek(archCiudades, 0, SEEK_SET);
    while(fread(&ciudad, sizeof(CorredoresCiudad), 1, archCiudades) && numCiudades < 1000){
        ciudades[numCiudades++] = ciudad;
    }

    // Procesar estadísticas
    struct Stat { 
        char localidad[40], ciudad[11]; 
        int cantidad; 
        float sumaTiempos; 
    };
    
    Stat stats[500]; 
    int numStats = 0;
    RegCorredores corredor;

    fseek(archCorredores, 0, SEEK_SET);
    while(fread(&corredor, sizeof(RegCorredores), 1, archCorredores)){
        float tiempo = tiempoEnSegundos(corredor.llegada);
        if(tiempo > 0){
            // Buscar ciudad del corredor
            for(int i = 0; i < numCiudades; i++){
                if(ciudades[i].numero == corredor.numero){
                    // Buscar en stats
                    bool encontrado = false;
                    for(int j = 0; j < numStats; j++){
                        if(strcmp(stats[j].localidad, ciudades[i].localidad) == 0 && 
                           strcmp(stats[j].ciudad, ciudades[i].ciudad) == 0){
                            stats[j].cantidad++;
                            stats[j].sumaTiempos += tiempo;
                            encontrado = true;
                            break;
                        }
                    }
                    if(!encontrado && numStats < 500){
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

    // Mostrar reporte
    cout << "| Localidad    | Ciudad    | Cantidad de Corredores | Tiempo Promedio |" << endl;
    cout << "|--------------|-----------|------------------------|-----------------|" << endl;
    
    char localidadActual[40] = "";
    int totalLocalidad = 0;
    float totalTiempo = 0;

    for(int i = 0; i < numStats; i++){
        if(strcmp(localidadActual, stats[i].localidad) != 0 && i > 0){
            float prom = totalTiempo / totalLocalidad;
            int h = prom / 3600;
            int m = ((int)prom % 3600) / 60;
            int s = (int)prom % 60;
            cout << "| Total " << localidadActual << " | | " << totalLocalidad 
                 << " | " << h << ":" << (m < 10 ? "0" : "") << m << ":" << (s < 10 ? "0" : "") << s << " |" << endl;
            cout << "|--------------|-----------|------------------------|-----------------|" << endl;
            totalLocalidad = 0;
            totalTiempo = 0;
        }
        
        float promedio = stats[i].sumaTiempos / stats[i].cantidad;
        int h = promedio / 3600;
        int m = ((int)promedio % 3600) / 60;
        int s = (int)promedio % 60;
        
        cout << "| " << stats[i].localidad << " | " << stats[i].ciudad << " | " 
             << stats[i].cantidad << " | " << h << ":" << (m < 10 ? "0" : "") << m << ":" << (s < 10 ? "0" : "") << s << " |" << endl;
        
        strcpy(localidadActual, stats[i].localidad);
        totalLocalidad += stats[i].cantidad;
        totalTiempo += stats[i].sumaTiempos;
    }

    if(numStats > 0){
        float prom = totalTiempo / totalLocalidad;
        int h = prom / 3600;
        int m = ((int)prom % 3600) / 60;
        int s = (int)prom % 60;
        cout << "| Total " << localidadActual << " | | " << totalLocalidad 
             << " | " << h << ":" << (m < 10 ? "0" : "") << m << ":" << (s < 10 ? "0" : "") << s << " |" << endl;
    }

    fclose(archCorredores);
    fclose(archCiudades);
}

void reportePorCarrera() {
    FILE* archCorredores = fopen("4Refugios/Archivo corredores 4Refugios.bin", "rb");
    FILE* archCiudades = fopen("4Refugios/Ciudades.bin", "rb");

    if(!archCorredores || !archCiudades){
        cout << "Error al abrir archivos" << endl;
        return;
    }

    // Leer ciudades
    CorredoresCiudad ciudades[1000];
    int numCiudades = 0;
    CorredoresCiudad ciudad;
    
    fseek(archCiudades, 0, SEEK_SET);
    while(fread(&ciudad, sizeof(CorredoresCiudad), 1, archCiudades) && numCiudades < 1000){
        ciudades[numCiudades++] = ciudad;
    }

    // Separar corredores por carrera
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

    fseek(archCorredores, 0, SEEK_SET);
    while(fread(&corredor, sizeof(RegCorredores), 1, archCorredores)){
        // Buscar localidad
        char localidad[40] = "";
        for(int i = 0; i < numCiudades; i++){
            if(ciudades[i].numero == corredor.numero){
                strcpy(localidad, ciudades[i].localidad);
                break;
            }
        }

        // Determinar carrera - versión más flexible
        Corredor* lista; 
        int* count;
        
        // Buscar "Clásica" con tilde y también "Clasica" sin tilde
        if(strstr(corredor.categoria, "Clásica") != NULL || strstr(corredor.categoria, "Clasica") != NULL){
            lista = clasica; 
            count = &numClasica;
        } 
        // Buscar "NonStop" y también variaciones
        else if(strstr(corredor.categoria, "NonStop") != NULL || strstr(corredor.categoria, "Nonstop") != NULL || strstr(corredor.categoria, "Non Stop") != NULL){
            lista = nonStop; 
            count = &numNonStop;
        } else {
            continue;
        }

        lista[*count].numero = corredor.numero;
        strcpy(lista[*count].nombre, corredor.nombreApellido);
        strcpy(lista[*count].categoria, corredor.categoria);
        lista[*count].genero = corredor.genero;
        strcpy(lista[*count].localidad, localidad);
        
        lista[*count].segundos = tiempoEnSegundos(corredor.llegada);
        if(lista[*count].segundos < 0){
            strcpy(lista[*count].tiempo, "No Termino");
        } else {
            int h = lista[*count].segundos / 3600;
            int m = ((int)lista[*count].segundos % 3600) / 60;
            int s = (int)lista[*count].segundos % 60;
            snprintf(lista[*count].tiempo, sizeof(lista[*count].tiempo), "%02d:%02d:%02d", h, m, s);
        }
        (*count)++;
    }

    // Función para procesar una carrera (calcular posiciones y diferencias)
    auto procesarCarrera = [](Corredor corredores[], int count) {
        // Ordenar por tiempo (los que terminaron primero, luego los que no)
        for(int i = 0; i < count-1; i++){
            for(int j = 0; j < count-i-1; j++){
                bool swapNeeded = false;
                
                if(corredores[j].segundos < 0 && corredores[j+1].segundos >= 0){
                    swapNeeded = true;
                } else if(corredores[j].segundos >= 0 && corredores[j+1].segundos >= 0){
                    if(corredores[j].segundos > corredores[j+1].segundos){
                        swapNeeded = true;
                    }
                }
                
                if(swapNeeded){
                    Corredor temp = corredores[j];
                    corredores[j] = corredores[j+1];
                    corredores[j+1] = temp;
                }
            }
        }

        // Calcular posiciones generales
        for(int i = 0; i < count; i++){
            if(corredores[i].segundos < 0){
                corredores[i].posGral = 0;
                strcpy(corredores[i].diffPrimero, "-");
                strcpy(corredores[i].diffAnterior, "-");
            } else {
                corredores[i].posGral = i + 1;
            }
        }

        // Calcular posiciones por género
        for(int i = 0; i < count; i++){
            if(corredores[i].segundos < 0){
                corredores[i].posGen = 0;
                continue;
            }
            
            int posGen = 1;
            for(int j = 0; j < i; j++){
                if(corredores[j].segundos >= 0 && corredores[j].genero == corredores[i].genero){
                    posGen++;
                }
            }
            corredores[i].posGen = posGen;
        }

        // Calcular posiciones por categoría
        for(int i = 0; i < count; i++){
            if(corredores[i].segundos < 0){
                corredores[i].posCat = 0;
                continue;
            }
            
            int posCat = 1;
            for(int j = 0; j < i; j++){
                if(corredores[j].segundos >= 0 && strcmp(corredores[j].categoria, corredores[i].categoria) == 0){
                    posCat++;
                }
            }
            corredores[i].posCat = posCat;
        }

        // Calcular diferencias
        float primerTiempo = -1;
        float tiempoAnterior = -1;
        
        for(int i = 0; i < count; i++){
            if(corredores[i].segundos >= 0){
                if(primerTiempo < 0){
                    primerTiempo = corredores[i].segundos;
                    strcpy(corredores[i].diffPrimero, "-");
                    strcpy(corredores[i].diffAnterior, "-");
                } else {
                    float diff = corredores[i].segundos - primerTiempo;
                    int h = diff / 3600;
                    int m = ((int)diff % 3600) / 60;
                    int s = (int)diff % 60;
                    snprintf(corredores[i].diffPrimero, sizeof(corredores[i].diffPrimero), "%02d:%02d:%02d", h, m, s);
                    
                    diff = corredores[i].segundos - tiempoAnterior;
                    h = diff / 3600;
                    m = ((int)diff % 3600) / 60;
                    s = (int)diff % 60;
                    snprintf(corredores[i].diffAnterior, sizeof(corredores[i].diffAnterior), "%02d:%02d:%02d", h, m, s);
                }
                tiempoAnterior = corredores[i].segundos;
            }
        }
    };

    // Procesar cada carrera
    procesarCarrera(clasica, numClasica);
    procesarCarrera(nonStop, numNonStop);

    // Mostrar reportes
    auto mostrarCarrera = [](Corredor corredores[], int count, const char* nombre) {
        cout << "\n" << nombre << endl;
        cout << "=====================================================================================================================" << endl;
        cout << "Pos. Pos. Pos. N°  Nombre                Categoria                           Género Localidad     Total        Dif.      Dif." << endl;
        cout << "Gral. Gen. Cat.                                                                                          primero    anterior" << endl;
        cout << "=====================================================================================================================" << endl;
        
        for(int i = 0; i < count; i++){
            char posG[10], posGe[10], posC[10];
            if(corredores[i].posGral == 0){
                strcpy(posG, "-"); strcpy(posGe, "-"); strcpy(posC, "-");
            } else {
                snprintf(posG, sizeof(posG), "%d", corredores[i].posGral);
                snprintf(posGe, sizeof(posGe), "%d", corredores[i].posGen);
                snprintf(posC, sizeof(posC), "%d", corredores[i].posCat);
            }
            
            // Truncar nombres y categorías largas para que quepa en la tabla
            char nombreTrunc[25];
            char catTrunc[35];
            
            strncpy(nombreTrunc, corredores[i].nombre, 24);
            nombreTrunc[24] = '\0';
            
            strncpy(catTrunc, corredores[i].categoria, 34);
            catTrunc[34] = '\0';
            
            printf("%-5s %-5s %-5s %-4d %-23s %-34s %-6c %-12s %-11s %-9s %-9s\n",
                   posG, posGe, posC, corredores[i].numero, nombreTrunc, catTrunc,
                   corredores[i].genero, corredores[i].localidad, corredores[i].tiempo,
                   corredores[i].diffPrimero, corredores[i].diffAnterior);
        }
        cout << "=====================================================================================================================" << endl;
    };

    // Mostrar ambas carreras
    if(numClasica > 0){
        mostrarCarrera(clasica, numClasica, "CARRERA CLÁSICA");
    } else {
        cout << "\nCARRERA CLÁSICA - No hay corredores" << endl;
    }
    
    if(numNonStop > 0){
        mostrarCarrera(nonStop, numNonStop, "CARRERA NONSTOP");
    } else {
        cout << "\nCARRERA NONSTOP - No hay corredores" << endl;
    }

    fclose(archCorredores);
    fclose(archCiudades);
}