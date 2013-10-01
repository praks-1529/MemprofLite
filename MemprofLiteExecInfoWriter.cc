/*!****************************************************************************
 *
 *  \file       MemprofLiteExecInfoWriter.cc
 *
 *  \brief
 *      
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      07/28/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <unistd.h>
#include <string>
#include <vector>
#include <string.h>
#include <assert.h>
#include <MemprofLite.h>
#include <MemprofLiteExecInfoWriter.h>
#include <MemprofLiteUtils.h>

#define MEMPROFLITE_SYM_VAL(x) ( x == (memproflite_addr_t_)-1 ? 0 : x )

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void
MapItemCollection::InsertLib(memproflite_addr_t_ a_start_addr, 
                             memproflite_addr_t_ a_end_addr, 
                             std::string a_func_name) {
  exclude_lib_.push_back(std::pair<memproflite_addr_t_, memproflite_addr_t_>(a_start_addr, a_end_addr));
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void 
MapItemCollection::Insert(memproflite_addr_t_ a_addr, 
    std::string a_func_name) {
  map_vector_.push_back(MapItem(a_addr, a_func_name));
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
std::string 
MapItemCollection::QueryName(memproflite_addr_t_ a_addr) {
  for(unsigned int i=0; i<exclude_lib_.size(); i++) {
    if(a_addr >= exclude_lib_[i].first && a_addr<= exclude_lib_[i].second) {
      return BinarySearch(a_addr, 0, map_vector_.size()-1).name();
    }
  }
  return undefined_item_.name();
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
memproflite_addr_t_ 
MapItemCollection::QueryAddr(memproflite_addr_t_ a_addr) {
  for(unsigned int i=0; i<exclude_lib_.size(); i++) {
    if(a_addr >= exclude_lib_[i].first && a_addr<= exclude_lib_[i].second) {
      return BinarySearch(a_addr, 0, map_vector_.size()-1).start();
    }
  }
  return undefined_item_.start();
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
void 
MapItemCollection::Prepare(void) {
  std::sort(map_vector_.begin(), map_vector_.end());
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
MapItem& 
MapItemCollection::BinarySearch(const memproflite_addr_t_ &addr,
                                memproflite_ul_t_ low, 
                                memproflite_ul_t_ high) {
  if(map_vector_.size() == 0) {
    return undefined_item_;
  }
  if(low >= high) {
    if(low == 0) {
      return undefined_item_;
    } else {
      return map_vector_[low];
    }
  }else {
    memproflite_ul_t_ mid = (low+high)/2;
    if(mid == 0 && high == 1) {
      if((addr >= map_vector_[0].start() && addr <= map_vector_[1].start())) {
        return map_vector_[0];
      } else {
        return undefined_item_;
      }
    } else if(mid == 0 && high == 0) {
      if(addr >= map_vector_[0].start()) {
        return map_vector_[0];
      } else {
        /* If it is less than start(), we can definitely say it is undefined */
        return undefined_item_;
      }
    }
    if(addr == map_vector_[mid].start()) {
      return map_vector_[mid];
    } else if(addr == map_vector_[mid+1].start()) {
      return map_vector_[mid+1];
    } else if(addr == map_vector_[mid-1].start()) {
      return map_vector_[mid-1];
    } else if(addr >= map_vector_[mid].start() && addr < map_vector_[mid+1].start()) {
      return map_vector_[mid];
    } else if(addr <= map_vector_[mid].start() && addr >= map_vector_[mid-1].start()) {
      return map_vector_[mid-1];
    } else if(addr > map_vector_[mid].start() && addr > map_vector_[mid+1].start()) {
      return BinarySearch(addr, mid+1, high);
    } else {
      return BinarySearch(addr, low, mid-1);
    }
  }
}
/*!****************************************************************************
 * \class  MemprofLiteExecInfoWriter
 * 
 * \internal 
 * method GenerateMapFile
 *
 * \brief 
 *      Given a log folder, parses /proc/self/maps file and creates
 *      map file for each of the shared library and execs
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      04/25/2013 - Prakash S : Created
 * 
 ******************************************************************************/
