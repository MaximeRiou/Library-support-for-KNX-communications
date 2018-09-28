/**
 * @file knx_phy_support.h
 * @author PON TU NOMBRE AQUÍ
 * @date Otoño 2017
 *
 * @brief Declaraciones de macros, desplazamientos y valores para TPUART de KNX y análisis/formateo de tramas KNX
 *
 * @{
 */
#ifndef __KNX_PHY_SUPPORT_H
#define __KNX_PHY_SUPPORT_H




/* 
 * Constantes para el intercambio de información con la TP-UART (órdenes)
 */
#define KNX_TPUART_COMMAND_U_RESET_REQUEST       0x01   /**< Orden de reset a enviar a la TP-UART                       */
#define KNX_TPUART_COMMAND_U_ACKINFO__ADDRESSED  0x11   /**< Orden a la TP-UART para confirmación de ACK (addressed)    */
#define KNX_TPUART_COMMAND_U_ACKINFO__BUSY       0x12   /**< Orden a la TP-UART para confirmación de ACK (ocupado)      */
#define KNX_TPUART_COMMAND_U_ACKINFO__NACK       0x14   /**< Orden a la TP-UART para confirmación de ACK (negative ack) */
#define KNX_TPUART_COMMAND_U_L_DATA_START        0x80   /**< Prefijo de control previo al primer dato de una trama      */
#define KNX_TPUART_COMMAND_U_L_DATA_CONTINUE     0x80   /**< Prefijo de control previo a un octeto intermedio de una trama 
                                                             (es necesario sumar a este valor el índice del dato intermedio, 
                                                             comenzando con el valor 1) */
#define KNX_TPUART_COMMAND_U_L_DATA_END          0x40   /**< Prefijo de control previo al último octeto de una trama 
                                                             (es necesario sumar a este valor la longitud de la trama) */

/* 
 * Constantes para el intercambio de información con la TP-UART (respuestas / señalizaciones)
 */
#define KNX_TPUART_U_RESET_INDICATION            0x03   /**< Octeto de respuesta de la TP-UART a la solicitud de reset        */
#define KNX_TPUART_U_STATE_INDICATION            0x07   /**< Octeto de respuesta / señalización estado de la TP-UART.
                                                             Los 5 bits de mayor peso indican el estado; por orden de MAS a MENOS peso:
                                                             SC - Slave collision, RE - Receive error, TE - Transmitter error, 
                                                             PE - Protocol error, TW - Temperature Warning */
#define KNX_TPUART_L_DATA_CONFIRMATION_POS       0x8B   /**< Señalización L_Data.confirmation (positiva, trama reconocida)    */
#define KNX_TPUART_L_DATA_CONFIRMATION_NEG       0x0B   /**< Señalización L_Data.confirmation (negativa, trama no reconocida) */

/*
 * Máscaras para el campo CTRL de una trama de datos (estándar o extendida)
 */

/*
 * Campo FT (Frame Type)
 */
#define KNX_DATA_FRAME_CTRL_FT_MASK      0x80  /**< Máscara del campo FT (Frame Type)    */
#define KNX_DATA_FRAME_CTRL_FT_SHIFT        7  /**< Desplazamiento del campo FT          */
#define KNX_DATA_FRAME_CTRL_FT__0        0x00  /**< Valor de FT = 0 (trama estándar)     */
#define KNX_DATA_FRAME_CTRL_FT__1        0x80  /**< Valor de FT = 1 (trama extendida)    */
#define KNX_DATA_FRAME_CTRL_FT__STANDARD 0x00  /**< Valor de FT para trama estándar (0)  */
#define KNX_DATA_FRAME_CTRL_FT__EXTENDED 0x80  /**< Valor de FT para trama extendida (1) */

/*
 * Campo REP (Repeated frame)
 */
