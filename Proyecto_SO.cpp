#include <mosquitto.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <limits>
#include "libConsulta.h"
#include <unistd.h>
#include <cstring>

using namespace std;

struct Alumno {
    int matricula;
    char nombre[50];
    char pap[50];
    char sap[50];
    char materia[50];
    char calificacion[5];
    char carrera[50];
};

void ingresar_datos_alumno(Alumno &alumno) {
    string input;

    cout << "Ingresa la matricula: ";
    while (true) {
        getline(cin, input);
        try {
            alumno.matricula = stoi(input);
            if (alumno.matricula > 0) break;
        } catch (exception &e) {
            
        }
        cout << "Error. Ingresa una matrícula válida: ";
    }

    cout << "Ingresa el nombre: ";
    getline(cin, input);
    strncpy(alumno.nombre, input.c_str(), sizeof(alumno.nombre) - 1);
    alumno.nombre[sizeof(alumno.nombre) - 1] = '\0';

    cout << "Ingresa el primer apellido: ";
    getline(cin, input);
    strncpy(alumno.pap, input.c_str(), sizeof(alumno.pap) - 1);
    alumno.pap[sizeof(alumno.pap) - 1] = '\0';

    cout << "Ingresa el segundo apellido: ";
    getline(cin, input);
    strncpy(alumno.sap, input.c_str(), sizeof(alumno.sap) - 1);
    alumno.sap[sizeof(alumno.sap) - 1] = '\0';

    cout << "Ingresa la materia: ";
    getline(cin, input);
    strncpy(alumno.materia, input.c_str(), sizeof(alumno.materia) - 1);
    alumno.materia[sizeof(alumno.materia) - 1] = '\0';

    cout << "Ingresa la calificación (0-100): ";
    while (true) {
        getline(cin, input);
        try {
            int cal = stoi(input);
            if (cal >= 0 && cal <= 100) {
                snprintf(alumno.calificacion, sizeof(alumno.calificacion), "%d", cal);
                break;
            }
        } catch (exception &e) {
            
        }
        cout << "Error. Ingresa una calificación válida (0-100): ";
    }

    cout << "Ingresa la carrera: ";
    getline(cin, input);
    strncpy(alumno.carrera, input.c_str(), sizeof(alumno.carrera) - 1);
    alumno.carrera[sizeof(alumno.carrera) - 1] = '\0';
}

void mostrar_datos_alumno(const Alumno &alumno) {
    cout << "\nDatos del alumno ingresados:" << endl;
    cout << "Matricula: " << alumno.matricula << endl;
    cout << "Nombre: " << alumno.nombre << endl;
    cout << "Primer Apellido: " << alumno.pap << endl;
    cout << "Segundo Apellido: " << alumno.sap << endl;
    cout << "Materia: " << alumno.materia << endl;
    cout << "Calificación: " << alumno.calificacion << endl;
    cout << "Carrera: " << alumno.carrera << endl;
}

void *productor(void *arg) {
    const char *topic = "mau/billy";
    int qos = 1;
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);

    if (!mosq) {
        cerr << "Error al inicializar Mosquitto." << endl;
        return NULL;
    }

    if (mosquitto_connect(mosq, "localhost", 1883, 60)) {
        cerr << "Error al conectar al broker MQTT." << endl;
        mosquitto_destroy(mosq);
        return NULL;
    }

    while (true) {
        Alumno alumno;
        ingresar_datos_alumno(alumno);
        mostrar_datos_alumno(alumno);

        char confirmar;
        cout << "\n¿Deseas publicar estos datos? (s/n): ";
        cin >> confirmar;
        cin.ignore();

        if (confirmar == 's' || confirmar == 'S') {
            string mensaje = to_string(alumno.matricula) + "," + alumno.nombre + "," + alumno.pap + "," + alumno.sap + "," + alumno.materia + "," + alumno.calificacion + "," + alumno.carrera;

            if (mosquitto_publish(mosq, NULL, topic, mensaje.length(), mensaje.c_str(), qos, false)) {
                cerr << "Error al publicar el mensaje." << endl;
            } else {
                cout << "Mensaje publicado: " << mensaje << endl;
            }
        } else {
            cout << "Publicación cancelada." << endl;
        }

        sleep(1);
    }

    mosquitto_destroy(mosq);
    return NULL;
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    cout << "Mensaje recibido en el tema '" << msg->topic << "': " << (char *)msg->payload << endl;

    Alumno alumno;
    if (sscanf((const char *)msg->payload, "%d,%49[^,],%49[^,],%49[^,],%49[^,],%4[^,],%49[^\n]",
               &alumno.matricula, alumno.nombre, alumno.pap, alumno.sap, alumno.materia, alumno.calificacion, alumno.carrera) == 7) {
        cout << "Datos del alumno: Matricula = " << alumno.matricula << ", Nombre = " << alumno.nombre
             << ", Pap = " << alumno.pap << ", Sap = " << alumno.sap << ", Materia = " << alumno.materia
             << ", Calificacion = " << alumno.calificacion << ", Carrera = " << alumno.carrera << endl;

        
        
            insertar(alumno.matricula, alumno.nombre, alumno.pap, alumno.sap, alumno.materia, alumno.calificacion, alumno.carrera);
            
        
            
  
    } else {
        cerr << "Error al parsear el mensaje." << endl;
    }
}

void *consumidor(void *arg) {
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        cerr << "Error al inicializar Mosquitto." << endl;
        return NULL;
    }

    if (mosquitto_connect(mosq, "localhost", 1883, 60)) {
        cerr << "Error al conectar al broker MQTT." << endl;
        mosquitto_destroy(mosq);
        return NULL;
    }

    mosquitto_message_callback_set(mosq, on_message);

    const char *topic = "mau/billy";
    int qos = 1;
    if (mosquitto_subscribe(mosq, NULL, topic, qos)) {
        cerr << "Error al suscribirse al tema." << endl;
        mosquitto_destroy(mosq);
        return NULL;
    }

    mosquitto_loop_forever(mosq, -1, 1);
    mosquitto_destroy(mosq);
    return NULL;
}

int main() {
    mosquitto_lib_init();

    pthread_t hilo_productor, hilo_consumidor;
    pthread_create(&hilo_productor, NULL, productor, NULL);
    pthread_create(&hilo_consumidor, NULL, consumidor, NULL);

    pthread_join(hilo_productor, NULL);
    pthread_join(hilo_consumidor, NULL);

    mosquitto_lib_cleanup();

    return 0;
}
