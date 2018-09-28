//*****************************************************************************
// 
// Fichero: debug_repo.h
// Proposito:
//   Repositorio de mensajes de depuración con envío a una consola mediante USART
// 
//*****************************************************************************

#ifndef __DEBUG_REPO_H
#define __DEBUG_REPO_H

#include <stdint.h>
#include "usart.h"
#include "cmsis_os.h"

#define DEBUGREPO_UART_HANDLE 				huart6

#define DEBUGREPO_SEND_STATS
#define DEBUGREPO_SEND_HANDLERS_STATS
#define DEBUGREPO_SEND_STATS_EVERY                      100
#define DEBUGREPO_SEND_HANDLERS_STATS_EVERY             100

// Para utilizar un mutex que permita serializar el acceso a la cola compartida como sección crítica
// basta con eliminar la marca de comentario de la definición de la macro USE_MUTEX:
#define DEBUGREPO_USE_MUTEX

// Para estresar al sistema mientras se insertan mensajes en la cola 
// (y forzar así conflictos de acceso a la cola entre las diferentes tareas) 
// basta con eliminar la marca de comentario de la definición de la macro USE_DELAY
//#define DEBUGREPO_USE_DELAY

#define DEBUGREPO_INSERT_DELAY_TICKS         1
#define DEBUGREPO_EXTRACT_DELAY_TICKS        10


/* Numero maximo de bytes almacenados en el repositorio (tareas)        */
#define DEBUGREPO_SIZE	                        (4*1024)
/* Numero maximo de bytes almacenados en el repositorio de manejadores 
  (uno por nivel de interrupcion)                                       */
#define DEBUGREPO_HANDLERS_SIZE	                (512)

#define DEBUGREPO_MSG_MAXLEN			(512)
#define DEBUGREPO_HANDLERS_FINAL_MASK 	        ((uint32_t)0x0000007FU)

/* Estadísticas de uso / errores                 */
struct s_debugrepo_stats {
    uint32_t items, total_items, insert_errors;
};
typedef struct s_debugrepo_stats t_debugrepo_stats;


void debugrepoInit (void);
int debugrepoInsertMsg (const char *msg);
int debugrepoInsertMsgLen (const char *msg, uint16_t len);
int debugrepoInsertBinMsgLen (const char *msg, uint16_t len);
int debugrepoExtractMsg (char msg[], int maxlen);
void debugrepoUARTCallback(UART_HandleTypeDef *huart);

#endif // __DEBUG_REPO_H
