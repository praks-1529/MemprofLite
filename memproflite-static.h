/*!****************************************************************************
 *
 *  \file       memproflite-static.h
 *
 *  \brief
 *      Some product statically build/link to some third party libs which re-implement
 *      malloc/free. In that case, our malloc will never be invoked because
 *      the static malloc/free gets precedence. Hence we #define it to our 
 *      version of malloc 
 *
 *  \details
 *      
 *  \internal
 *  HISTORY:
 *      08/19/2013 - Prakash S : Created
 *
 *****************************************************************************/

#ifndef MEMPROFLITE_STATIC_H
#define MEMPROFLITE_STATIC_H

#define malloc(x) memproflite_malloc(x)
#define free(x)   memproflite_free(x)

#endif    /* MEMPROFLITE_STATIC_H */

