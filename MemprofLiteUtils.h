/*!****************************************************************************
 *
 *  \file       MemprofLiteUtils.h
 *
 *  \brief
 *      Some utilities used throughout MemprofLite
 *
 *  \details
 *      
 *  \internal
 *  HISTORY:
 *      07/25/2013 - Prakash S : Created
 *
 *****************************************************************************/

#ifndef MEMPROFLITEUTILS_H
#define MEMPROFLITEUTILS_H

 

/*--------------------------------------------------------------
 *
 *      Includes
 */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite/sqlite3.h>
#include <string>
#include <vector>

/*--------------------------------------------------------------
 *
 *      Data Declarations
 */

typedef size_t memproflite_size_t_ ;
typedef unsigned long long memproflite_ull_t_ ;
typedef unsigned long memproflite_ul_t_ ;
typedef long long memproflite_ll_t_ ;
typedef long memproflite_l_t_ ;
typedef void* memproflite_voidp_;
typedef pthread_t memproflite_pthread_t_;
typedef unsigned long memproflite_addr_t_;

#define MPL_ADDR(x) ((memproflite_addr_t_)x)

/*--------------------------------------------------------------
 *
 *      Macro Definitions
 */
#define MAX_TOKEN_LENGTH 4096

#define MEMPROFLITE_FAILED (-1)
#define MEMPROFLITE_SUCCESS (0)
 

/*--------------------------------------------------------------
 *
 *      Function Declarations
 */

 

/*--------------------------------------------------------------
 *
 *      Class Declarations
 */

/*!****************************************************************************
 * \class MemprofLiteUtils           \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
class MemprofLiteUtils {
  public:
    MemprofLiteUtils(void);
    static const char* GetENV(const char* a_env_variable);
    static bool IsFolderWritable(const char* a_folder);
    static void LogError(int error_id, const char* a_msg="");
    static void LogWarning(int error_id, const char* a_msg="");
    static void LogInfo(int error_id, const char* a_msg="");
    static bool IsFileExists(const char* a_file);
    static void Tokenize(const char* ip_line, 
                         const char* delim, 
                         std::vector<std::string> &a_op);
};

#endif    /* MEMPROFLITEUTILS_H */

