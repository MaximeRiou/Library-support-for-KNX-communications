//*****************************************************************************
// 
// Fichero: debug_repo.c
// Proposito:
//   Repositorio de mensajes de depuración con envío a una consola mediante USART
// 
//*****************************************************************************

#include <string.h>
#include <LowLevelIOInterface.h>
#include "helpers.h"
#include "debug_repo.h"

    // *NO* modificar estas macros (MUTEX_WAIT, MUTEX_RELEASE)
#ifdef DEBUGREPO_USE_MUTEX
  #define MUTEX_WAIT     if (osKernelRunning()) {osMutexWait(_repo.mutex, osWaitForever); }
  #define MUTEX_RELEASE  if (osKernelRunning()) {osMutexRelease(_repo.mutex); }
#else
  #define MUTEX_WAIT      
  #define MUTEX_RELEASE   
#endif  // DEBUGREPO_USE_MUTEX


    // *NO* modificar esta macro (DELAY)
#ifdef DEBUGREPO_USE_DELAY
    #define DELAY   _short_delay()
#else
    #define DELAY   
#endif // DEBUGREPO_USE_DELAY


#define DEBUGREPO_HANDLERS_NUMLEVELS         ((1<<__NVIC_PRIO_BITS) + 2)

struct s_debugrepo {
	/* Indices de lectura/escritura         */
	uint32_t head, tail;
	/* Estadísticas */
	t_debugrepo_stats stats;
	/* Buffer de almacenamiento             */
	uint8_t  buffer[DEBUGREPO_SIZE];
    /* Semáforo de sincronización con USART (HAL driver) */
	osSemaphoreId sem;
    /* Mutex de acceso en exclusión mutua   */
    osMutexId mutex;
	/* Task handle de la tarea que extrae los mensajes y los envía a través de la USART */
	osThreadId task;
};
typedef struct s_debugrepo t_debugrepo;

struct s_debugrepo_handler {
	/* Indices de lectura/escritura         */
	uint32_t head, tail;
	/* Estadísticas */
	t_debugrepo_stats stats;
	/* Buffer de almacenamiento             */
	uint8_t  buffer[DEBUGREPO_HANDLERS_SIZE];
};
typedef struct s_debugrepo_handler t_debugrepo_handler;

//*****************************************************************************
// Parte privada
typedef struct {
  int valid, idx;
  osThreadId thid;
  char buffer[DEBUGREPO_MSG_MAXLEN];
} t_write_threaded;
#define DEBUGREPO_WRITE_THREADED_HANDLERS
#define DEBUGREPO_PRINTF_SUPPORTED_MAX_TASKS    (5)
#ifdef DEBUGREPO_WRITE_THREADED_HANDLERS
#define DEBUGREPO_WRITE_HANDLERS_NUMLEVELS       DEBUGREPO_HANDLERS_NUMLEVELS
#else
#define DEBUGREPO_WRITE_HANDLERS_NUMLEVELS       (0)
#endif
#define DEBUGREPO_WRITE_THREADED_NUMLEVELS       (DEBUGREPO_WRITE_HANDLERS_NUMLEVELS+DEBUGREPO_PRINTF_SUPPORTED_MAX_TASKS+1)
static t_write_threaded _writeth[DEBUGREPO_WRITE_THREADED_NUMLEVELS];
static void _write_threaded_init(t_write_threaded *wth);
static void _write_threaded_init_all(void);
static t_write_threaded *_write_threaded_get_index(unsigned int handler_mode, unsigned int prio_level, osThreadId task);
static void _write_threaded_write(t_write_threaded *wth, const unsigned char * buffer, size_t size);

static t_debugrepo _repo;
static t_debugrepo_handler _repo_handlers[DEBUGREPO_HANDLERS_NUMLEVELS];
static char _msg2send[DEBUGREPO_MSG_MAXLEN];

