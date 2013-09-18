/*!****************************************************************************
 *
 *  \file       ThreadContext.h
 *
 *  \brief
 *      Thread context created per thread
 *
 *  \details
 *      
 *  \internal
 *  HISTORY:
 *      07/25/2013 - Prakash S : Created
 *
 *****************************************************************************/

#ifndef THREADCONTEXT_H
#define THREADCONTEXT_H

#include <execinfo.h>
#include <MemprofLiteUtils.h>
#include <MemprofLiteHook.h>

/*!****************************************************************************
 * \class ThreadContext 
 *  
 * \brief
 *
 * \details
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      07/23/2013 - Prakash S : Created
 * 
 ******************************************************************************/
class ThreadContext {
  public:
    /*! Constructor */
    ThreadContext(memproflite_pthread_t_ &a_tid,
                  const char* a_log_folder) {
      is_safe_  = true;
      tid_      = a_tid;
      hook_     = NULL;
      log_folder_ = a_log_folder;
    }
    ~ThreadContext() {
      is_safe_ = false;
      if(hook_) {
        /* This must inturn close DB handle */
        delete hook_;
        hook_ = NULL;
      }
    }
    /*! Accessors */
    bool is_safe(void) const { return is_safe_; }
    memproflite_pthread_t_ tid(void) const { return tid_; }

    /* Mutators */
    void set_is_safe(bool a_value) { is_safe_ = a_value; }

    /*! Records the malloc */
    void record_malloc(memproflite_size_t_ &a_size,
                       memproflite_voidp_ a_ptr) {
      if(hook_) {
        hook_->RecordMalloc(a_size, a_ptr);
      } else {
        hook_ = new MemprofLiteHookDefault(tid_, log_folder_);
        hook_->RecordMalloc(a_size, a_ptr);
      }
    }
    /*! Records the free */
    void record_free(memproflite_voidp_ a_ptr) {
      if(hook_) {
        hook_->RecordFree(a_ptr);
      } else {
        hook_ = new MemprofLiteHookDefault(tid_, log_folder_);
        hook_->RecordFree(a_ptr);
      }
    }

  private:
   /*! The log folder */
   const char* log_folder_;
   /* All malloc/free are safe */
   bool is_safe_;
   /*! Id of the thread */
   memproflite_pthread_t_ tid_;
   /*! The hook */
   IMemprofLiteHook* hook_;
};

#endif    /* THREADCONTEXT_H */

