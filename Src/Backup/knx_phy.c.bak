/**
 * @file knx_phy.c
 * @author PON TU NOMBRE AQUÃ�
 * @date OtoÃ±o 2017
 *
 * @brief ImplementaciÃ³n completa del nivel fÃ­sico KNX
 *
 *
 * Servicios de nivel fÃ­sico:
 * - InicializaciÃ³n (reset): Primitivas
 *   1 - Request
 *       FunciÃ³n pÃºblica knx_phy_reset_req
 *   2 - Confirmation
 *       Cola FreeRTOS pÃºblica knx_phy_reset_con
 *   
 * Utilizaremos la capa HAL para la transmisiÃ³n / recepciÃ³n a la UART,
 * y un timer TIM en modo bÃ¡sico para el time-out del reset de la TP-UART
 *
 * @{
 */

/* ---------------- #includes necesarios para este fichero ----------------- */

#include <stdint.h>     // Para los tipos uintXX_t
#include "knx_link.h"   // Para el acceso a los parÃ¡metros del nivel de enlace
#include "knx_phy.h"    // Para  las declaraciones pÃºblicas de este mÃ³dulo
#include "stm32f4xx_hal.h" // Para declaraciones de la capa HAL

/* --------------------------- Macros privadas ---------------------------- */

/* Valores asociados a knx_phy_data_ft */
#define KNX_PHY_DATA_FT_ESTANDAR             0
#define KNX_PHY_DATA_FT_EXTENDIDA            1
/* Valores asociados a knx_phy_data_at */
#define KNX_PHY_DATA_AT_INDIVIDUAL           0
#define KNX_PHY_DATA_AT_GRUPO                1

/* ----------------------- Tipos de datos privados ------------------------ */

/**
 * Tipo enumerado con los posibles estados de la mÃ¡quina de estados KNX fÃ­sico
 */
enum knx_phy_fsm_state_e { 
    KNX_PHY_FSM_E_CTRL,       /**< Procesar campo CTRL (cualquier trama)    */
    KNX_PHY_FSM_E_SA1,        /**< Procesar parte alta SA (trama estÃ¡ndar)  */
    KNX_PHY_FSM_E_SA2,        /**< Procesar parte baja SA (trama estÃ¡ndar)  */
    KNX_PHY_FSM_E_DA1,        /**< Procesar parte alta DA (trama estÃ¡ndar)  */
    KNX_PHY_FSM_E_DA2,        /**< Procesar parte baja DA (trama estÃ¡ndar)  */
    KNX_PHY_FSM_E_ATLSDULG,   /**< Procesar AT/LSDU/LG (trama estÃ¡ndar)     */
    KNX_PHY_FSM_E_OTRO,       /**< Procesar otro octeto (cualquier trama)   */
    KNX_PHY_FSM_EX_CTRL,      /**< Procesar campo CTRLE (trama extendida)   */
    KNX_PHY_FSM_EX_SA1,       /**< Procesar parte alta SA (trama extendida) */
    KNX_PHY_FSM_EX_SA2,       /**< Procesar parte baja SA (trama extendida) */
    KNX_PHY_FSM_EX_DA1,       /**< Procesar parte alta DA (trama extendida) */
    KNX_PHY_FSM_EX_DA2,       /**< Procesar parte baja DA (trama extendida) */
};
/**
 * RedefiniciÃ³n con typedef para usar una Ãºnica palabra
 */
typedef enum knx_phy_fsm_state_e knx_phy_fsm_state_t;


/* ------------------------- Variables privadas --------------------------- */

/**
 * Tiempo absoluto de inicio del reset de la TPUART (en ticks de la capa HAL)
 */
static uint32_t knx_phy_reset_start_tick;

/**
 * Estado de la FSM que analiza las tramas entrantes
 */
static knx_phy_fsm_state_t knx_phy_fsm_state;

/**
 * GestiÃ³n de valores del anÃ¡lisis de las tramas entrantes
 */
static uint8_t knx_phy_data_ft;  /**< Frame type (KNX_PHY_DATA_FT_ESTANDAR / KNX_PHY_DATA_FT_EXTENDIDA) */
static uint8_t knx_phy_data_at;  /**< Address type (KNX_PHY_DATA_AT_INDIVIDUAL / KNX_PHY_DATA_AT_GRUPO) */
static uint16_t knx_phy_data_sa; /**< Source address      */
static uint16_t knx_phy_data_da; /**< Destination address */


/* ----------------- DeclaraciÃ³n de funciones privadas -------------------- */


/* ---------------- ImplementaciÃ³n de funciones privadas ------------------ */


/* ---------------- ImplementaciÃ³n de funciones pÃºblicas ------------------ */


/* ----------------- PARTE 1: Callbacks de la capa HAL  -------------------- */

void knx_phy_tpuart_tx_cplt(void)
{
}

void knx_phy_tpuart_rx_cplt(void)
{
}

void knx_phy_tpuart_reset_timeout(void)
{
}


  
/* ------------------------ PARTE 2: Primitivas  -------------------------- */

 
/* ----------------------- SECCIÃ“N 2.A: Ph_reset  ------------------------- */

uint32_t knx_phy_reset_req (void)
{
}



/* ----------------------- SECCIÃ“N 2.B: Ph_data  -------------------------- */


uint32_t knx_phy_data_req (knx_phy_data_req_class_t p_req_class, uint8_t p_data)
{
}



/* ----------------------- SECCIÃ“N 2.C: General  -------------------------- */


void knx_phy_init (void)
{
}



/* @} */