#ifdef DEBUGREPO_USE_DELAY
static void _short_delay(void);
#endif // DEBUGREPO_USE_DELAY
static void _debugrepoWaitUntilUARTFlag(uint32_t flag);
static int _inHandlerMode (void);
static int _isHandlerFinal (void);
static int _getHandlerPrioLevel (void);
static void _debugrepoGetStats (t_debugrepo_stats *stats);
static void _debugrepoGetHandlerStats (uint32_t prio_level, t_debugrepo_stats *stats);
static void _debugrepoInitStats (t_debugrepo_stats *stats);
static int _debugrepoInsertChar_nomutex (uint8_t dato);
static int _debugrepoExtractChar_nomutex (uint8_t *dato);
static int _debugrepoHandlerInsertChar (uint32_t prio_level, uint8_t dato);
static int _debugrepoHandlerExtractChar (uint32_t prio_level, uint8_t *dato);
static int _debugrepoInsertHandlerMsgBlocking(const char *msg, uint16_t len);
static int _debugrepoInsertHandlerMsgNonBlocking(uint32_t prio_level, const char *msg, uint16_t len);
static int _debugrepoInsertMsgLen (const char *msg, uint16_t len);
static int _debugrepoHandlerExtractMsg (uint32_t prio_level, char msg[], int maxlen);
static void _debugrepoTask(void const * argument);
//*****************************************************************************



//*****************************************************************************
//
// Pequeño retardo software
//
//*****************************************************************************
#ifdef DEBUGREPO_USE_DELAY
static void _short_delay(void)
{
    // La variable debe declararse como volatile para que el compilador NO optimice
    // los accesos a la misma.
    // Si el acceso a la variable se optimiza el código equivalente del bucle for sería:
    // i = 1000;
    // ya que en el interior del bucle no se hace nada
    volatile uint32_t i;
    for (i=0; i<1000; i++) {
    }
}
#endif // DEBUGREPO_USE_DELAY


static void _debugrepoWaitUntilUARTFlag(uint32_t flag) {
  while (!__HAL_UART_GET_FLAG(&DEBUGREPO_UART_HANDLE, flag)) {
  }
}

/* Determine whether we are in thread mode or handler mode. */
static int _inHandlerMode (void) {
  return (__get_IPSR() & 0x00FF) != 0;
}

static int _isHandlerFinal (void) {
  uint32_t handler_mask;
  uint32_t ipsr = __get_IPSR() & 0x00FF;
  if ((ipsr == 0) || (ipsr >= 16)) {
    /* Thread mode or IRQn are *never* considered final */
    return 0;
  }
  handler_mask = 1 << ipsr;
  return ((DEBUGREPO_HANDLERS_FINAL_MASK & handler_mask) != 0);
}

static int _getHandlerPrioLevel (void) {
  uint32_t ipsr = __get_IPSR() & 0x00FF;
  int result = DEBUGREPO_HANDLERS_NUMLEVELS;
  switch (ipsr) {
  case 0:  /* Thread mode */
  case 1:  /* Reserved */
  case 7:  /* Reserved */
  case 8:  /* Reserved */
  case 9:  /* Reserved */
  case 10: /* Reserved */
  case 12: /* Reserved for Debug */
  case 13: /* Reserved */
    return result;
  }
  switch (ipsr) {
  case 2:  /* NMI */
    result = -2;
    break;
  case 3:  /* HardFault */
    result = -1;
    break;
  case 4:  /* MemManage */
    result = NVIC_GetPriority(MemoryManagement_IRQn);
    break;    
  case 5:  /* BusFault */
    result = NVIC_GetPriority(BusFault_IRQn);
    break;    
  case 6:  /* UsageFault */
    result = NVIC_GetPriority(UsageFault_IRQn);
    break;    
  case 11: /* SVCall */
    result = NVIC_GetPriority(SVCall_IRQn);
    break;    
  case 14: /* PendSV */
    result = NVIC_GetPriority(PendSV_IRQn);
    break;    
  case 15: /* SysTick */
    result = NVIC_GetPriority(SysTick_IRQn);
    break;    
  default: /* IRQn, n = ipsr - 16 */
    result = NVIC_GetPriority((IRQn_Type)(ipsr-16));
    break;    
  }
  return result+2;
}

//*****************************************************************************
//*****************************************************************************
// Funciones de inicialización
//*****************************************************************************
//*****************************************************************************

static void _debugrepoInitStats (t_debugrepo_stats *stats) {
	stats->items = stats->total_items = stats->insert_errors = 0;
}

void debugrepoInit (void)
{
  int i;
  
  _write_threaded_init_all();
  
  /* Initialize handlers repositories */
  for (i = 0; i < DEBUGREPO_HANDLERS_NUMLEVELS; i++) {
    _repo_handlers[i].head = _repo_handlers[i].tail = 0;
    _debugrepoInitStats(&_repo_handlers[i].stats);
  }
  
  /* Initialize thread mode / tasks repository */
  _repo.head = _repo.tail = 0;
  _debugrepoInitStats(&_repo.stats);
  osSemaphoreDef(_debugreposem);
  _repo.sem = osSemaphoreCreate(osSemaphore(_debugreposem), 1);
  osMutexDef(_debugrepomutex);
  _repo.mutex = osMutexCreate(osMutex(_debugrepomutex));
  osThreadDef(_debugrepotask, _debugrepoTask, osPriorityNormal, 0, 128);
  _repo.task = osThreadCreate(osThread(_debugrepotask), NULL);
}




