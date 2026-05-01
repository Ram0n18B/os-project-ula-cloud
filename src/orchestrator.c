#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "orchestrator.h"

/**
 * TODO: Implementar el despliegue del microservicio.
 * * Requisitos:
 * 1. Crear un nuevo proceso.
 * 2. En el proceso HIJO:
 * - Configurar las restricciones de recursos (memoria).
 * - Reemplazar la imagen del proceso por el binario del servicio.
 * 3. En el proceso PADRE:
 * - Registrar el PID y el estado inicial en el dashboard.
 * - Retornar el PID asignado.
 */
int spawn_service(int index) {
    pid_t pid;

    // TODO: Invocar la creación del proceso hijo.

    // Casos a manejar:
    // - Error en la creación del proceso.
    // - Lógica del proceso HIJO (Setup de límites y Ejecución).
    // - Lógica del proceso PADRE (Gestión del dashboard).

    pid = fork();
    switch (pid) {
    case -1:
        perror("Could not create process");
         exit(EXIT_FAILURE);
    case 0:
        apply_resource_limits(dashboard[index].mem_limit);
        int exit_val = execvp(dashboard[index].path, NULL);
        perror("Could not execute service\n");
        _exit(exit_val);
    default:
        pthread_mutex_lock(&dashboard_mutex);
        dashboard[index].pid = pid;
        dashboard[index].state = STATE_RUNNING;
        pthread_mutex_unlock(&dashboard_mutex);
    }
        
    return dashboard[index].pid;
}
