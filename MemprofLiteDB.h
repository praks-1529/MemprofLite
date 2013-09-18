/*!****************************************************************************
 *
 *  \file       MemprofLiteDB.h
 *
 *  \brief
 *      The core DB writer and reader in Memproflite. Uses sqlite3
 *
 *  \details
 *      
 *  \internal
 *  HISTORY:
 *      07/28/2013 - Prakash S : Created
 *
 *****************************************************************************/

#ifndef MEMPROFLITEDB_H
#define MEMPROFLITEDB_H

/*--------------------------------------------------------------
 *
 *      Includes
 */
#include <sqlite/sqlite3.h>
#include <MemprofLiteUtils.h>

class MemprofLiteDB {
  public:
    /*! COnstructor */
    MemprofLiteDB(const char *a_db_name);
    /*! Destructor */
    ~MemprofLiteDB(void);
    /*! Execute the statement */
    bool MemprofLite_execute1(const char* statement);
    /*! Get the text */
    std::string MemprofLite_get_TEXT(const char* statement);
    /*! Get the INTEGER */
    const memproflite_ll_t_ MemprofLite_get_INTEGER(const char *statement);
    /*! Begin transaction */
    void BeginTransaction(void);
    /*! End transaction */
    void EndTransaction(void);
  private:
    sqlite3 *db_;
};

#endif    /* MEMPROFLITEDB_H */