//*****************************************************************************
//
// Insertar un caracter sin esperar a tener el mutex (asume que ya lo tenemos)
//
// En caso de que la tail esté llena no se entail el dato y se incrementan
// las estadísticas de errores de inserción.
// En caso de éxito se incrementan las estadísticas de total de datos insertados.
//
// Retorna 0 si error (tail llena), 1 si la operación termina con éxito
//*****************************************************************************
static int _debugrepoInsertChar_nomutex (uint8_t dato)
{
    if (DEBUGREPO_SIZE <= _repo.stats.items) {
        _repo.stats.insert_errors++;
        return 0;
    }
    // Alargar el tiempo necesario para la operación forzará la aparición de conflictos de acceso
    DELAY;
    _repo.stats.total_items++;
    DELAY;
    _repo.stats.items++;
    DELAY;
    _repo.buffer[_repo.head] = dato;
    DELAY;
    _repo.head = (_repo.head + 1) % DEBUGREPO_SIZE;
    DELAY;
    return 1;
}

static int _debugrepoHandlerInsertChar (uint32_t prio_level, uint8_t dato)
{
    if (DEBUGREPO_HANDLERS_SIZE <= _repo_handlers[prio_level].stats.items) {
        _repo_handlers[prio_level].stats.insert_errors++;
        return 0;
    }
    _repo_handlers[prio_level].stats.total_items++;
    _repo_handlers[prio_level].stats.items++;
    _repo_handlers[prio_level].buffer[_repo_handlers[prio_level].head] = dato;
    _repo_handlers[prio_level].head = (_repo_handlers[prio_level].head + 1) % DEBUGREPO_HANDLERS_SIZE;
    return 1;
}

//*****************************************************************************
//
// Extraer un caracter sin esperar a tener el mutex (asume que ya lo tenemos)
//
// En caso de que la tail esté vacía no se desentail nada ni se modifica
// el parámetro dato.
//
// Retorna 0 si error (tail vacía), 1 si la operación termina con éxito
//*****************************************************************************
static int _debugrepoExtractChar_nomutex (uint8_t *dato)
{
    if (0 == _repo.stats.items) {
        return 0;
    }
    // Alargar el tiempo necesario para la operación forzará la aparición de conflictos de acceso
    DELAY;
    _repo.stats.items--;
    DELAY;
    *dato = _repo.buffer[_repo.tail];
    DELAY;
    _repo.tail = (_repo.tail + 1) % DEBUGREPO_SIZE;
    DELAY;
    return 1;
}

static int _debugrepoHandlerExtractChar (uint32_t prio_level, uint8_t *dato) {
    if (0 == _repo_handlers[prio_level].stats.items) {
        return 0;
    }
    _repo_handlers[prio_level].stats.items--;
    *dato = _repo_handlers[prio_level].buffer[_repo_handlers[prio_level].tail];
    _repo_handlers[prio_level].tail = (_repo_handlers[prio_level].tail + 1) % DEBUGREPO_HANDLERS_SIZE;
    return 1;
}


static void _debugrepoGetStats (t_debugrepo_stats *stats)
{
    MUTEX_WAIT;
    stats->items = _repo.stats.items;
    stats->total_items = _repo.stats.total_items;
    stats->insert_errors = _repo.stats.insert_errors;
    MUTEX_RELEASE;
}

static void _debugrepoGetHandlerStats (uint32_t prio_level, t_debugrepo_stats *stats)
{
    stats->items = _repo_handlers[prio_level].stats.items;
    stats->total_items = _repo_handlers[prio_level].stats.total_items;
    stats->insert_errors = _repo_handlers[prio_level].stats.insert_errors;
}


//*****************************************************************************
//*****************************************************************************
// Funciones de inserción/extracción de mensajes completos
//*****************************************************************************
//*****************************************************************************

static void _write_threaded_init(t_write_threaded *wth) {
  wth->valid = wth->idx = 0;
  wth->thid = 0;
}

static void _write_threaded_init_all(void) {
  int i;
  for (i=0; i < DEBUGREPO_WRITE_THREADED_NUMLEVELS; i++) {
    _write_threaded_init(&_writeth[i]);
  }
}

