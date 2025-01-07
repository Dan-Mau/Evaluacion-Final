
#include "libConsulta.h"

#include <stdio.h>

void insertar(const int matricula, const char *nombre, const char *pap, const char *sap, const char *materia, const char *calificacion, const char *carrera) {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Error al inicializar MySQL: %s\n", mysql_error(conn));
        return;
    }

    if (mysql_real_connect(conn, "localhost", "root", "root", "escuela", 0, NULL, 0) == NULL) {
        fprintf(stderr, "Error al conectar a la base de datos: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO alumno (matricula, nombre, Pap, Sap, materia, calificacion, carrera) VALUES ('%d', '%s', '%s', '%s', '%s', '%s', '%s')",
             matricula, nombre, pap, sap, materia, calificacion, carrera);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Error al insertar el alumno: %s\n", mysql_error(conn));
    } else {
        printf("Alumno insertado correctamente.\n");
    }

    mysql_close(conn);
}
