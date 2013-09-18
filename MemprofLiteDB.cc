/*!****************************************************************************
 *
 *  \file       MemprofLiteDB.cc
 *
 *  \brief
 *      
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      07/30/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <stdlib.h>
#include <MemprofLiteDB.h>
#include <iostream>
#include <assert.h>

/*!****************************************************************************
 * \class            \author Prakash S - 09/02/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLiteDB::MemprofLiteDB(const char* a_db_name) {
  if(sqlite3_open(a_db_name, &db_) != SQLITE_OK) {
    db_ = NULL;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/02/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLiteDB::~MemprofLiteDB(void) {
  sqlite3_close(db_);
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/02/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
bool
MemprofLiteDB::MemprofLite_execute1(const char* statement) {
  char *err;
  //std::cout << statement << std::endl;
  if(sqlite3_exec(db_, statement, NULL, NULL, &err) != SQLITE_OK) {
    printf("Error: %s\n", err);
    return false;
  } else {
    return true;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/02/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
std::string
MemprofLiteDB::MemprofLite_get_TEXT(const char* statement) {
  //std::cout << statement << std::endl;
  const unsigned char* ret_val=NULL;
  sqlite3_stmt *stmt;
  if (sqlite3_prepare(db_, statement, -1, &stmt, 0 ) == SQLITE_OK )
  {
    if(SQLITE_ROW == sqlite3_step(stmt)) {
      ret_val = sqlite3_column_text(stmt,0);
    }
  }
  if(NULL != ret_val) {
    std::string ret_val_s((const char*)ret_val);
    sqlite3_finalize(stmt);
    return ret_val_s;
  } else {
    sqlite3_finalize(stmt);
    return "";
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/02/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
const memproflite_ll_t_
MemprofLiteDB::MemprofLite_get_INTEGER(const char *statement) {
  memproflite_ull_t_ ret_val=-1;
  sqlite3_stmt *stmt;
  if (sqlite3_prepare(db_, statement, -1, &stmt, 0 ) == SQLITE_OK )
  {
    if(SQLITE_ROW == sqlite3_step(stmt)) {
      ret_val = (memproflite_ll_t_)sqlite3_column_double(stmt,0);
    }
  }
  sqlite3_finalize(stmt);
  return ret_val;
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/03/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLiteDB::BeginTransaction(void) {
  /*! SYnc off */ 
  MemprofLite_execute1("PRAGMA synchronous = OFF");
  /*! STart transaction */
  MemprofLite_execute1("BEGIN TRANSACTION ");
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/03/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLiteDB::EndTransaction(void) {
  MemprofLite_execute1("END TRANSACTION ");
}