static void _write_threaded_write(t_write_threaded *wth, const unsigned char * buffer, size_t size) {
  unsigned int buffer_idx = 0;
  while ((size + wth->idx) >= (DEBUGREPO_MSG_MAXLEN-2)) {
    copyBinString((const char *)&buffer[buffer_idx], DEBUGREPO_MSG_MAXLEN-wth->idx - 2, &(wth->buffer[wth->idx]), DEBUGREPO_MSG_MAXLEN-wth->idx - 2);
    size -= DEBUGREPO_MSG_MAXLEN-wth->idx - 2;
    buffer_idx += DEBUGREPO_MSG_MAXLEN-wth->idx - 2;
    wth->idx = 0;
    wth->buffer[DEBUGREPO_MSG_MAXLEN-2] = '\r';
    wth->buffer[DEBUGREPO_MSG_MAXLEN-1] = '\n';
    debugrepoInsertMsgLen (wth->buffer, DEBUGREPO_MSG_MAXLEN);
  }
  if (size > 0) {
    copyBinString((const char *)&buffer[buffer_idx], size, &wth->buffer[wth->idx], DEBUGREPO_MSG_MAXLEN-wth->idx-2);
    wth->idx += size;
    if (buffer[size-1]== '\n') {
      debugrepoInsertMsgLen (wth->buffer, wth->idx);
      wth->idx = 0;
    }
  }
}

static t_write_threaded *_write_threaded_get_index(unsigned int handler_mode, unsigned int prio_level, osThreadId task) {
  unsigned int i;
  unsigned int idx;
  unsigned int found = 0;
  
  if (handler_mode) {
    idx = prio_level;
  }
  for (i=DEBUGREPO_WRITE_HANDLERS_NUMLEVELS; i < DEBUGREPO_WRITE_THREADED_NUMLEVELS-1; i++) {
    if ((_writeth[i].valid) && (_writeth[i].thid == task)) {
      idx = i;
      found = 1;
      break;
    }
  }
  if (!found) {
    for (i=DEBUGREPO_WRITE_HANDLERS_NUMLEVELS; i < DEBUGREPO_WRITE_THREADED_NUMLEVELS-1; i++) {
      if (!_writeth[i].valid) {
        _writeth[i].valid = 1;
        _writeth[i].thid = task;
        idx = i;
        found = 1;
        break;
      }
    }
  }
  if (!found) {
    idx = DEBUGREPO_WRITE_THREADED_NUMLEVELS-1;
  }
  return &_writeth[idx];
}

/* Reimplement __write in order to redirect printf output */
// size_t __write(int handle, const unsigned char * buffer, size_t size)
// redirected here with linker option --redirect __write=__write_redirected
size_t __write_redirected(int handle, const unsigned char * buffer, size_t size)
{
  if (buffer == 0)
  {
    /*
     * This means that we should flush internal buffers.  Since we
     * don't we just return.  (Remember, "handle" == -1 means that all
     * handles should be flushed.)
     */
    return 0;
  }
  /* This template only writes to "standard out" and "standard err",
   * for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }
  int handler_mode = 0;
  int prio_level = 0;
  osThreadId thread_id = 0;
  t_write_threaded *wth;
  
  if (_inHandlerMode()) {
#ifdef DEBUGREPO_WRITE_THREADED_HANDLERS
    if (_isHandlerFinal()) {
        _debugrepoInsertHandlerMsgBlocking((const char *)buffer, size);
    } else {
      prio_level = _getHandlerPrioLevel();
      if ((prio_level < 0) || (prio_level >= DEBUGREPO_HANDLERS_NUMLEVELS)) {
        return size;
      }
      handler_mode = 1;
    }
#else
    _debugrepoInsertHandlerMsgBlocking((const char *)buffer, size);
    return size;
#endif    
  } else {
    if (osKernelRunning()) {
      thread_id = osThreadGetId();
    }
  }

  wth = _write_threaded_get_index(handler_mode, prio_level, thread_id);
  if (wth != NULL) {
    _write_threaded_write(wth, buffer, size);
  } else {
    while(1);
  }
  return size;
}

//*****************************************************************************
// Insertar un mensaje completo (hasta '\0') 
//
// Retorna 0 si error (repo lleno, mensaje no almacenado o almacenado sólo parcialmente), 
// 1 si operación terminada con éxito
//*****************************************************************************
int debugrepoInsertMsg (const char *msg) 
{
    unsigned int len;

    // Obtener la longitud del mensaje a insertar  
    len = getStringLength(msg);
    if (len > DEBUGREPO_MSG_MAXLEN) {
      /* Mensaje demasiado largo */
      return 0;
    }
    return debugrepoInsertMsgLen(msg, (uint16_t)len);
}

