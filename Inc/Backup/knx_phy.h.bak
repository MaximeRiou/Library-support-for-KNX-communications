/**
 * @file knx_phy.h
 * @author PON TU NOMBRE AQUÍ
 * @date Otoño 2017
 *
 * @brief Implementación completa del nivel físico KNX
 *
 *
 * Servicios de nivel físico:
 * - Inicialización (reset): Primitivas
 *   1 - Request
 *       Función pública knx_phy_reset_req
 *   2 - Confirmation
 *       Cola FreeRTOS pública knx_phy_reset_con
 *   
 * Utilizaremos la capa HAL para la transmisión / recepción a la UART,
 * y un timer TIM en modo básico para el time-out del reset de la TP-UART
 *
 * @{
 */
#ifndef __KNX_PHY_H
#define __KNX_PHY_H

/* ---------------- #includes necesarios para este fichero ----------------- */
#include <stdint.h>        // Para los tipos uintXX_t
#include "FreeRTOS.h"      // FreeRTOS + capa CMSIS_OS (declaraciones semáforos y colas)  
#include "queue.h"
#include "cmsis_os.h"


/* --------------------------- Macros públicas ----------------------------- */

/* Valores asociados a knx_phy_reset_req() */
#define KNX_PHY_RESET_REQ_OK        ((uint32_t)1) /**< Solicitud Ph_reset.req() correcta */
#define KNX_PHY_RESET_REQ_ERROR     ((uint32_t)0) /**< Error en la solicitud Ph_reset.req(), el estado del nivel de enlace no es INIT (NORMAL, STOP, etc.) */

/* Valores asociados a knx_phy_data_req() */
#define KNX_PHY_DATA_REQ_OK         ((uint32_t)1) /**< Solicitud Ph_data.req() correcta */
#define KNX_PHY_DATA_REQ_ERROR      ((uint32_t)0) /**< Error en la solicitud Ph_data.req(), el estado del nivel de enlace no es NORMAL (INIT, STOP, etc.) */

/* ----------------------- Tipos de datos públicos ------------------------- */

/**
 * Tipo enumerado que implementa p_req_class, posibles clases de solicitud Ph_Data.req()
 */
enum knx_phy_data_req_class_e { 
    KNX_PHY_DATA_REQ_CLASS_START,   /**< El dato es inicio de trama (CTRL)  */
    KNX_PHY_DATA_REQ_CLASS_INNER,   /**< El dato es intermedio              */
    KNX_PHY_DATA_REQ_CLASS_END      /**< El dato es fin de trama (CHK)      */
    /* No utilizados:
    ,
    KNX_PHY_DATA_REQ_CLASS_ACK,
    KNX_PHY_DATA_REQ_CLASS_POLL_DATA,
    KNX_PHY_DATA_REQ_CLASS_FILL
    */
};
/**
 * Redefinición con typedef para usar una única palabra
 */
typedef enum knx_phy_data_req_class_e knx_phy_data_req_class_t;


/**
 * Tipo enumerado que implementa p_con_status, posibles clases de estado Ph_Data.con()
 */
enum knx_phy_data_con_status_e { 
    KNX_PHY_DATA_CON_STATUS_START,    /**< El dato es inicio de trama (CTRL)*/
    KNX_PHY_DATA_CON_STATUS_INNER,    /**< El dato es intermedio            */
    KNX_PHY_DATA_CON_STATUS_END,      /**< El dato es fin de trama (CHK)    */
    KNX_PHY_DATA_CON_STATUS_LDATA_CONFIRM /**< El dato es L_Data.confirm    */
    /* No utilizados:
    ,
    KNX_PHY_DATA_CON_STATUS_OK,
    KNX_PHY_DATA_CON_STATUS_FAILURE
    */
};
/**
 * Redefinición con typedef para usar una única palabra
 */
typedef enum knx_phy_data_con_status_e knx_phy_data_con_status_t;



/**
 * Tipo enumerado que implementa p_ind_class, posibles clases de señalización Ph_Data.ind()
 */
enum knx_phy_data_ind_class_e { 
    KNX_PHY_DATA_IND_CLASS_START,   /**< El dato es inicio de trama (CTRL)  */
    KNX_PHY_DATA_IND_CLASS_INNER,   /**< El dato es intermedio              */
    /* Añadido: */
    KNX_PHY_DATA_IND_CLASS_END      /**< El dato es fin de trama (CHK)      */
    /* No utilizados:
    ,
    KNX_PHY_DATA_REQ_CLASS_ACK,
    KNX_PHY_DATA_REQ_CLASS_POLL_DATA,
    KNX_PHY_DATA_REQ_CLASS_PARITY_ERROR,
    KNX_PHY_DATA_REQ_CLASS_FRAME_ERROR,
    KNX_PHY_DATA_REQ_CLASS_BIT_ERROR,
    */
};
/**
 * Redefinición con typedef para usar una única palabra
 */
typedef enum knx_phy_data_ind_class_e knx_phy_data_ind_class_t;