#define KNX_DATA_FRAME_CTRL_REP_MASK         0x20  /**< Máscara del campo R (Repeated frame)      */
#define KNX_DATA_FRAME_CTRL_REP_SHIFT           5  /**< Desplazamiento del campo R                */
#define KNX_DATA_FRAME_CTRL_REP__0           0x00  /**< Valor de REP = 0 (Repeated frame)         */
#define KNX_DATA_FRAME_CTRL_REP__1           0x20  /**< Valor de REP = 1 (Non repeated frame)     */
#define KNX_DATA_FRAME_CTRL_REP__REPEATED    0x00  /**< Valor de REP para tramas repetidas (0)    */
#define KNX_DATA_FRAME_CTRL_REP__NONREPEATED 0x20  /**< Valor de REP para tramas no repetidas (1) */

/*
 * Campo PRIO (Priority)
 */
#define KNX_DATA_FRAME_CTRL_PRIO_MASK    0x0C  /**< Máscara del campo PRIO (Priority)                 */
#define KNX_DATA_FRAME_CTRL_PRIO_SHIFT      2  /**< Desplazamiento del campo PRIO                     */
#define KNX_DATA_FRAME_CTRL_PRIO__0      0x00  /**< Valor de PRIO = 0 (prioridad SYSTEM)              */
#define KNX_DATA_FRAME_CTRL_PRIO__1      0x04  /**< Valor de PRIO = 1 (prioridad URGENT)              */
#define KNX_DATA_FRAME_CTRL_PRIO__2      0x08  /**< Valor de PRIO = 2 (prioridad NORMAL)              */
#define KNX_DATA_FRAME_CTRL_PRIO__3      0x0C  /**< Valor de PRIO = 3 (prioridad LOW)                 */
#define KNX_DATA_FRAME_CTRL_PRIO__SYSTEM 0x00  /**< Valor de PRIO para tramas de prioridad SYSTEM (0) */
#define KNX_DATA_FRAME_CTRL_PRIO__URGENT 0x04  /**< Valor de PRIO para tramas de prioridad URGENT (1) */
#define KNX_DATA_FRAME_CTRL_PRIO__NORMAL 0x08  /**< Valor de PRIO para tramas de prioridad NORMAL (2) */
#define KNX_DATA_FRAME_CTRL_PRIO__LOW    0x0C  /**< Valor de PRIO para tramas de prioridad LOW (3)    */

/* 
 * Bits fijos (máscara y valor) en CTRL para tramas de datos
 */
#define KNX_DATA_FRAME_CTRL_FIXED_MASK   0x53  /**< Máscara de bits fijos en CTRL para tramas de datos */
#define KNX_DATA_FRAME_CTRL_FIXED_VALUE  0x10  /**< Valor de bits fijos en CTRL para tramas de datos   */

/* 
 * Bits fijos (máscara y valor) en CTRL para tramas de polling
 */
#define KNX_POLL_FRAME_CTRL_FIXED_MASK   0xFF  /**< Máscara de bits fijos en CTRL para tramas de polling */
#define KNX_POLL_FRAME_CTRL_FIXED_VALUE  0xF0  /**< Valor de bits fijos en CTRL para tramas de polling   */

/* 
 * Bits fijos (máscara y valor) en CTRL para tramas de reconocimiento
 */
#define KNX_ACK_FRAME_CTRL_FIXED_MASK    0x33  /**< Máscara de bits fijos en CTRL para tramas de reconocimiento */
#define KNX_ACK_FRAME_CTRL_FIXED_VALUE   0x00  /**< Valor de bits fijos en CTRL para tramas de reconocimiento   */




/*
 * Máscaras para el campo AT/LSDU/LG de una trama estándar
 */

/*
 * Campo AT (Address Type)
 */