static char _binmsg[DEBUGREPO_MSG_MAXLEN];
int debugrepoInsertBinMsgLen (const char *msg, uint16_t len) {
  unsigned int from_idx, to_idx;

  for (from_idx=0, to_idx=0; (from_idx < len) && (to_idx < DEBUGREPO_MSG_MAXLEN); from_idx++) {
    if (to_idx+6 < DEBUGREPO_MSG_MAXLEN) {
      if (from_idx > 0) {
        _binmsg[to_idx++] = ' ';
      }
      _binmsg[to_idx++] = asHex((msg[from_idx] >> 4) & 0x0F);
      _binmsg[to_idx++] = asHex(msg[from_idx] & 0x0F);
      if ((msg[from_idx] >= 32) && (msg[from_idx] < 128)) {
        _binmsg[to_idx++] = '(';
        _binmsg[to_idx++] = msg[from_idx];
        _binmsg[to_idx++] = ')';
      }
    }
  }
  if ((to_idx + 3) >= DEBUGREPO_MSG_MAXLEN) {
    to_idx -= 3;
  }
  _binmsg[to_idx++] = '\r';
  _binmsg[to_idx++] = '\n';
  _binmsg[to_idx++] = '\0';
  return debugrepoInsertMsgLen (_binmsg, to_idx);
}

    
int debugrepoInsertMsgLen (const char *msg, uint16_t len) {
  int prio_level;
  
  if (_inHandlerMode()) {
    if (_isHandlerFinal()) {
      return _debugrepoInsertHandlerMsgBlocking(msg, len);
    } else {
      prio_level = _getHandlerPrioLevel();
      if ((prio_level < 0) || (prio_level >= DEBUGREPO_HANDLERS_NUMLEVELS)) {
        return 0;
      }
      return _debugrepoInsertHandlerMsgNonBlocking((uint32_t)prio_level, msg, len);
    }
  } else {
    return _debugrepoInsertMsgLen(msg, len);
  }
}

static int _debugrepoInsertHandlerMsgBlocking(const char *msg, uint16_t len) {
  int i;

  for (i=0; i < len; i++) {
    _debugrepoWaitUntilUARTFlag(UART_FLAG_TXE);
    DEBUGREPO_UART_HANDLE.Instance->DR = msg[i] & 0x00FF;
  }
  return 1;
}

static int _debugrepoInsertHandlerMsgNonBlocking(uint32_t prio_level, const char *msg, uint16_t len) {
  int i;
  
  if ((DEBUGREPO_HANDLERS_SIZE - _repo_handlers[prio_level].stats.items) < len) {
    return 0;
  }
  for (i = 0; i < len; i++) {
    if (!_debugrepoHandlerInsertChar(prio_level, (uint8_t)msg[i])) {
      return 0;
    }
  }
  return 1;
}

static int _debugrepoInsertMsgLen (const char *msg, uint16_t len) {
  int i;
  
    /* Esperar a que haya suficiente espacio para todo el mensaje.
        Para esperar:
        1) Liberar el mutex de acceso 
        2) Esperar unos pocos ticks (para dar tiempo a la tarea consumidora a 
           extraer algunos datos) 
        3) Retomar el mutex de acceso para volver a consultar
    */
    MUTEX_WAIT;
    do {
        if ((DEBUGREPO_SIZE - _repo.stats.items) < len) {
            // De momento no hay hueco suficiente para el mensaje => 
			// devolver mutex, esperar, y retomar mutex
            MUTEX_RELEASE;
            vTaskDelay(DEBUGREPO_INSERT_DELAY_TICKS);
            MUTEX_WAIT;
        } else {
            /* Ojo, salimos del bucle dentro de la sección crítica */
            break;
        }
    } while (1);
  
    /* Insertar el mensaje caracter a caracter; 
        en caso de error (por repositorio lleno, no debería ocurrir en circunstancias normaler), 
		el error de insercion queda registrado en las estadisticas. 
       Nota: no debería haber error ya que 
       nos hemos asegurado que hay suficiente hueco antes de empezar la inserción ...
    */
    for (i = 0; msg[i] != '\0'; i++) {
        if (!_debugrepoInsertChar_nomutex((uint8_t)msg[i])) {
            // Oops, error de inserción => liberar mutex y terminar
            MUTEX_RELEASE;
            return 0;
        }
        // Alargar el tiempo necesario para la operación forzará la aparición de conflictos de acceso
        DELAY;
    }
    MUTEX_RELEASE;
    return 1;
}

