/**
  ******************************************************************************
  * @file    ../middlewares/inc/m_scheduler.h
  * @author  yhangzzz
  * @version V1.0.0
  * @date    2010.10.22
  * @brief   m_scheduler.h
  ******************************************************************************
  */ 

#ifndef M_SCHEDULER_H__
#define M_SCHEDULER_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define M_SCHED_EVENT_HEADER_SIZE 8      /**< Size of m_scheduler.event_header_t (only for use inside M_SCHED_BUF_SIZE()). */



/** @defgroup ERRORS_BASE Error Codes Base number definitions
 * @{ */
#define ERROR_BASE_NUM      (0x0)       ///< Global error base
#define ERROR_SDM_BASE_NUM  (0x1000)    ///< SDM error base
#define ERROR_SOC_BASE_NUM  (0x2000)    ///< SoC error base
#define ERROR_STK_BASE_NUM  (0x3000)    ///< STK error base
/** @} */

#define SUCCESS                           (ERROR_BASE_NUM + 0)  ///< Successful command
#define ERROR_SVC_HANDLER_MISSING         (ERROR_BASE_NUM + 1)  ///< SVC handler is missing
#define ERROR_SOFTDEVICE_NOT_ENABLED      (ERROR_BASE_NUM + 2)  ///< SoftDevice has not been enabled
#define ERROR_INTERNAL                    (ERROR_BASE_NUM + 3)  ///< Internal Error
#define ERROR_NO_MEM                      (ERROR_BASE_NUM + 4)  ///< No Memory for operation
#define ERROR_NOT_FOUND                   (ERROR_BASE_NUM + 5)  ///< Not found
#define ERROR_NOT_SUPPORTED               (ERROR_BASE_NUM + 6)  ///< Not supported
#define ERROR_INVALID_PARAM               (ERROR_BASE_NUM + 7)  ///< Invalid Parameter
#define ERROR_INVALID_STATE               (ERROR_BASE_NUM + 8)  ///< Invalid state, operation disallowed in this state
#define ERROR_INVALID_LENGTH              (ERROR_BASE_NUM + 9)  ///< Invalid Length
#define ERROR_INVALID_FLAGS               (ERROR_BASE_NUM + 10) ///< Invalid Flags
#define ERROR_INVALID_DATA                (ERROR_BASE_NUM + 11) ///< Invalid Data
#define ERROR_DATA_SIZE                   (ERROR_BASE_NUM + 12) ///< Invalid Data size
#define ERROR_TIMEOUT                     (ERROR_BASE_NUM + 13) ///< Operation timed out
#define ERROR_NULL                        (ERROR_BASE_NUM + 14) ///< Null Pointer
#define ERROR_FORBIDDEN                   (ERROR_BASE_NUM + 15) ///< Forbidden Operation
#define ERROR_INVALID_ADDR                (ERROR_BASE_NUM + 16) ///< Bad Memory Address
#define ERROR_BUSY                        (ERROR_BASE_NUM + 17) ///< Busy
#define ERROR_CONN_COUNT                  (ERROR_BASE_NUM + 18) ///< Maximum connection count exceeded.
#define ERROR_RESOURCES                   (ERROR_BASE_NUM + 19) ///< Not enough resources for operation

/**@brief Macro for calling error handler function if supplied error code any other than SUCCESS.
 *
 * @param[in] ERR_CODE Error code supplied to the error handler.
 */
#define M_ERROR_CHECK(ERR_CODE)   {}



/**@brief Macro for entering a critical region.
 *
 * @note Due to implementation details, there must exist one and only one call to
 *       CRITICAL_REGION_EXIT() for each call to CRITICAL_REGION_ENTER(), and they must be located
 *       in the same scope.
 */
#define CRITICAL_REGION_ENTER()
#define CRITICAL_REGION_EXIT()
 

/**@brief Function for checking if a pointer value is aligned to a 4 byte boundary.
 *
 * @param[in]   p   Pointer value to be checked.
 *
 * @return      TRUE if pointer is aligned to a 4 byte boundary, FALSE otherwise.
 */
static inline bool is_word_aligned(void const* p)
{
    return (((uintptr_t)p & 0x03) == 0);
}

/**@brief Macro for performing integer division, making sure the result is rounded up.
 *
 * @details One typical use for this is to compute the number of objects with size B is needed to
 *          hold A number of bytes.
 *
 * @param[in]   A   Numerator.
 * @param[in]   B   Denominator.
 *
 * @return      Integer result of dividing A by B, rounded up.
 */
#define CEIL_DIV(A, B)      \
    (((A) + (B) - 1) / (B))
    
    
 /************  Memory distribution ******************************************************
              -------------------------    	offset   type     size
              m_sched_event_handler_t    	0        (*f)()   4
              event_data_size							 	4				int16    2    align     event_header_t  
                        
              m_sched_event_handler_t    	8        (*f)()   4
              event_data_size							 	12				int16    2    align     event_header_t
              
              ...
              ...														
              															
              event_data[event_data_size] 	(queue_size+1)* M_SCHED_EVENT_HEADER_SIZE(8)
              event_data[event_data_size]	
              ...	
              ...          									total queue_size+1
              (queue_size+1)*event_data_size
                    
**/
/**@brief Compute number of bytes required to hold the scheduler buffer.
 *
 * @param[in] EVENT_SIZE   Maximum size of events to be passed through the scheduler.
 * @param[in] QUEUE_SIZE   Number of entries in scheduler queue (i.e. the maximum number of events
 *                         that can be scheduled for execution).
 *
 * @return    Required scheduler buffer size (in bytes).
 */