int
MemprofLiteExecInfoWriter::GenerateMapFile(const char* a_log_folder) {
  FILE *fpr;
  if (NULL == (fpr=fopen("/proc/self/maps", "r"))) {
    return MEMPROFLITE_FAILED;
  }
  /* Create a db */
  std::string db_name(a_log_folder);
  db_name += "/exec_info.db";
  if(MemprofLiteUtils::IsFileExists(db_name.c_str())) {
    MemprofLiteUtils::LogInfo(2, db_name.c_str());
    return MEMPROFLITE_SUCCESS;
  }
  MemprofLiteDB db(db_name.c_str());
  /* Create a table that holds start and end address of all libs */
  db.MemprofLite_execute1("CREATE TABLE [lib_start_end_addr] (start INTEGER, end INTEGER, lib_basename TEXT, lib TEXT)");
  /*! Create a table that holds the start and end address of all functions */
  db.MemprofLite_execute1("CREATE TABLE [func_start_end_addr] (start_addr INTEGER, end_addr INTEGER, func_name TEXT)");
  /* Parse line by line and create map files for each of them */  
  char tbuf[MAX_TOKEN_LENGTH];
  while(fgets(tbuf, MAX_TOKEN_LENGTH, fpr)) {
    ParseProcessMapLine(tbuf, &db);
  };
  fclose(fpr);
  (GETENV()->map_collection()).Prepare();
  return MEMPROFLITE_SUCCESS;
}

/*!****************************************************************************
 * \class MemprofLiteExecInfoWriter 
 * 
 * \internal 
 * method ParseProcessMap
 *
 * \brief 
 *      Parse each line of the file and create a table in the data base  
 *      corresponding shared lib/exec. 
 *      The format of each line is as below
 *
 *      start_addr-end_addr perms offset  dev   inode   pathname
 *      40000000-40013000   r-xp 00000000 03:0c 4165    /lib/ld-2.2.4.so
 *      08058000-0805b000   rwxp 00000000 00:00 0
 *
 *      Caution!! pathname could be empty in some case as shown above
 *      We will be only interested in start_addr-end_addr and pathname.
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      04/25/2013 - Prakash S : Created
 * 
 ******************************************************************************/
void
MemprofLiteExecInfoWriter::ParseProcessMapLine(const char* a_map_line, 
                                               MemprofLiteDB *db) {
  char buf[MAX_TOKEN_LENGTH];
  std::vector<std::string> tokens;
  MemprofLiteUtils::Tokenize(a_map_line," ", tokens);
  if(tokens.size() == 6) {
    std::string exec_name = chomp(tokens[5]);
    if(IsELFFormat(exec_name) && 
       !IsStripped(exec_name) && 
       IsBinaryReadExecuteable(tokens[1]) &&
       !IsLibFiltered(exec_name)) {
      memproflite_addr_t_ start_addr, end_addr;
      start_addr = end_addr = 0;
      start_addr = strtol(tokens[0].substr(0, (tokens[0].size() - tokens[0].find_first_of('-',0)+1)).c_str(), NULL, 16);
      end_addr   = strtol(tokens[0].substr(tokens[0].find_first_of('-',0)+1, (tokens[0].size()-tokens[0].find_first_of('-',0)+1)).c_str(), NULL, 16); 
      (GETENV()->map_collection()).InsertLib(start_addr, end_addr, exec_name);
      if(IsStaticLib(exec_name)) {
        start_addr = end_addr = 0;
      }
      sprintf(buf, "INSERT INTO [lib_start_end_addr] VALUES(%lu, %lu, \"%s\",\"%s\")", MPL_ADDR(start_addr), MPL_ADDR(end_addr) , Basename(exec_name).c_str(), exec_name.c_str());
      db->MemprofLite_execute1(buf);
      ProcessLib(exec_name, start_addr, db);
    }
  }
}

/*!****************************************************************************
 * \class  
 * 
 * \internal 
 * method
 *
 * \brief 
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      07/29/2013 - Prakash S : Created
 * 
 ******************************************************************************/