#define KNX_STD_FRAME_ATLSDULG_AT_MASK      0x80  /**< Máscara del campo AT (Address Type) en AT/LSDU/LG (trama estándar)                   */
#define KNX_STD_FRAME_ATLSDULG_AT_SHIFT        7  /**< Desplazamiento del campo AT en AT/LSDU/LG (trama estándar)                           */
#define KNX_STD_FRAME_ATLSDULG_AT_SHIFT__0  0x00  /**< Valor AT = 0 en AT/LSDU/LG (trama estándar, DA es dirección individual)              */
#define KNX_STD_FRAME_ATLSDULG_AT_SHIFT__1  0x80  /**< Valor AT = 1 en AT/LSDU/LG (trama estándar, DA es dirección grupo)                   */
#define KNX_STD_FRAME_ATLSDULG_AT_SHIFT__DA_INDIVIDUAL 0x00  /**< Valor AT en AT/LSDU/LG para DA como dirección individual (trama estándar) */
#define KNX_STD_FRAME_ATLSDULG_AT_SHIFT__DA_GROUP      0x80  /**< Valor AT en AT/LSDU/LG para DA como dirección grupo (trama estándar)      */

/*
 * Campo LSDU (Link Servide Data Unit)
 */
#define KNX_STD_FRAME_ATLSDULG_LSDU_MASK      0x70  /**< Máscara del campo LSDU (Link Servide Data Unit) en AT/LSDU/LG (trama estándar)     */
#define KNX_STD_FRAME_ATLSDULG_LSDU_SHIFT        4  /**< Desplazamiento del campo LSDU en AT/LSDU/LG (trama estándar)                       */

/*
 * Campo LG (Frame Length)
 */
#define KNX_STD_FRAME_ATLSDULG_LG_MASK      0x0F  /**< Máscara del campo LG (Frame Length) en AT/LSDU/LG (trama estándar)     */
#define KNX_STD_FRAME_ATLSDULG_LG_SHIFT        0  /**< Desplazamiento del campo LG en AT/LSDU/LG (trama estándar)             */





/*
 * Máscaras para el campo CTRLE de una trama extendida
 */

/*
 * Campo AT (Address Type)
 */
#define KNX_EXT_FRAME_CTRLE_AT_MASK      0x80  /**< Máscara del campo AT (Address Type) en CTRLE (trama extendida)                   */
#define KNX_EXT_FRAME_CTRLE_AT_SHIFT        7  /**< Desplazamiento del campo AT en CTRLE (trama extendida)                           */
#define KNX_EXT_FRAME_CTRLE_AT_SHIFT__0  0x00  /**< Valor AT = 0 en CTRLE (trama extendida, DA es dirección individual)              */
#define KNX_EXT_FRAME_CTRLE_AT_SHIFT__1  0x80  /**< Valor AT = 1 en CTRLE (trama extendida, DA es dirección grupo)                   */
#define KNX_EXT_FRAME_CTRLE_AT_SHIFT__DA_INDIVIDUAL 0x00  /**< Valor AT en CTRLE para DA como dirección individual (trama extendida) */
#define KNX_EXT_FRAME_CTRLE_AT_SHIFT__DA_GROUP      0x80  /**< Valor AT en CTRLE para DA como dirección grupo (trama extendida)      */

/*
 * Campo HOP (Hop Count)
 */
#define KNX_EXT_FRAME_CTRLE_HOP_MASK     0x70  /**< Máscara del campo HOP (Hop Count) en CTRLE (trama extendida) */
#define KNX_EXT_FRAME_CTRLE_HOP_SHIFT       4  /**< Desplazamiento del campo HOP en CTRLE (trama extendida)      */

/*
 * Campo EXT FRAME FMT (Extended Frame Format)
 */
#define KNX_EXT_FRAME_CTRLE_HOP_MASK     0x0F  /**< Máscara del campo EXT FRAME FMT (Extended Frame Format) en CTRLE (trama extendida) */
#define KNX_EXT_FRAME_CTRLE_HOP_SHIFT       0  /**< Desplazamiento del campo EXT FRAME FMT en CTRLE (trama extendida)                  */


/* @} */
#endif // __KNX_PHY_SUPPORT_H