//*****************************************************************************
//
// Extraer un mensaje completo (hasta '\n' inclusive) 
//
// Retorna:
// a) >0 La longitud del mensaje extraido, 
// b) 0 si no hay mensaje que extraer, 
// c) -1 si el mensaje almacenado en el repositorio no cabe en el array que se pasa como parámetro
// d) -2 si los datos almacenados en el repositorio no contienen un mensaje completo (finalizado en '\n')
//*****************************************************************************
int debugrepoExtractMsg (char msg[], int maxlen)
{
    int len, tail;
    
    MUTEX_WAIT;
    if (0 == _repo.stats.items) {
        // No hay mensaje que extraer
        MUTEX_RELEASE;
        return 0;
    }

    // Calcular la longitud del mensaje a extraer.
    // Por seguridad, comprobar que no excedemos el tamaño máximo posible (DEBUGREPO_SIZE)
    for (len = 1, tail = _repo.tail; ('\n' != _repo.buffer[tail]) && (DEBUGREPO_SIZE >= len); len++) {
        tail = (tail + 1) % DEBUGREPO_SIZE;
        // Alargar el tiempo necesario para la operación forzará la aparición de conflictos de acceso
        DELAY;
    }
    if (DEBUGREPO_SIZE <= len) {
        // Hay datos, pero el mensaje no termina con '\n'
        MUTEX_RELEASE;
        return -2;
    }
    if (len >= maxlen) {
        // El mensaje que se debe extraer de la tail no cabe en el array que se ha pasado como argumento
        MUTEX_RELEASE;
        return -1;
    }

    // Extraer el mensaje y copiarlo a msg caracter a caracter
    len = 0;
    while (_repo.stats.items > 0) {
        _debugrepoExtractChar_nomutex ((uint8_t *)&msg[len]);
        if (msg[len++] == '\n') {
            // Cuando copiamos el '\n' hemos terminado
            break;
        }
        if (len+1 == maxlen) {
            // Por seguridad comprobamos que la longitud no excede el máximo permitido
            break;
        }
    }
    // Rellenamos con '\0' para finalizar la cadena
    msg[len] = '\0';
    
    MUTEX_RELEASE;
    return len;
}

static int _debugrepoHandlerExtractMsg (uint32_t prio_level, char msg[], int maxlen)
{
    int len, tail;
    
    if (0 == _repo_handlers[prio_level].stats.items) {
        return 0;
    }

    // Calcular la longitud del mensaje a extraer.
    // Por seguridad, comprobar que no excedemos el tamaño máximo posible (DEBUGREPO_HANDLERS_SIZE)
    for (len = 1, tail = _repo_handlers[prio_level].tail; ('\n' != _repo_handlers[prio_level].buffer[tail]) && (DEBUGREPO_HANDLERS_SIZE >= len); len++) {
        tail = (tail + 1) % DEBUGREPO_HANDLERS_SIZE;
    }
    if (DEBUGREPO_HANDLERS_SIZE <= len) {
        return -2;
    }
    if (len >= maxlen) {
        return -1;
    }

    // Extraer el mensaje y copiarlo a msg caracter a caracter
    len = 0;
    while (_repo_handlers[prio_level].stats.items > 0) {
        _debugrepoHandlerExtractChar (prio_level, (uint8_t *)&msg[len]);
        if (msg[len++] == '\n') {
            // Cuando copiamos el '\n' hemos terminado
            break;
        }
        if (len+1 == maxlen) {
            // Por seguridad comprobamos que la longitud no excede el máximo permitido
            break;
        }
    }
    // Rellenamos con '\0' para finalizar la cadena
    msg[len] = '\0';
    return len;
}


static char DEBUGREPO_STATS_HEADER_MSG[] =   "[debugrepoTask] Stats info            :: Total items | current items | insert errors\r\n";
static char DEBUGREPO_STATS_TASKS_MSG[] =    "[debugrepoTask]            Tasks msgs ::    NNNNNNNN |      NNNNNNNN |      NNNNNNNN\r\n";
static char DEBUGREPO_STATS_HANDLERS_MSG[] = "[debugrepoTask] Handlers  (prio PPPP) ::    NNNNNNNN |      NNNNNNNN |      NNNNNNNN\r\n";
static char DEBUGREPO_HANDLERS_ERROR_MSG[] = "[debugrepoTask] Handler msg (prio PPPP) extraction failed. Error code EEEE\r\n";
static char DEBUGREPO_ERROR_MSG[] =          "[debugrepoTask] Extraction failed. Error code EEEE\r\n";

