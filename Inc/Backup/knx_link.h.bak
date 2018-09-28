/**
 * @file knx_link.h
 * @author PON TU NOMBRE AQUÃ�
 * @date OtoÃ±o 2017
 *
 * @brief ImplementaciÃ³n mÃ­nima del nivel de enlace KNX suficiente para el desarrollo del nivel fÃ­sico KNX completo
 *
 * En este momento la implementaciÃ³n del nivel de enlace no es relevante.
 * Lo Ãºnico que necesitamos para completar la implementaciÃ³n del nivel fÃ­sico es la
 * gestiÃ³n de los parÃ¡metros de nivel de enlace.
 *
 * ParÃ¡metros:
 * - Una Ãºnica direcciÃ³n individual
 * - Una Ãºnica direcciÃ³n de polling consistente en una direcciÃ³n de grupo de polling y un slot number
 * - Una colecciÃ³n de direcciones de grupo
 * - Un estado del nivel de enlace
 *
 * @{
 */
#ifndef __KNX_LINK_H
#define __KNX_LINK_H

/* ---------------- #includes necesarios para este fichero ----------------- */
#include <stdint.h>     // Para los tipos uintXX_t

/* --------------------------- Macros pÃºblicas ----------------------------- */

/* ----------------------- Tipos de datos pÃºblicos ------------------------- */


/**
 * Tipo enumerado con los posibles estados de KNX enlace
 */
enum knx_link_comm_state_e { 
    KNX_LINK_ILLEGAL_STATE,         /**< Valor de inicializaciÃ³n             */
    KNX_LINK_INIT_STATE,            /**< Haciendo reset de la TP-UART        */
    KNX_LINK_NORMAL_STATE,          /**< Modo normal de trabajo, 
                                         podemos enviar/recibir tramas */
    KNX_LINK_STOP_STATE,            /**< Modo stop / error                   */
    KNX_LINK_MONITOR_STATE          /**< Modo monitor (no implementado)      */
};
/**
 * RedefiniciÃ³n con typedef para usar una Ãºnica palabra
 */
typedef enum knx_link_comm_state_e knx_link_comm_state_t;

/* ----------------- DeclaraciÃ³n de funciones pÃºblicas --------------------- */

/**
 * @brief Obtener direcciÃ³n individual
 *
 * Esta funciÃ³n simplemente retorna la direcciÃ³n individual almacenada sin
 * ningÃºn tipo de control de errores (ej: direcciÃ³n no inicializada)
 *
 * @returns DirecciÃ³n individual de este sistema
 */
uint16_t knx_link_get_ind_address (void);



/**
 * @brief Obtener direcciÃ³n de grupo de polling
 *
 * Esta funciÃ³n simplemente retorna la direcciÃ³n de grupo de polling
 * almacenada sin ningÃºn tipo de control de errores (ej: direcciÃ³n no inicializada)
 *
 * @returns DirecciÃ³n de grupo de polling de este sistema
 */
uint16_t knx_link_get_poll_grp_address (void);

/**
 * @brief Obtener el slot number de polling
 *
 * Esta funciÃ³n simplemente retorna el slot number de polling
 * almacenada sin ningÃºn tipo de control de errores (ej: valor no inicializado)
 *
 * @returns Slot number de polling de este sistema
 */
uint16_t knx_link_get_poll_slot_number (void);



/**
 * @brief AÃ±adir una nueva direcciÃ³n de grupo al sistema
 * @param[in] grp_address DirecciÃ³n de grupo a aÃ±adir
 *
 * Esta funciÃ³n almacena la direcciÃ³n de grupo grp_address en la tabla
 * de direcciones de grupo del nivel de enlace, sin comprobar
 * si la direcciÃ³n es vÃ¡lida o ya estÃ¡ en el sistema.
 *
 * @returns 0 Falta memoria
 * @returns 1 OperaciÃ³n terminada con Ã©xito
 */
uint32_t knx_link_add_grp_address (uint16_t grp_address); 

/**
 * @brief Buscar una direcciÃ³n de grupo entre las almacenadas en el sistema
 * @param[in] grp_address DirecciÃ³n de grupo a buscar
 *
 * Esta funciÃ³n busca la direcciÃ³n de grupo grp_address en la tabla
 * de direcciones de grupo del nivel de enlace.
 *
 * @returns 0 DirecciÃ³n no encontrada
 * @returns 1 DirecciÃ³n encontrada
 */
uint32_t knx_link_exists_grp_address (uint16_t grp_address); 



/**
 * @brief Obtener el estado del nivel de enlace
 *
 * Esta funciÃ³n retorna el estado del nivel de enlace sin hacer comprobaciÃ³n
 * de errores (ej: valor no inicializado).
 *
 * @returns Estado actual del nivel de enlace
 */
knx_link_comm_state_t knx_link_get_comm_state (void);



/**
 * @brief Inicializar el nivel de enlace
 * @param[in] ind_address DirecciÃ³n individual de este sistema
 * @param[in] poll_grp_address DirecciÃ³n de grupo de polling de este sistema
 * @param[in] poll_slot_number Slot number de polling de este sistema
 *
 * Esta funciÃ³n inicializa los parÃ¡metros del nivel de enlace e intenta inicializar
 * la TP-UART utilizando el servicio correspondiente del nivel fÃ­sico.
 * En la inicializaciÃ³n de los parÃ¡metros no se hace ningÃºn tipo de comprobaciÃ³n de errores
 *
 * @warning Esta funciÃ³n debe ser la primera utilizada del nivel de enlace por los niveles superiores
 *
 * @returns Nada
 */
void knx_link_init (uint16_t ind_address, uint16_t poll_grp_address, uint16_t poll_slot_number);


/* @} */

#endif // __KNX_LINK_H
