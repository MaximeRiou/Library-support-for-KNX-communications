/**
 * @file knx_link.c
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

/* ---------------- #includes necesarios para este fichero ----------------- */

#include <stdint.h>     // Para los tipos uintXX_t
#include "knx_link.h"   // Para las declaraciones pÃºblicas de este mÃ³dulo

/* --------------------------- Macros privadas ---------------------------- */

#define KNX_LINK_MAX_GRP_ADDRESSES  100

#define KNX_LINK_MAX_LSDU    255

/* ----------------------- Tipos de datos privados ------------------------ */

/**
 * Tipo estructurado para la gestiÃ³n de direcciones de grupo
 */
struct knx_link_grp_addresses_s {
    uint32_t used;                                      /**< Entradas en uso      */
    uint16_t addresses[KNX_LINK_MAX_GRP_ADDRESSES];   /**< Entradas almacenadas */
};
/**
 * RedefiniciÃ³n con typedef para usar una Ãºnica palabra
 */
typedef struct knx_link_grp_addresses_s knx_link_grp_addresses_t;

/**
 * Tipo estructurado para la gestiÃ³n de la direcciÃ³n de polling
 */
struct knx_link_poll_address_s {
    uint16_t grp_address;           /**< DirecciÃ³n de grupo de polling */
    uint16_t slot_number;           /**< Slot number de polling        */
};
/**
 * RedefiniciÃ³n con typedef para usar una Ãºnica palabra
 */
typedef struct knx_link_poll_address_s knx_link_poll_address_t;




// written by me from there

struct knx_link_dest_address_s {
   uint16_t address;
   uint8_t address_type; /* flag 0/1, other values are illegal */
};

typedef struct knx_link_dest_address_s knx_link_dest_address_t;


struct knx_link_lsdu_s {
   uint8_t lsdu_3bits_used; /* flag 0/1 */
   uint8_t lsdu_3bits; /* actual lsdu bits if prev flag is 1 */
   uint8_t lsdu_length;
   uint8_t lsdu_bytes[KNX_LINK_MAX_LSDU];

   // lsdu_3bits_used only for standard frame
   // lsdu_bytes[] : don't need to check the length because lsdu_length type is uint8_t
   // lsdu_3bits is 0 or 1 : for standard frame it's 1 and otherwise 0 ? error if >= 2 ?

   // lsdu_3bits_used : 0 or 1 for standard frame
   // lsdu_3bits_used : 0 for extended frame

};

typedef struct knx_link_lsdu_s knx_link_lsdu_t;


struct knx_link_data_req_params_s {
   uint8_t confirmation_status; /* Used for confirmation primitive only  */
   uint16_t source_address; /* Used for indication primitive only */
   uint8_t repetitions;
   uint8_t priority;
   knx_link_dest_address_t dest_address;
   knx_link_lsdu_t lsdu;
};

typedef struct knx_link_data_req_params_s knx_link_data_req_params_t;

/* ------------------------- Variables privadas --------------------------- */

/**
 * DirecciÃ³n individual de este sistema
 *
 * El valor inicial es 0xFF, que segÃºn el estÃ¡ndar corresponde a un nodo no
 * incializado/configurado.
 * Este valor es modificado desde @ref knx_link_init() a travÃ©s de @ref
 * knx_link_init_ind_address()
 */
static uint16_t knx_link_ind_address = 0xFF;
/**
 * DirecciÃ³n de polling de este sistema
 */
static knx_link_poll_address_t knx_link_poll_address;
/**
 * Tabla de direcciones de grupo de este sistema
 */
static knx_link_grp_addresses_t knx_link_grp_addresses;
/**
 * Estado del nivel de enlace de KNX
 *
 * El valor inicial KNX_LINK_ILLEGAL_STATE permite determinar si se
 * ejecutado o no la inicializaciÃ³n del nivel de enlace con
 * @ref knx_link_init(), que modifica este valor a travÃ©s
 * de @ref knx_link_init_comm_state()
 */
static knx_link_comm_state_t knx_link_comm_state = KNX_LINK_ILLEGAL_STATE;

// written by me from here

static knx_link_dest_address_t knx_link_dest_address;

static knx_link_lsdu_t knx_link_lsdu;

static knx_link_data_req_params_t knx_link_data_req_params;


/* ----------------- DeclaraciÃ³n de funciones privadas -------------------- */

/**
 * @brief Inicializar direcciÃ³n individual
 * @param[in] ind_address DirecciÃ³n individual de este sistema
 *
 * Esta funciÃ³n sÃ³lo es llamada desde @ref knx_link_init durante
 * la inicializaciÃ³n del nivel de enlace
 *
 * @returns Nada
 */
static void knx_link_init_ind_address (uint16_t ind_address);

/**
 * @brief Inicializar direcciÃ³n de polling
 * @param[in] grp_address DirecciÃ³n de grupo de polling de este sistema
 * @param[in] slot_number Slot numbre de polling de este sistema
 *
 * Esta funciÃ³n sÃ³lo es llamada desde @ref knx_link_init durante
 * la inicializaciÃ³n del nivel de enlace
 *
 * @returns Nada
 */
static void knx_link_init_poll_address (uint16_t grp_address, uint16_t slot_number);

/**
 * @brief Inicializar direcciones de grupo
 *
 * Esta funciÃ³n sÃ³lo es llamada desde @ref knx_link_init durante
 * la inicializaciÃ³n del nivel de enlace
 *
 * @returns Nada
 */
static void knx_link_init_grp_addresses (void);