#define DEBUGREPO_STATS_HEADER_MSG_LEN          (sizeof(DEBUGREPO_STATS_HEADER_MSG))
#define DEBUGREPO_STATS_TASKS_MSG_LEN           (sizeof(DEBUGREPO_STATS_TASKS_MSG))
#define DEBUGREPO_STATS_HANDLERS_MSG_LEN        (sizeof(DEBUGREPO_STATS_HANDLERS_MSG))
#define DEBUGREPO_HANDLERS_ERROR_MSG_LEN        (sizeof(DEBUGREPO_HANDLERS_ERROR_MSG))
#define DEBUGREPO_ERROR_MSG_LEN                 (sizeof(DEBUGREPO_ERROR_MSG))

typedef struct {
  unsigned int pos, len;
  char pad;
} t_format_info;

static t_format_info _tasks_msgs_format_info[] = {
  {44, 8, ' '},
  {60, 8, ' '},
  {76, 8, ' '}
};
#define DEBUGREPO_STATS_TASKS_MSG_FORMAT_NUM    (sizeof(_tasks_msgs_format_info) /sizeof(t_format_info))

static t_format_info _handlers_msgs_format_info[] = {
  {32, 4, ' '},
  {44, 8, ' '},
  {60, 8, ' '},
  {76, 8, ' '}
};
#define DEBUGREPO_STATS_HANDLERS_MSG_FORMAT_NUM    (sizeof(_handlers_msgs_format_info) /sizeof(t_format_info))

static t_format_info _handlers_error_msg_format_info[] = {
  {34, 4, ' '},
  {70, 4, ' '}
};
#define DEBUGREPO_HANDLERS_ERROR_MSG_FORMAT_NUM    (sizeof(_handlers_error_msg_format_info) /sizeof(t_format_info))

static t_format_info _error_msg_format_info[] = {
  {46, 4, ' '}
};
#define DEBUGREPO_ERROR_MSG_FORMAT_NUM    (sizeof(_error_msg_format_info) /sizeof(t_format_info))


static void _formatStatsMsg(t_debugrepo_stats *pstats, t_format_info* pformat, char *msg, unsigned int msg_len);
static void _formatStatsMsg(t_debugrepo_stats *pstats, t_format_info* pformat, char *msg, unsigned int msg_len) {
  unsigned int i;
  unsigned int stats[3] = {pstats->total_items, pstats->items, pstats->insert_errors};
  for (i=0; i < 3; i++) {
    formatUnsignedInt(&msg[pformat[i].pos], msg_len-pformat[i].pos, stats[i], pformat[i].len, pformat[i].pad);
  }
}