#define M_SCHED_BUF_SIZE(EVENT_SIZE, QUEUE_SIZE)                                                 \
            (((EVENT_SIZE) + M_SCHED_EVENT_HEADER_SIZE) * ((QUEUE_SIZE) + 1))

/**@brief Scheduler event handler type. */
typedef void (*m_sched_event_handler_t)(void * p_event_data, uint16_t event_size);

/**@brief Macro for initializing the event scheduler.
 *
 * @details It will also handle dimensioning and allocation of the memory buffer required by the
 *          scheduler, making sure the buffer is correctly aligned.
 *
 * @param[in] EVENT_SIZE   Maximum size of events to be passed through the scheduler.
 * @param[in] QUEUE_SIZE   Number of entries in scheduler queue (i.e. the maximum number of events
 *                         that can be scheduled for execution).
 *
 * @note Since this macro allocates a buffer, it must only be called once (it is OK to call it
 *       several times as long as it is from the same location, e.g. to do a reinitialization).
 */
#define M_SCHED_INIT(EVENT_SIZE, QUEUE_SIZE)                                                     \
    do                                                                                             \
    {                                                                                              \
        static uint32_t M_SCHED_BUF[CEIL_DIV(M_SCHED_BUF_SIZE((EVENT_SIZE), (QUEUE_SIZE)),     \
                                               sizeof(uint32_t))];                                 \
        uint32_t ERR_CODE = m_sched_init((EVENT_SIZE), (QUEUE_SIZE), M_SCHED_BUF);             \
        M_ERROR_CHECK(ERR_CODE);                                                                 \
    } while (0)

/**@brief Function for initializing the Scheduler.
 *
 * @details It must be called before entering the main loop.
 *
 * @param[in]   max_event_size   Maximum size of events to be passed through the scheduler.
 * @param[in]   queue_size       Number of entries in scheduler queue (i.e. the maximum number of
 *                               events that can be scheduled for execution).
 * @param[in]   p_evt_buffer   Pointer to memory buffer for holding the scheduler queue. It must
 *                               be dimensioned using the M_SCHED_BUFFER_SIZE() macro. The buffer
 *                               must be aligned to a 4 byte boundary.
 *
 * @note Normally initialization should be done using the M_SCHED_INIT() macro, as that will both
 *       allocate the scheduler buffer, and also align the buffer correctly.
 *
 * @retval      SUCCESS               Successful initialization.
 * @retval      ERROR_INVALID_PARAM   Invalid parameter (buffer not aligned to a 4 byte
 *                                        boundary).
 */
uint32_t m_sched_init(uint16_t max_event_size, uint16_t queue_size, void * p_evt_buffer);

/**@brief Function for executing all scheduled events.
 *
 * @details This function must be called from within the main loop. It will execute all events
 *          scheduled since the last time it was called.
 */
void m_sched_execute(void);

/**@brief Function for scheduling an event.
 *
 * @details Puts an event into the event queue.
 *
 * @param[in]   p_event_data   Pointer to event data to be scheduled.
 * @param[in]   event_size     Size of event data to be scheduled.
 * @param[in]   handler        Event handler to receive the event.
 *
 * @return      SUCCESS on success, otherwise an error code.
 */
uint32_t m_sched_event_put(void const *              p_event_data,
                             uint16_t                  event_size,
                             m_sched_event_handler_t handler);

/**@brief Function for getting the maximum observed queue utilization.
 *
 * Function for tuning the module and determining QUEUE_SIZE value and thus module RAM usage.
 *
 * @note @ref M_SCHEDULER_WITH_PROFILER must be enabled to use this functionality.
 *
 * @return Maximum number of events in queue observed so far.
 */
uint16_t m_sched_queue_utilization_get(void);

/**@brief Function for getting the current amount of free space in the queue.
 *
 * @details The real amount of free space may be less if entries are being added from an interrupt.
 *          To get the sxact value, this function should be called from the critical section.
 *
 * @return Amount of free space in the queue.
 */
uint16_t m_sched_queue_space_get(void);

/**@brief A function to pause the scheduler.
 *
 * @details When the scheduler is paused events are not pulled from the scheduler queue for
 *          processing. The function can be called multiple times. To unblock the scheduler the
 *          function @ref m_sched_resume has to be called the same number of times.
 *
 * @note @ref M_SCHEDULER_WITH_PAUSE must be enabled to use this functionality.
 */
void m_sched_pause(void);

/**@brief A function to resume a scheduler.
 *
 * @details To unblock the scheduler this function has to be called the same number of times as
 *          @ref m_sched_pause function.
 *
 * @note @ref M_SCHEDULER_WITH_PAUSE must be enabled to use this functionality.
 */
void m_sched_resume(void);

#ifdef __cplusplus
}
#endif

#endif // M_SCHEDULER_H__