/**
 * @brief Inicializar estado del nivel de enlace
 *
 * Esta funciÃ³n sÃ³lo es llamada desde @ref knx_link_init durante
 * la inicializaciÃ³n del nivel de enlace
 *
 * @returns Nada
 */
static void knx_link_init_comm_state (void); 

/**
 * @brief Modificar el estado del nivel de enlace
 * @param[in] new_state Nuevo estado del nivel de enlace
 *
 * Esta funciÃ³n modifica el estado del nivel de enlace sin ningÃºn tipo
 * de comprobaciÃ³n de errores (ej: transiciÃ³n entre estados imposible)
 *
 * @returns Nada
 */
static void knx_link_set_comm_state (knx_link_comm_state_t new_state); 


// written by me from here
// auxiliary functions
// can embed all validity checks around these elements (da, at, lsdu)


int knx_link_check_da (knx_link_dest_address_t *ptr_da);

int knx_link_check_lsdu_std (knx_link_lsdu_t *ptr_lsdu); // Check lsdu fields for validity on a standard frame

int knx_link_check_lsdu_ext (knx_link_lsdu_t *ptr_lsdu); // Check lsdu fields for validity on a extended frame


int knx_link_data_req_std (knx_link_data_req_params_t *ptr_req);

int knx_link_data_req_ext (knx_link_data_req_params_t *ptr_req);




/* ---------------- ImplementaciÃ³n de funciones privadas ------------------ */

static void knx_link_init_ind_address (uint16_t ind_address)
{
	knx_link_ind_address = ind_address;
}

static void knx_link_init_poll_address (uint16_t grp_address, uint16_t slot_number)
{
	knx_link_poll_address.grp_address = grp_address;
	knx_link_poll_address.slot_number = slot_number;
}

void knx_link_init_grp_addresses (void)
{
	knx_link_grp_addresses.used = 0;
	// knx_link_grp_addresses.addresses[0];
}

static void knx_link_init_comm_state (void)
{
	knx_link_comm_state = KNX_LINK_INIT_STATE;
}

static void knx_link_set_comm_state (knx_link_comm_state_t new_state)
{
	knx_link_comm_state = new_state;
}


// written by me from here

int knx_link_check_da (knx_link_dest_address_t *ptr_da)
{
	if(ptr_da -> address_type >= 2)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


int knx_link_check_lsdu_std (knx_link_lsdu_t *ptr_lsdu)		// Check lsdu fields for validity on a standard frame
{
	if(ptr_lsdu -> lsdu_3bits_used >= 2)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


int knx_link_check_lsdu_ext (knx_link_lsdu_t *ptr_lsdu) 	// Check lsdu fields for validity on a extended frame
{
	if(ptr_lsdu -> lsdu_3bits_used != 0)	// error because lsdu_3bits_used is not used for extended frame
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
int knx_link_data_req_std (knx_link_data_req_params_t *ptr_req)
{
	// The Control field shall indicate the type of the request Frame: L_Data_Standard Frame L_Data_Extended Frame
	// Bytes 2 and 3 : for the priority (standard, urgent, normal, low)

	if(ptr_req -> lsdu.lsdu_3bits_used >= 2)   // can be 0 or 1 for standard frame
	{
		return 0;	// error because lsdu_3bits_used must be 0 or 1 for standard frame
	}
	else
	{
		return 1;
	}

	// For the control field : if bit7 (frame type) == 1 --> standard frame

	// if all is ok, send confirmation status
}

int knx_link_data_req_ext (knx_link_data_req_params_t *ptr_req)
{
	// confirmation_status, source_address, repetitions, priority, dest_address, lsdu

	// framing error, parity error, bit error

	if(ptr_req -> lsdu.lsdu_3bits_used != 0)
	{
		return 0;	// error because lsdu_3bits_used must not be used for standard frame
	}
	else
	{
		return 1;
	}

}
*/


/* ---------------- ImplementaciÃ³n de funciones pÃºblicas ------------------ */




uint16_t knx_link_get_ind_address (void)
{
	return knx_link_ind_address;
}



uint16_t knx_link_get_poll_grp_address (void)
{
	return knx_link_poll_address.grp_address;
}

uint16_t knx_link_get_poll_slot_number (void)
{
	return knx_link_poll_address.slot_number;
}



uint32_t knx_link_add_grp_address (uint16_t grp_address)
{

	// check if the max number of addresses is not reached 'KNX_LINK_MAX_GRP_ADDRESSES  100'
	if(sizeof(knx_link_grp_addresses.used)<100)
	{
		knx_link_grp_addresses.addresses[(knx_link_grp_addresses.used)+1] = grp_address;	// add the group address
		knx_link_grp_addresses.used++;
		return 1;	// it's ok
	}
	else  // no space to stock a new group address
	{
		return 0;
	}
}

uint32_t knx_link_exists_grp_address (uint16_t grp_address)
{
	// make a loop to check each address

	for(int i = 0; i < (knx_link_grp_addresses.used)+1; i++){

		if(knx_link_grp_addresses.addresses == grp_address)  // if the address is stock
		{
			return 1;
		}

		else	// if the address is not stock
		{
			return 0;
		}
	}
}



knx_link_comm_state_t knx_link_get_comm_state (void)
{
	return knx_link_comm_state;
}



void knx_link_init (uint16_t ind_address, uint16_t poll_grp_address, uint16_t poll_slot_number)
{
  knx_link_init_ind_address(ind_address);
  
  knx_link_init_poll_address(poll_grp_address, poll_slot_number);

  knx_link_init_grp_addresses();

  knx_link_init_comm_state();
}



/* @} */

