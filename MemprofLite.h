/*!****************************************************************************
 *
 *  \file       memprof_lite_int.h
 *
 *  \brief
 *    Internal header for mempro_lite  
 *
 *  \details
 *      
 *  \internal
 *  HISTORY:
 *      07/12/2013 - Prakash S : Created
 *
 *****************************************************************************/
#ifndef MEMPROFLITE_INT_H
#define MEMPROFLITE_INT_H

/*--------------------------------------------------------------
 *
 *      Includes
 */
#include <pthread.h>
#include <list>
#include <MemprofLiteUtils.h>
#include <ThreadContext.h>
#include <MemprofLiteExecInfoWriter.h>

/*--------------------------------------------------------------
 *
 *      Class Declarations
 */
extern void* memproflite_malloc_cpp(size_t a_size);
extern void memproflite_free_cpp(void* a_ptr);
extern void memproflite_stop_cpp(void);
extern void memproflite_start_cpp(void);
extern void register_malloc_cpp(void* (*a_real_malloc)(size_t));
extern void register_free_cpp(void (*a_real_free)(void*));

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
class MemprofLiteENV_LOG_FOLDER {
  public:
    /*! COnstructor */
    MemprofLiteENV_LOG_FOLDER(const char* a_log_folder) {
      if(!a_log_folder || !(MemprofLiteUtils::IsFolderWritable(a_log_folder))) {
        if(a_log_folder) {
          /* User specfied a un-writable folder*/
          MemprofLiteUtils::LogError(1, a_log_folder);
        }
        if(!MemprofLiteUtils::IsFolderWritable(".")) {
          /* CWD not writable either */
          MemprofLiteUtils::LogError(2);
          log_folder_ = db_name_ = "";
          return;
        } else {
          log_folder_ = std::string("./");
        } 
      } else {
        log_folder_ = std::string(a_log_folder);
      } 
      db_name_ = log_folder_ + "/exec_info.db";
    }
    /*! Log folder */
    const char* log_folder(void) const { return log_folder_.c_str(); }
    /*! DB name */
    const char* db_name(void) const { return db_name_.c_str(); }
    /*! Is data valid */
    bool is_valid(void) {
       if(log_folder_.compare("")) {
         return true;
       } else {
         return false;
       }
    }
  private:
    std::string log_folder_;
    std::string db_name_;
};

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
class MemprofLiteENV_FILTER {
  public:
    /*! Constructor */
    MemprofLiteENV_FILTER(const char* a_filter) {
    }
    bool is_valid(void) {
      if(filter_by_.size() != 0) {
        return true;
      } else {
        return false;
      }
    }
  private:
    std::list<std::string> filter_by_;
};

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
class MemprofLiteENV_FOCUS {
  public:
    /*! Constructor */
    MemprofLiteENV_FOCUS(const char* a_focus) {
    }
    bool is_valid(void) {
      if(focus_by_.size() != 0) {
        return true;
      } else {
        return false;
      }
    }
  private:
    std::list<std::string> focus_by_;
};

/*!****************************************************************************
 * \class MemprofLiteENV           \author Prakash S - 08/29/2013
 *
 *  \brief Object of this class is shared by all the threads
 * 
 ******************************************************************************/
class MemprofLiteENV {
  public:
    /*! Constructor */
    MemprofLiteENV(void) : log_info_(NULL), filter_info_(NULL), 
                           focus_info_(NULL), is_explicit_(false), is_valid_(false) {
    }
    /*! mutators */
    void set_log_info(MemprofLiteENV_LOG_FOLDER* a_log_info) {
      log_info_ = a_log_info;
    }
    void set_filter_info(MemprofLiteENV_FILTER* a_filter_info) {
      filter_info_ = a_filter_info;
    }
    void set_focus_info(MemprofLiteENV_FOCUS *a_focus_info) {
      focus_info_ = a_focus_info;
    }
    void set_is_explicit(bool a_is_explicit) {
      is_explicit_ = a_is_explicit;
    }
    void set_is_valid(bool a_is_valid) {
     is_valid_ = a_is_valid;
    }
    /*! All accessors */
    MemprofLiteENV_LOG_FOLDER* log_info(void) {
      return log_info_;
    }
    MemprofLiteENV_FILTER* filter_info(void) {
      return filter_info_;
    }
    MemprofLiteENV_FOCUS* focus_info(void) {
      return focus_info_;
    }
    MapItemCollection& map_collection(void) {
      return map_collection_;
    }
    bool is_explicit(void) const {
      return is_explicit_;
    }
    bool is_valid(void) const { 
      return is_valid_;
    }
  private:
    MemprofLiteENV_LOG_FOLDER* log_info_;
    MemprofLiteENV_FILTER* filter_info_;
    MemprofLiteENV_FOCUS* focus_info_;
    MapItemCollection map_collection_;
    bool is_explicit_;
    bool is_valid_;
};

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief Singleton class of MemprofLite
 * 
 ******************************************************************************/
class MemprofLite {
  public:
    /*! Destructor */
    ~MemprofLite(); 
    /*! Gets the singleton instance */
    static MemprofLite* getInstance();
    /*! Start profiling */
    void Start(void);
    /*! Stop profiling */
    void Stop(void);
    /* Overidden malloc */
    void* MemprofLite_malloc(size_t a_size);
    /* Overidden malloc */
    void MemprofLite_free(void* a_ptr);
    /*! Gets the context */
    ThreadContext* GetThreadContext(void);
    /*! Accessor */
    bool is_profiler_on(void) const { 
      return is_profiler_on_; 
    }
    /*! Mutator */
    void set_real_malloc(void* (*a_real_malloc)(size_t)) {
      real_malloc_ = a_real_malloc;
    }
    void set_real_free(void (*a_real_free)(void*)) {
      real_free_ = a_real_free;
    }
    pthread_mutex_t& memproflite_mutex(void) {
       return memproflite_mutex_;
    }
    void set_free_in_progress(memproflite_pthread_t_ a_cur_thread) {
      free_in_progress_ = a_cur_thread;
    }
    bool is_safe(void);
    MemprofLiteENV* env_info(void) {
      return env_info_;
    }

  private:
    /*! Set env info */
    void CreateENVInfo(void);
    /*! Private constructor*/
    MemprofLite(void);
    /*! Initializing */
    void Init(void);
    /*! malloc_low */
    void* malloc_low(size_t a_size);
    /*! malloc_free */
    void free_low(void* a_ptr);
    /*! Profiler flag */
    bool memprofilelite_on_;
    /*! Initialized */
    bool is_initialized_;
    /*! Is profiler on */
    bool is_profiler_on_;
    /*! original_malloc_*/
    void* (*real_malloc_)(size_t);
    /*! original_free_*/
    void (*real_free_)(void*);
    /*! The instance flag */
    static bool instance_flag_; 
    /*! The singleton instance */
    static MemprofLite memproflite_;
    /*! Mutex for a syncronization */
    pthread_mutex_t memproflite_mutex_;
    /*! The thread specific key */
    static pthread_key_t file_ptr_key_;
    /*! The shared info object */
    MemprofLiteENV *env_info_; 
    /*! Freeing_thread_ */
    memproflite_pthread_t_ free_in_progress_;
};

#define  GETENV() (MemprofLite::getInstance()->env_info())

#endif    /* MEMPROFLITE_INT_H */