int 
MemprofLiteExecInfoWriter::ProcessLib(const std::string &a_lib, 
                                      memproflite_addr_t_ a_start_addr,
                                      MemprofLiteDB *db) {
  /* Get all the symbols in the binary */
  bfd *abfd;
  abfd = bfd_openr(a_lib.c_str(), NULL);
  if(!bfd_check_format(abfd, bfd_object)) {
    return MEMPROFLITE_FAILED;
  } 
  long storage = bfd_get_symtab_upper_bound(abfd);
  asymbol **symbol_table;
  symbol_table = (asymbol**)malloc(storage);
  memproflite_ll_t_ number_of_symbols = bfd_canonicalize_symtab(abfd, 
                                                                symbol_table);
  /* Remove some useless symbols here */
  memproflite_ll_t_ actual_symbols = RemoveUselessSymbols(symbol_table, 
                                                          number_of_symbols);  
  assert(actual_symbols <= number_of_symbols);
  if(!actual_symbols) { 
    return MEMPROFLITE_SUCCESS;
  }
  /* Sort based on starting addresses */
  qsort (symbol_table, 
         actual_symbols, 
         sizeof (asymbol *), 
         MemprofLiteExecInfoWriter::compare_symbols);

  /*! Create a table by the name Basename(<lib name>) */
  db->BeginTransaction();
  /*! The addresses are of the type bfd_vma which are unsigned long */
  memproflite_addr_t_ start_a, end_a;
  MapItemCollection &map_collection = GETENV()->map_collection();
  for (int i = 0; i < actual_symbols-1; i++) {
    start_a = MEMPROFLITE_SYM_VAL(bfd_asymbol_value(symbol_table[i]));
    end_a   = MEMPROFLITE_SYM_VAL(bfd_asymbol_value(symbol_table[i+1])-1);
    /*! Insert in func_start_end_addr also */
    DBInsert(db,
             "func_start_end_addr",
             start_a + a_start_addr,
             end_a + a_start_addr,
             DemangleName(symbol_table[i]->name).c_str());
    map_collection.Insert((start_a + a_start_addr), DemangleName(symbol_table[i]->name));
  }
  db->EndTransaction();
  free(symbol_table);
  bfd_close(abfd);
  return MEMPROFLITE_SUCCESS;
}

/*!****************************************************************************
 * \class  
 * 
 * \internal 
 * method
 *
 * \brief 
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      07/29/2013 - Prakash S : Created
 * 
 ******************************************************************************/
void 
MemprofLiteExecInfoWriter::DBInsert(MemprofLiteDB *db, 
                                    const std::string &a_table_name,
                                    memproflite_addr_t_ start_addr, 
                                    memproflite_addr_t_ end_addr, 
                                    const char *func_name) {
  if(end_addr == start_addr-1) {
    end_addr = start_addr;
  }
  assert(start_addr <= end_addr);
  char buf[4096] = {'\0'};
  sprintf(buf, "INSERT INTO [%s] VALUES( %lu, %lu, \"%s\" )", a_table_name.c_str(), MPL_ADDR(start_addr), MPL_ADDR(end_addr), func_name);
#if DEBUG
  //printf("%s\n", buf);
#endif
  db->MemprofLite_execute1(buf);
}

/*!****************************************************************************
 * \class  
 * 
 * \internal 
 * method
 *
 * \brief 
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      08/08/2013 - Prakash S : Created
 * 
 ******************************************************************************/
memproflite_ll_t_ 
MemprofLiteExecInfoWriter::RemoveUselessSymbols(asymbol **symbols, 
                                                memproflite_ll_t_ count) {
  asymbol **in_ptr = symbols, **out_ptr = symbols;
  while (--count >= 0)
  {
    asymbol *sym = *in_ptr++;
    if (sym->name == NULL || sym->name[0] == '\0')
      continue;
    if (sym->flags & (BSF_SECTION_SYM))
      continue;
    /* This is to remove purify inserted symbols */
    if (sym->flags == 144)
      continue;
    if (bfd_is_und_section (sym->section)
        || bfd_is_com_section (sym->section))
      continue;
    *out_ptr++ = sym;
  }
  return out_ptr - symbols;
}

/*!****************************************************************************
 * \class  
 * 
 * \internal 
 * method
 *
 * \brief 
 *      This function has been taken from objdump source code, so credit goes 
 *      original author 
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      08/08/2013 - Prakash S : Created
 * 
 ******************************************************************************/
