#ifndef LIBCONSULTA_H
#define LIBCONSULTA_H

#include <mariadb/mysql.h>

void insertar(int matricula, const char *nombre, const char *pap, const char *sap, const char *materia, const char *calificacion, const char *carrera);

#endif
