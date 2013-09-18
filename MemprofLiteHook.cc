/*!****************************************************************************
 *
 *  \file       MemprofLiteHook.cc
 *
 *  \brief
 *      
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      08/10/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <iostream>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <MemprofLite.h>
#include <MemprofLiteHook.h>
#include <MemprofLiteExecInfoWriter.h>

#define FUNC_DELIM (";")

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLiteHookDefault::MemprofLiteHookDefault(memproflite_pthread_t_ &a_tid,
                                               const char* a_log_folder) : 
                        log_folder_(a_log_folder), tid_(a_tid), db_(NULL), 
                        ip_db_(NULL), malloc_count_(0), free_count_(0){
  /* Open up a handle to common db */
  sprintf(db_buf_, "%s/exec_info.db", a_log_folder);
  ip_db_ = new MemprofLiteDB(db_buf_);
  //InitThreadData();
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/31/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLiteHookDefault::InitThreadData(void) {
  /* Create a thread specific DB */
  sprintf(db_buf_, "%s/%lu.db", log_folder_, (memproflite_ul_t_)tid_);
  db_ = new MemprofLiteDB(db_buf_);
  /* Create 4 tables */
  db_->MemprofLite_execute1("CREATE TABLE [func_prof_table] (func_name TEXT, malloc_count INTEGER, free_count INTEGER, last_op INTEGER)");
  //db_->MemprofLite_execute1("CREATE TABLE [lib_prof_tbl] (lib_name TEXT, malloc_count INTEGER, free_count INTEGER, last_op INTEGER)");
  db_->MemprofLite_execute1("CREATE TABLE [malloc_track] (ptr INTEGER, func_name TEXT, size INTEGER)");
  db_->MemprofLite_execute1("CREATE TABLE [malloc_realtime] (time INTEGER, cur_memory INTEGER)");
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLiteHookDefault::~MemprofLiteHookDefault() {
  if(db_) {
    delete db_;
    db_ = NULL;
  }
  if(ip_db_) {
    delete ip_db_;
    ip_db_ = NULL;
  }
  std::map<memproflite_addr_t_, memproflite_ull_t_>::iterator it;
#if TEXT_OP
  sprintf(db_buf_, "%s/%lu.txt", log_folder_, (memproflite_ul_t_)tid_);
#else 
  sprintf(db_buf_, "%s/%lu.db", log_folder_, (memproflite_ul_t_)tid_);
#endif
  if(access(db_buf_, R_OK) == -1) {
#if TEXT_OP
    FILE *fp = fopen(db_buf_, "a+");
    fprintf(fp, "Total malloc = %lld\n", malloc_count_);
    fprintf(fp, "Total free = %lld\n", free_count_);
    for(it=addr_vs_malloc_.begin(); it!=addr_vs_malloc_.end(); it++) {
      fprintf(fp, "%s | %lld | %lld\n", ResolveFuncName((*it).first).c_str(), (*it).second, addr_vs_free_[((*it).first)]);
    }
#else
    db_ = new MemprofLiteDB(db_buf_);
    db_->BeginTransaction();
    /* Create 4 tables */
    db_->MemprofLite_execute1("CREATE TABLE [func_prof_table] (func_name TEXT, malloc_count INTEGER, free_count INTEGER)");
    for(it=addr_vs_malloc_.begin(); it!=addr_vs_malloc_.end(); it++) {
      sprintf(db_buf_, "INSERT INTO [func_prof_table] VALUES(\"%s\", %lld, %lld)", ResolveFuncName((*it).first).c_str(), (*it).second, addr_vs_free_[((*it).first)]);
      db_->MemprofLite_execute1(db_buf_);
    }
    db_->EndTransaction();
#endif
    sprintf(db_buf_, "%lu", (memproflite_ul_t_)tid_);
    MemprofLiteUtils::LogInfo(3, db_buf_);
  } 
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void 
MemprofLiteHookDefault::RecordMalloc(memproflite_size_t_ &a_size, 
                                     memproflite_voidp_ a_ptr) {
  /*! Increment the global counter */
  malloc_count_ += a_size;
  /*! Get the backtrace */
  int nptrs = backtrace(stack_buffer_, STACK_LENGTH);
  std::set<memproflite_addr_t_> *cur_alloc = new std::set<memproflite_addr_t_>;
  memproflite_addr_t_ func_start_addr;
  for (int j = 0; j < nptrs; j++) {
    if(ADDR_UNDEFINED != (func_start_addr = ResolveFuncAddr(MPL_ADDR(stack_buffer_[j])))) {
      if(0 == cur_alloc->count(func_start_addr)) {
        cur_alloc->insert(func_start_addr);
        addr_vs_malloc_[func_start_addr] += a_size;  
      }
    }
  }
  ptr_vs_info_[MPL_ADDR(a_ptr)] = std::pair<std::set<memproflite_addr_t_>*, memproflite_size_t_>(cur_alloc, a_size);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void 
MemprofLiteHookDefault::RecordFree(memproflite_voidp_ a_ptr) {
  int count;
  /* Get the size from the pointer value */
  if((count = ptr_vs_info_.count((MPL_ADDR(a_ptr))))) {
    std::map< memproflite_addr_t_, AllocInfo >::iterator it = ptr_vs_info_.find(MPL_ADDR(a_ptr));
    assert(count == 1);
    std::set<memproflite_addr_t_> *alloc        = (it->second).first;
    memproflite_size_t_ size                    = (it->second).second;
    free_count_ += size;
    std::set<memproflite_addr_t_>::iterator it2;
    for(it2 = alloc->begin(); it2 != alloc->end(); it2++) {
      addr_vs_free_[(*it2)] += size;
    }
    delete alloc;
    ptr_vs_info_.erase(it); 
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/30/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
memproflite_addr_t_ 
MemprofLiteHookDefault::ResolveFuncAddr(memproflite_addr_t_ a_addr) {
  MapItemCollection &map_collection = MemprofLite::getInstance()->env_info()->map_collection();
  return (map_collection.QueryAddr(a_addr));
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/05/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
std::string
MemprofLiteHookDefault::ResolveFuncName(memproflite_addr_t_ a_addr) {
  MapItemCollection &map_collection = MemprofLite::getInstance()->env_info()->map_collection();
  return map_collection.QueryName(MPL_ADDR(a_addr));
}