int 
MemprofLiteExecInfoWriter::compare_symbols (const void *ap, const void *bp) {
  const asymbol *a = * (const asymbol **) ap;
  const asymbol *b = * (const asymbol **) bp;
  const char *an;
  const char *bn;
  size_t anl;
  size_t bnl; 
  bfd_boolean af;
  bfd_boolean bf;
  flagword aflags;
  flagword bflags;

  if (bfd_asymbol_value (a) > bfd_asymbol_value (b))
    return 1;
  else if (bfd_asymbol_value (a) < bfd_asymbol_value (b))
    return -1;

  if (a->section > b->section)
    return 1;
  else if (a->section < b->section)
    return -1;

  an = bfd_asymbol_name (a);
  bn = bfd_asymbol_name (b);
  anl = strlen (an);
  bnl = strlen (bn);

  /* The symbols gnu_compiled and gcc2_compiled convey no real
   *      information, so put them after other symbols with the same value.  */
  af = (strstr (an, "gnu_compiled") != NULL
      || strstr (an, "gcc2_compiled") != NULL);
  bf = (strstr (bn, "gnu_compiled") != NULL
      || strstr (bn, "gcc2_compiled") != NULL);

  if (af && ! bf)
    return 1; 
  if (! af && bf)
    return -1;
  /* We use a heuristic for the file name, to try to sort it after
   *      more useful symbols.  It may not work on non Unix systems, but it
   *           doesn't really matter; the only difference is precisely which
   *                symbol names get printed.  */

#define file_symbol(s, sn, snl)         \
  (((s)->flags & BSF_FILE) != 0         \
   || ((sn)[(snl) - 2] == '.'           \
     && ((sn)[(snl) - 1] == 'o'       \
       || (sn)[(snl) - 1] == 'a')))

  af = file_symbol (a, an, anl);
  bf = file_symbol (b, bn, bnl);

  if (af && ! bf)
    return 1; 
  if (! af && bf)
    return -1;

  /* Try to sort global symbols before local symbols before function
   *      symbols before debugging symbols.  */

  aflags = a->flags;
  bflags = b->flags;

  if ((aflags & BSF_DEBUGGING) != (bflags & BSF_DEBUGGING))
  {
    if ((aflags & BSF_DEBUGGING) != 0)
      return 1;
    else
      return -1;
  }
  if ((aflags & BSF_FUNCTION) != (bflags & BSF_FUNCTION))
  {
    if ((aflags & BSF_FUNCTION) != 0)
      return -1;
    else
      return 1;
  }
  if ((aflags & BSF_LOCAL) != (bflags & BSF_LOCAL))
  {
    if ((aflags & BSF_LOCAL) != 0)
      return 1;
    else
      return -1;
  }
  if ((aflags & BSF_GLOBAL) != (bflags & BSF_GLOBAL))
  {
    if ((aflags & BSF_GLOBAL) != 0)
      return -1;
    else
      return 1;
  }
  /* Symbols that start with '.' might be section names, so sort them
   *      after symbols that don't start with '.'.  */
  if (an[0] == '.' && bn[0] != '.')
    return 1;
  if (an[0] != '.' && bn[0] == '.')
    return -1;

  /* Finally, if we can't distinguish them in any other way, try to
   *      get consistent results by sorting the symbols by name.  */
  return strcmp (an, bn);
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
int 
MemprofLiteExecInfoWriter::IsStripped(const std::string &a_bin_name) { 
  bfd *abfd = bfd_openr(a_bin_name.c_str(), NULL);
  if(!abfd) {
    return true;
  } 
  if(!bfd_check_format(abfd, bfd_object)) {
    bfd_close(abfd);
    return true;
  } 
  if(0 == bfd_get_symtab_upper_bound(abfd)) {
    bfd_close(abfd);
    return true; 
  } else { 
    bfd_close(abfd);
    return false; 
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
bool 
MemprofLiteExecInfoWriter::IsBinaryReadExecuteable(const std::string &a_permissions) { 
  //if(a_permissions[0] == 'r' && a_permissions[2] == 'x') {
  if(!(a_permissions.compare("r-xp"))) {
    return true;
  } else {
    return false;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
bool 
MemprofLiteExecInfoWriter::IsStaticLib(const std::string &a_bin_name) { 
  if(a_bin_name.find(".so") != std::string::npos) {
    return false;
  } else {
    return true;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
int 
MemprofLiteExecInfoWriter::IsELFFormat(const std::string &a_bin_name) {
  if(-1 == access(a_bin_name.c_str(), R_OK)) {
    return false;
  } 
  bfd *abfd = bfd_openr(a_bin_name.c_str(), NULL);
  if(!abfd) {
    return false;
  } else {
    if(!bfd_check_format(abfd, bfd_object)) {
      bfd_close(abfd);
      return false;
    } 
    bfd_close(abfd);
    return true;
  }
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
bool 
MemprofLiteExecInfoWriter::IsLibFiltered(const std::string &a_bin_name) {
  if(!GETENV()->filter_info()->is_valid()) return false;
  std::vector<std::string> filter_list = GETENV()->filter_info()->filter_by();
  for(unsigned int i=0; i<filter_list.size(); i++) {
    if(std::string::npos != a_bin_name.rfind(filter_list[i])) {
      return true;
    }
  }
  return false;
}

/*!****************************************************************************
 * \class            \author Prakash S - 10/01/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
std::string 
MemprofLiteExecInfoWriter::DemangleName(const char* a_mangled_name) { 
#ifndef DEMANGLE
  return std::string(a_mangled_name);
#else
  int status;
  char * realname = abi::__cxa_demangle(a_mangled_name, 0, 0, &status);
  if(0 == status) {
    std::string s_real(realname);
    free(realname);
    return s_real;
  } else {
    return a_mangled_name;
  }
#endif
}
