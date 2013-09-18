/*!****************************************************************************
 *
 *  \file       memproflite_int.cc
 *
 *  \brief
 *     Memproflite core implementation 
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      07/25/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <assert.h>
#include <dlfcn.h>
#include <iostream>
#include <assert.h>
#include <MemprofLite.h>
#include <MemprofLiteExecInfoWriter.h>
#include <memproflite.h>

#ifdef CUSTOM_ALLOC 
#ifdef __cplusplus
extern "C" {
#endif

  extern void* mem_mt_malloc(size_t size);
  extern void mem_free(void* block);

#ifdef __cplusplus
}
#endif // end #ifdef extern "C"
#endif // end #ifdef CUSTOM_ALLOC

/*! Static variables of singleton class */
bool MemprofLite::instance_flag_       = false;
MemprofLite MemprofLite::memproflite_;
pthread_key_t MemprofLite::file_ptr_key_;
bool static_start = true;

/*! Static functions */
static void 
free_thread_specific_context(void *a_data) {
  ThreadContext *thread_specific_context = static_cast<ThreadContext*>(a_data);
  pthread_mutex_lock( &MemprofLite::getInstance()->memproflite_mutex() );
  /* Lock and say everybody that i am freeing the context */
  MemprofLite::getInstance()->set_free_in_progress(thread_specific_context->tid());
  delete thread_specific_context;
  /* Freeing done, so unset the gvar */
  MemprofLite::getInstance()->set_free_in_progress(-1);
  pthread_mutex_unlock( &MemprofLite::getInstance()->memproflite_mutex() );
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void* 
memproflite_malloc_cpp(size_t a_size) {
#if START_STATIC
  if(!MemprofLite::getInstance()->is_profiler_on() && static_start) {
    static_start=false;
    memproflite_start_cpp();
  }
#endif
  return MemprofLite::getInstance()->MemprofLite_malloc(a_size);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void memproflite_free_cpp(void* a_ptr) {
  MemprofLite::getInstance()->MemprofLite_free(a_ptr);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
memproflite_start_cpp(void) {
  MemprofLite::getInstance()->Start();
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
memproflite_stop_cpp(void) {
  MemprofLite::getInstance()->Stop();
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void register_malloc_cpp(void* (*a_real_malloc)(size_t)) {
  MemprofLite::getInstance()->set_real_malloc(a_real_malloc);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void register_free_cpp(void (*a_real_free)(void*)) {
  MemprofLite::getInstance()->set_real_free(a_real_free);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLite::MemprofLite() {
  instance_flag_      = false;
  pthread_mutex_init(&memproflite_mutex_, NULL);
  pthread_key_create(&file_ptr_key_, free_thread_specific_context);
  free_in_progress_ = -1;
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLite::~MemprofLite() {
  if(is_profiler_on_) {
    is_profiler_on_ = false;
    /* First delet the thread specific context of main thread */
    ThreadContext *thread_specific_context = GetThreadContext();
    pthread_mutex_lock( &MemprofLite::getInstance()->memproflite_mutex() );
    MemprofLite::getInstance()->set_free_in_progress(thread_specific_context->tid());
    delete thread_specific_context;
    /* Now delete shared obj */
    if(env_info_) {
      delete env_info_;
    }
    MemprofLite::getInstance()->set_free_in_progress(-1);
    pthread_mutex_unlock( &MemprofLite::getInstance()->memproflite_mutex() );
    /* Now delete the mutexes */
    pthread_mutex_destroy(&memproflite_mutex_);
    pthread_key_delete(file_ptr_key_);
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLite*
MemprofLite::getInstance(void) {
  if(instance_flag_) {
   return &memproflite_;
  } else {
    memproflite_.instance_flag_ = true;
    return &memproflite_;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void 
MemprofLite::Init() {
  if(real_malloc_ && real_free_) {
    is_initialized_ = true;
    return;
  }
#ifdef CUSTOM_ALLOC
  real_malloc_ = &mem_mt_malloc; 
  real_free_ = &mem_free; 
#else
  real_malloc_ = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc"); 
  real_free_ = (void (*)(void*))(dlsym(RTLD_NEXT, "free")); 
#endif
  if(!(real_malloc_ && real_free_)) {
    real_malloc_ = (void* (*)(size_t))NULL;
    real_free_ = (void (*)(void*))NULL;
  }
  is_initialized_ = true;
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLite::Start() { 
  /*! First read all ENV vars and create respective objects */
  CreateENVInfo();
  if(env_info()->log_info()->is_valid()) {
    /* Create some internal DB's */
    MemprofLiteExecInfoWriter::GenerateMapFile(env_info()->log_info()->log_folder());
    /* Start the profiler */
    MemprofLiteUtils::LogInfo(1, env_info()->log_info()->log_folder());
    is_profiler_on_ = true;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLite::Stop() {
  is_profiler_on_ = false;
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void*
MemprofLite::MemprofLite_malloc(size_t a_size) {
  if(is_profiler_on_ && is_safe()) {
    void* ret_pt = malloc_low(a_size);
    ThreadContext *thread_specific_context = GetThreadContext();
    assert(thread_specific_context);
    if(thread_specific_context && thread_specific_context->is_safe()) {
       thread_specific_context->set_is_safe(false);
       /* All tracking code (malloc code) must go here */
       thread_specific_context->record_malloc(a_size, ret_pt);
       thread_specific_context->set_is_safe(true);
    }
    return ret_pt;
  } else {
    //printf("Here\n");
    return malloc_low(a_size);
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLite::MemprofLite_free(void* a_ptr) {
  if(is_profiler_on_ && is_safe()) {
    ThreadContext *thread_specific_context = GetThreadContext();
    if(thread_specific_context->is_safe()) {
      thread_specific_context->set_is_safe(false);
      thread_specific_context->record_free(a_ptr);
      thread_specific_context->set_is_safe(true);
    }
    return free_low(a_ptr);
  } else {
    return free_low(a_ptr);
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
inline void* 
MemprofLite::malloc_low(size_t a_size) {
  if(!is_initialized_) {
    Init();
  } 
  if(NULL == real_malloc_) {
    /* Something went wrong in dllsym() call */
    return malloc(a_size);
  } else {
    void* ret_pt = (*real_malloc_)(a_size);
    return ret_pt;
  } 
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
inline void
MemprofLite::free_low(void* a_ptr) {
  if(!is_initialized_) {
    Init();
  } 
  if(NULL == real_free_) {
    /* Something went wrong in dllsym() call */
    return free(a_ptr);
  } else {
    return (*real_free_)(a_ptr);
  } 
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/28/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
ThreadContext*
MemprofLite::GetThreadContext(void) {
  ThreadContext *context=static_cast<ThreadContext*>(pthread_getspecific(file_ptr_key_));
  if(NULL == context) {
    memproflite_pthread_t_ tid = pthread_self();
    void* (*real_malloc)(size_t);
#ifdef CUSTOM_ALLOC
  real_malloc = &mem_mt_malloc; 
#else
  real_malloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc"); 
#endif
    void *buf = ((*real_malloc)(sizeof(MemprofLite)));
    context = new(buf) ThreadContext(tid, env_info_->log_info()->log_folder());
    pthread_setspecific(file_ptr_key_, (void*)context);
  } 
  return context;
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLite::CreateENVInfo(void) {
  if(env_info_) {
    /*! Already initialized */
    return;
  } else {
    env_info_ = new MemprofLiteENV();
    MemprofLiteENV_LOG_FOLDER *log_info = new MemprofLiteENV_LOG_FOLDER(MemprofLiteUtils::GetENV("MPL_LOG_FOLDER"));
    MemprofLiteENV_FILTER *filter_info  = new MemprofLiteENV_FILTER(MemprofLiteUtils::GetENV("MPL_LOG_FILTER"));
    MemprofLiteENV_FOCUS *focus_info    = new MemprofLiteENV_FOCUS(MemprofLiteUtils::GetENV("MPL_LOG_FOCUS"));
    env_info_->set_log_info(log_info);
    env_info_->set_filter_info(filter_info);
    env_info_->set_focus_info(focus_info);
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/02/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
bool
MemprofLite::is_safe(void) {
  if(pthread_self() != free_in_progress_) {
    return true;
  } else {
    return false;
  }
}
