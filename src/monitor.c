#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include "orchestrator.h"

/**
 * TODO: Implementar la rutina del hilo monitor (Watchdog).
 * * Requisitos:
 * 1. Recuperar la estructura del servicio desde el argumento 'arg'.
 * 2. Utilizar 'waitpid' de forma BLOQUEANTE para esperar al proceso hijo.
 * 3. Analizar el estado de terminación (éxito, error o señal del sistema).
 * 4. Actualizar el Dashboard Global de forma SEGURA (evitar condiciones de carrera).
 */
void* monitor_service(void *arg) {
    // TODO: Castear el argumento al tipo de dato correcto.
    service_t *service = (service_t *) arg;

    // TODO: Implementar la espera del proceso específico.
    // Ayuda: Revisar el uso de waitpid(pid, &status, 0).
    pthread_mutex_lock(&dashboard_mutex);
    pid_t pid = service->pid;
    pthread_mutex_unlock(&dashboard_mutex);
    int status = 0;
    waitpid(pid, &status, 0);


    /* * Una vez que waitpid retorna, el proceso hijo ha cambiado de estado.
     * TODO: Analizar el 'status' usando las macros de sys/wait.h:
     * - WIFEXITED: ¿Terminó normalmente?
     * - WEXITSTATUS: ¿Cuál fue su código de retorno?
     * - WIFSIGNALED: ¿Fue terminado por una señal (Segfault, OOM Killer)?
     * - WTERMSIG: ¿Qué señal lo mató?
     */
    service_state_t svst = STATE_RUNNING;
    int exit_code = 0;
     if(WIFEXITED(status))
    {
        exit_code = WEXITSTATUS(status);
        switch (exit_code)
        {
            case EXIT_SUCCESS:
                svst = STATE_STOPPED;
                break;
            case EXIT_FAILURE:
                svst = STATE_CRASHED;
                kill(service->pid, SIGKILL);
                break;
        }
    }
    if(WIFSIGNALED(status))
    {
        exit_code = WTERMSIG(status);
        switch(exit_code)
        {
            case SIGSEGV:
            case SIGKILL:
            case SIGFPE:
                svst = STATE_KILLED;
                break;
        }
    }

    /*
     * TODO: Actualizar el dashboard global.
     * ¡CRÍTICO!: El acceso al array 'dashboard' debe estar protegido. 
     * No olvides liberar el mecanismo de sincronización al terminar.
     */
    pthread_mutex_lock(&dashboard_mutex);
    service->state = svst;
    service->exit_status = exit_code;
    pthread_mutex_unlock(&dashboard_mutex);
    return NULL;
}
