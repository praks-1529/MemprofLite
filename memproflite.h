/*!****************************************************************************
 *
 *  \file       memproflite.h
 *
 *  \brief
 *     Exported API from memproflite 
 *
 *  \details
 *      
 *  \internal
 *  HISTORY:
 *      07/15/2013 - Prakash S : Created
 *
 *****************************************************************************/

#ifndef MEMPROFLITE_H
#define MEMPROFLITE_H
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Start MemprofLite */
extern void memproflite_start(void);
/*! Stop MemprofLite */
extern void memproflite_stop(void);
/*! Explicit call to malloc */
extern void* memproflite_malloc(size_t a_size);
/*! Explicit call to free */
extern void memproflite_free(void* a_ptr);
/*! Register custom malloc */
extern void register_malloc(void* (*a_real_malloc)(size_t));
/*! Register custom free */
extern void register_free(void (*a_real_free)(void*));

#ifdef __cplusplus
}
#endif

#endif    /* MEMPROFLITE_H */

