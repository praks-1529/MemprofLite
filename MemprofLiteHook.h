/*!****************************************************************************
 *
 *  \file       MemprofLiteHook.h
 *
 *  \brief
 *      The hooks of MemprofLite
 *
 *  \details
 *      
 *  \internal
 *  HISTORY:
 *      08/10/2013 - Prakash S : Created
 *
 *****************************************************************************/

#ifndef MEMPROFLITEHOOK_H
#define MEMPROFLITEHOOK_H

/*--------------------------------------------------------------
 *
 *      Includes
 */
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <MemprofLiteUtils.h>
#include <MemprofLiteDB.h>

#define STACK_LENGTH 256

/*--------------------------------------------------------------
 *
 *      Class Declarations
 */

/*!****************************************************************************
 * \class IMemprofLiteHook 
 *  
 * \brief
 *      Interface class for all types of hooks. Any one who wants to hook to 
 *      MemprofLite must derive from this and do whatever he wants to do in 
 *      it
 *
 * \details
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      08/10/2013 - Prakash S : Created
 * 
 ******************************************************************************/
class IMemprofLiteHook {
  public :
    /*! Virtual destructor */
   virtual ~IMemprofLiteHook() { }
   /*! Malloc hook */
   virtual void RecordMalloc(memproflite_size_t_ &a_size, 
                             memproflite_voidp_ a_ptr)=0; 
   /*! Free hook */
   virtual void RecordFree(memproflite_voidp_ a_ptr)=0;
};
 
typedef std::pair<std::set<memproflite_addr_t_>*, memproflite_size_t_> AllocInfo;
/*!****************************************************************************
 * \class MemprofLiteHookDefault 
 *  
 * \brief
 *      Default hook for memproflite
 *
 * \details
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      08/10/2013 - Prakash S : Created
 * 
 ******************************************************************************/
class MemprofLiteHookDefault : public IMemprofLiteHook {
  public:
    MemprofLiteHookDefault(memproflite_pthread_t_ &a_tid,
                           const char* a_log_folder);
    ~MemprofLiteHookDefault();
    /*! Malloc hook */
    void RecordMalloc(memproflite_size_t_ &a_size, 
                      memproflite_voidp_ a_ptr);
    /*! Free hook */
    void RecordFree(memproflite_voidp_ a_ptr);

  private:
    /* Resolves a function address */
    memproflite_addr_t_ ResolveFuncAddr(memproflite_addr_t_ a_addr);
    /* Resolves a function name*/
    std::string ResolveFuncName(memproflite_addr_t_ a_addr);
     /* Initialize data of the thread */
    void InitThreadData(void);
    /* The log folder */
    const char* log_folder_;
    /*! Id of the thread */
    memproflite_pthread_t_ tid_;
    /*! DB to hold all the info */
    MemprofLiteDB *db_;
    /*! Last timestamp at which the entries were written */
    memproflite_ull_t_ last_write_time_;
    /*! Input DB */
    MemprofLiteDB *ip_db_;
   /* Buffer used to collect stack */
    void *stack_buffer_[STACK_LENGTH];
    /*! Temp buf */
    char db_buf_[4096];
    /*! Start address vs malloc_count */
    std::map<memproflite_addr_t_, memproflite_ull_t_> addr_vs_malloc_;
    /*! Start address vs free_count */
    std::map<memproflite_addr_t_, memproflite_ull_t_> addr_vs_free_;
    /*! COunt of malloc so far */
    memproflite_ll_t_ malloc_count_;
    /*! COunt of free so far */
    memproflite_ll_t_ free_count_;
    /*! Pointer vs stack */
    std::map< memproflite_addr_t_, AllocInfo > ptr_vs_info_;
};

#endif    /* MEMPROFLITEHOOK_H */