/* ----------------- Declaración de funciones públicas --------------------- */


/* ----------------- PARTE 1: Callbacks de la capa HAL  -------------------- */

/**
 * Callback de finalización de la transmisión de la UART conectada a la TPUART
 *
 * Este callback es llamado desde el callback general de transmisión terminada
 * de las diferentes UARTs del sistema, o directamente desde la ISR correspondiente, 
 * dependiendo de la implementación elegida.
 */
void knx_phy_tpuart_tx_cplt(void);

/**
 * Callback de aviso de recepción de un dato de la UART conectada a la TPUART
 *
 * Este callback es llamado desde el callback general de recepción de datos 
 * de las diferentes UARTs del sistema, o directamente desde la ISR correspondiente, 
 * dependiendo de la implementación elegida.
 */
void knx_phy_tpuart_rx_cplt(void);

/**
 * Callback de aviso de timeout durante el reset de la TPUART
 *
 * Este callback es llamado desde el callback general de gestión de time-out 
 * de los diferentes TIMs del sistema funcionando en modo básico,
 * @code HAL_TIM_PeriodElapsedCallback. 
 */
void knx_phy_tpuart_reset_timeout(void);

  
/* ------------------------ PARTE 2: Primitivas  -------------------------- */

 
/* ----------------------- SECCIÓN 2.A: Ph_reset  ------------------------- */

/**
 * @brief Ph_reset.req() :: Inicializar TPUART
 *
 * Esta función da comienzo a la inicialización la TPUART y 
 * arranca un timer TIM en modo básico para gestionar el caso de time-out 
 * por falta de respuesta de la TPUART 
 *
 * @returns KNX_PHY_RESET_REQ_OK En caso de solicitud correcta (el estado actual del nivel de enlace es INIT)
 * @returns KNX_PHY_RESET_REQ_ERROR En caso de solicitud incorrecta (el estado actual del nivel de enlace no es INIT)
 */
uint32_t knx_phy_reset_req (void);

/**
 * @brief Ph_reset.con() :: Confirmación de la inicialización de la TPUART
 *
 * Cola descrita como knx_reset_con, el handle asignado por STCubeMX es knx_reset_conHandle
 */
extern osMessageQId knx_phy_reset_conHandle;


/* ----------------------- SECCIÓN 2.B: Ph_data  -------------------------- */


/**
 * @brief Ph_data.req() :: Enviar dato a TPUART
 * @param[in] p_req_class Clase de octeto a enviar (ver @ref knx_phy_data_req_class_t)
 * @param[in] p_data      Octeto a enviar
 *
 * @returns KNX_PHY_DATA_REQ_OK En caso de solicitud correcta (el estado actual del nivel de enlace es NORMAL)
 * @returns KNX_PHY_DATA_REQ_ERROR En caso de solicitud incorrecta (el estado actual del nivel de enlace no es NORMAL)
 */
uint32_t knx_phy_data_req (knx_phy_data_req_class_t p_req_class, uint8_t p_data);

/**
 * @brief Ph_data.con() :: Confirmación del envío de octeto a la TPUART
 *
 * Cola descrita como knx_phy_data_con, el handle asignado por STCubeMX es knx_phy_data_conHandle
 *
 * Cada elemento de esta cola es un uint16_t y empaqueta dos uint8_t:
 * - La parte alta es el knx_phy_data_con_status_t
 * - La parte baja es el dado recibido desde la TPUART
 *
 * Asumiendo las variables con_status (tipo @ref knx_phy_data_con_status_t) y p_data (tipo uint8_t), 
 * el empaquetamiento se realiza con
 * <tt> ((((uint16_t)con_status) << 8) & 0xFF00) | (((uint16_t)p_data) & 0x00FF)  </tt>
 */
extern osMessageQId knx_phy_data_conHandle;

/**
 * @brief Ph_data.ind() :: Señalización de recepción de octeto desde la TPUART
 *
 * Cola descrita como knx_phy_data_ind, el handle asignado por STCubeMX es knx_phy_data_indHandle
 *
 * Cada elemento de esta cola es un uint16_t y empaqueta dos uint8_t:
 * - La parte alta es el knx_phy_data_ind_class_t
 * - La parte baja es el dado recibido desde la TPUART
 *
 * Asumiendo las variables ind_class (tipo @ref knx_phy_data_ind_class_t) y p_data (tipo uint8_t), 
 * el empaquetamiento se realiza con
 * <tt> ((((uint16_t)ind_class) << 8) & 0xFF00) | (((uint16_t)p_data) & 0x00FF)  </tt>
 */
extern osMessageQId knx_phy_data_indHandle;


/* ----------------------- SECCIÓN 2.C: General  -------------------------- */


/**
 * @brief Inicializar el nivel físico
 *
 * @warning Esta función debe ser la primera utilizada del nivel físico por los niveles superiores
 *
 * @returns Nada
 */
void knx_phy_init (void);


/* @} */

#endif // __KNX_PHY_H
