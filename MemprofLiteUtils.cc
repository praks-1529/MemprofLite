/*!****************************************************************************
 *
 *  \file       MemprofLiteUtils.cc
 *
 *  \brief
 *      
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      08/29/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MemprofLiteUtils.h>

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MemprofLiteUtils::MemprofLiteUtils(void) {
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
const char*
MemprofLiteUtils::GetENV(const char *a_env_variable) {
  return getenv(a_env_variable);
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
bool
MemprofLiteUtils::IsFolderWritable(const char* a_folder) {
  if(-1 != access(a_folder, W_OK)) {
    return true;
  } else {
    return false;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
bool
MemprofLiteUtils::IsFileExists(const char* a_folder) {
  if(-1 != access(a_folder, R_OK | W_OK)) {
    return true;
  } else {
    return false;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLiteUtils::LogError(int error_id, const char* a_msg) {
  switch(error_id) {
    case 1:
      printf("[ERROR-001] The folder %s is not writable\n", a_msg);
      break;

    case 2:
      printf("[ERROR-002] CWD is not writable\n");
      break;

    default:
      break;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLiteUtils::LogWarning(int error_id, const char* a_msg) {
  switch(error_id) {
    case 1:
      break;

    case 2:
      break;

    default:
      break;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 08/29/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MemprofLiteUtils::LogInfo(int error_id, const char* a_msg) {
  switch(error_id) {
    case 1:
      printf("[INFO-001] MemprofLite started successfully. Data is being written at %s\n", a_msg);
      break;

   case 2:
      printf("[INFO-002] The DB already exists @ %s. If you start/stop multiple times, ignore it, else this could show wrong results of shared libs. Memprofilte will continue as normal\n", a_msg);
      break;
   case 3:
     printf("[INFO-003] The data for thread=%s collected and dumped successfully\n", a_msg);
     break;

    default:
      break;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 09/02/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void 
MemprofLiteUtils::Tokenize(const char* ip_line, 
                           const char* delim, 
                           std::vector<std::string> &a_tokens) {
  a_tokens.clear();
  if(!ip_line) return;
  char *token;
  char *saveptr;
  token = strtok_r(const_cast<char*>(ip_line), delim , &saveptr);
  while(token) {
    a_tokens.push_back(std::string(token));
    token = strtok_r(NULL, delim , &saveptr);
  }
}
