/*!****************************************************************************
 *
 *  \file       memproflite.c
 *
 *  \brief
 *      Implementation of memproflie
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      07/12/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <memproflite.h>
#include <MemprofLite.h>

/*!****************************************************************************
 * \class malloc           \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void* malloc(size_t a_size) {
  return memproflite_malloc_cpp(a_size);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void free(void* a_ptr) {
  return memproflite_free_cpp(a_ptr);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void* memproflite_malloc(size_t a_size) {
  return memproflite_malloc_cpp(a_size);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void memproflite_free(void* a_ptr) {
  return memproflite_free_cpp(a_ptr);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
memproflite_start(void) {
  return memproflite_start_cpp();
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
memproflite_stop(void) {
  return memproflite_stop_cpp();
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void register_malloc(void* (*a_real_malloc)(size_t)) {
  return register_malloc_cpp(a_real_malloc);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void register_free(void (*a_real_free)(void*)) {
  return register_free_cpp(a_real_free);
}