static void _debugrepoTask(void const * argument) {
  int i, extract_res = 0;
#ifdef DEBUGREPO_SEND_STATS
  t_debugrepo_stats stats;
  int stats_header_sent = 0;
  int count = 0;
  #ifdef DEBUGREPO_SEND_HANDLERS_STATS
  int handler_count = 0;
  #endif  
#endif  

  while (1) {

#ifdef DEBUGREPO_SEND_STATS
    stats_header_sent = 0;
    if (count == DEBUGREPO_SEND_STATS_EVERY) {
      count = 0;
      osSemaphoreWait(_repo.sem, osWaitForever);
      HAL_UART_Transmit_IT(&DEBUGREPO_UART_HANDLE, (uint8_t *)DEBUGREPO_STATS_HEADER_MSG, DEBUGREPO_STATS_HEADER_MSG_LEN);
      stats_header_sent = 1;
      _debugrepoGetStats(&stats);
      osSemaphoreWait(_repo.sem, osWaitForever);
      _formatStatsMsg(&stats, _tasks_msgs_format_info, DEBUGREPO_STATS_TASKS_MSG, DEBUGREPO_STATS_TASKS_MSG_LEN);
      HAL_UART_Transmit_IT(&DEBUGREPO_UART_HANDLE, (uint8_t *)DEBUGREPO_STATS_TASKS_MSG, DEBUGREPO_STATS_TASKS_MSG_LEN);
    }
#endif    
#ifdef DEBUGREPO_SEND_STATS
  #ifdef DEBUGREPO_SEND_HANDLERS_STATS
    if (handler_count == DEBUGREPO_SEND_HANDLERS_STATS_EVERY) {
      handler_count = 0;
      if (!stats_header_sent) {
        osSemaphoreWait(_repo.sem, osWaitForever);
        HAL_UART_Transmit_IT(&DEBUGREPO_UART_HANDLE, (uint8_t *)DEBUGREPO_STATS_HEADER_MSG, DEBUGREPO_STATS_HEADER_MSG_LEN);
      }
      for (i=0; i < DEBUGREPO_HANDLERS_NUMLEVELS; i++) {
        _debugrepoGetHandlerStats(i, &stats);
        osSemaphoreWait(_repo.sem, osWaitForever);
        //copyString(DEBUGREPO_STATS_HANDLERS_MSG, _msg2send, DEBUGREPO_MSG_MAXLEN);
        //formatInt(&_msg2send[_handlers_msgs_format_info[0].pos], DEBUGREPO_STATS_HANDLERS_MSG_LEN-_handlers_msgs_format_info[0].pos, -i, _handlers_msgs_format_info[0].len, _handlers_msgs_format_info[0].pad);
        formatUnsignedInt(&DEBUGREPO_STATS_HANDLERS_MSG[_handlers_msgs_format_info[0].pos], DEBUGREPO_STATS_HANDLERS_MSG_LEN-_handlers_msgs_format_info[0].pos, i, _handlers_msgs_format_info[0].len, _handlers_msgs_format_info[0].pad);
        _formatStatsMsg(&stats, &_handlers_msgs_format_info[1], DEBUGREPO_STATS_HANDLERS_MSG, DEBUGREPO_STATS_HANDLERS_MSG_LEN);
        HAL_UART_Transmit_IT(&DEBUGREPO_UART_HANDLE, (uint8_t *)DEBUGREPO_STATS_HANDLERS_MSG, DEBUGREPO_STATS_HANDLERS_MSG_LEN);
      }
    }
  #endif    
#endif    
    osSemaphoreWait(_repo.sem, osWaitForever);
    for (i=0; i < DEBUGREPO_HANDLERS_NUMLEVELS; i++) {
      extract_res = _debugrepoHandlerExtractMsg(i, _msg2send, DEBUGREPO_MSG_MAXLEN);
      if (extract_res > 0) {
        break;
      }
      if (extract_res < 0) {
        formatUnsignedInt(&DEBUGREPO_HANDLERS_ERROR_MSG[_handlers_error_msg_format_info[0].pos], DEBUGREPO_HANDLERS_ERROR_MSG_LEN-_handlers_error_msg_format_info[0].pos, i, _handlers_error_msg_format_info[0].len, _handlers_error_msg_format_info[0].pad);
        formatInt(&DEBUGREPO_HANDLERS_ERROR_MSG[_handlers_error_msg_format_info[1].pos], DEBUGREPO_HANDLERS_ERROR_MSG_LEN-_handlers_error_msg_format_info[1].pos, extract_res, _handlers_error_msg_format_info[1].len, _handlers_error_msg_format_info[1].pad);
        extract_res = DEBUGREPO_HANDLERS_ERROR_MSG_LEN;
        break;
      }
    }
    
    if (extract_res == 0) {
      extract_res = debugrepoExtractMsg(_msg2send, DEBUGREPO_MSG_MAXLEN);
    }
    if (extract_res == 0) {
      osSemaphoreRelease(_repo.sem);
      osDelay(DEBUGREPO_EXTRACT_DELAY_TICKS);
      continue;
    }
    if (extract_res < 0) {
      formatInt(&DEBUGREPO_ERROR_MSG[_error_msg_format_info[0].pos], DEBUGREPO_ERROR_MSG_LEN-_error_msg_format_info[0].pos, extract_res, _error_msg_format_info[1].len, _error_msg_format_info[1].pad);
      extract_res = DEBUGREPO_ERROR_MSG_LEN;
    }
    HAL_UART_Transmit_IT(&DEBUGREPO_UART_HANDLE, (uint8_t *)_msg2send, extract_res);
#ifdef DEBUGREPO_SEND_STATS
    count++;
  #ifdef DEBUGREPO_SEND_HANDLERS_STATS
    handler_count++;
  #endif    
#endif    
  }
}

void debugrepoUARTCallback(UART_HandleTypeDef *huart) {
  if (huart == &DEBUGREPO_UART_HANDLE) {
    osSemaphoreRelease(_repo.sem);
  }
}
