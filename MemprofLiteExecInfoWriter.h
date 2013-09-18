/*!****************************************************************************
 *
 *  \file       MemprofLiteExecInfoWriter.h
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

#ifndef MEMPROFLITEEXECINFOWRITER_H
#define MEMPROFLITEEXECINFOWRITER_H

#include <libgen.h>
#include <bfd.h>
#include <vector>
#include <string>
#include <cxxabi.h>
#include <algorithm>
#include <MemprofLiteDB.h>
#include <MemprofLiteUtils.h>

#define ADDR_UNDEFINED (18446744073709551615U) //(2e+64 -1)

/*!****************************************************************************
 * \class            \author Prakash S - 09/04/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
class MapItem {
  public:
    MapItem() : start_(ADDR_UNDEFINED), name_("") { }
    MapItem(memproflite_addr_t_ a_start, std::string a_name) : 
       start_(a_start), name_(a_name) { }
    memproflite_addr_t_ start() const { return start_; }
    std::string name(void) const { return name_; }
    bool operator <(const MapItem& a_rhs) const  {
      if(start_ < a_rhs.start()) {
        return true;
      } else {
        return false;
      }
    }
  private:
    memproflite_addr_t_ start_;
    std::string name_; 
}; 

/*!****************************************************************************
 * \class            \author Prakash S - 09/04/2013
 *
 *  \brief 
 * 
 ******************************************************************************/
class MapItemCollection {
  public:
    MapItemCollection() { 
      map_vector_.clear();
    }
    void Insert(memproflite_addr_t_ a_addr, std::string a_func_name) {
      map_vector_.push_back(MapItem(a_addr, a_func_name));
    }
    std::string QueryName(memproflite_addr_t_ a_addr) {
      return BinarySearch(a_addr, 0, map_vector_.size()-1).name();
    }
    memproflite_addr_t_ QueryAddr(memproflite_addr_t_ a_addr) {
      return BinarySearch(a_addr, 0, map_vector_.size()-1).start();
    }
    void Prepare(void) {
      std::sort(map_vector_.begin(), map_vector_.end());
    }
  private:
   MapItem& BinarySearch(const memproflite_addr_t_ &addr,
                         memproflite_ul_t_ low, 
                         memproflite_ul_t_ high) {
      if(map_vector_.size() == 0) {
        return undefined_item_;
      }
#if LINEAR
      /* Lets do linear search for time being */
      for(memproflite_ul_t_ i=0; i<map_vector_.size()-1; i++) {
        if(addr >=  (map_vector_[i]).start() && addr < (map_vector_[i+1]).start()) {
          return (map_vector_[i]).name();
        }
      }
      return (map_vector_[map_vector_.size()-1]).name();
#else
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
#endif
    }
    std::vector<MapItem> map_vector_;
    MapItem undefined_item_;
};

/*!****************************************************************************
 * \class MemprofLiteExecInfoWriter 
 *  
 * \brief
 *      Writes all the executable info in to a db
 *
 * \details
 *
 * \author Prakash S
 *
 *  \internal
 *  HISTORY:
 *      07/28/2013 - Prakash S : Created
 * 
 ******************************************************************************/
class MemprofLiteExecInfoWriter {
  public:
    MemprofLiteExecInfoWriter() { }
    /*! Given a log folder, parses /proc/self/maps file and creates a db by the
     * name exec_name.db. Reads all the shared library info as well
     */
    static int GenerateMapFile(const char* a_log_folder);

  private:
    /*! Parse each line of proc_map file */
    static void ParseProcessMapLine(const char* a_map_line,
                                    MemprofLiteDB *db); 
    /*! Process a library and write its info */
    static int ProcessLib(const std::string &a_lib, 
                          memproflite_addr_t_ a_start_addr,
                          MemprofLiteDB *db); 
    /*! Process one bfd symbol */
    static void ProcessSymbol(asymbol *sym, 
                              MemprofLiteDB *db, 
                              const std::string &a_table_name);
    /*! Insert the entry in to DB */
    static void DBInsert(MemprofLiteDB *db, 
                         const std::string &a_table_name,
                         memproflite_addr_t_ start_addr, 
                         memproflite_addr_t_ end_addr, 
                         const char *func_name);
    /*! Gets the name of the binary */
    static void GetBinaryName(char* a_file, 
                              char *a_map_file_name);
    /*! Tells if the binary is stripped */
    static int IsStripped(const std::string &a_bin_name) { 
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
    /*! Checks if the permissions is r-X */
    static bool IsBinaryReadExecuteable(const std::string &a_permissions) { 
      //if(a_permissions[0] == 'r' && a_permissions[2] == 'x') {
      if(!(a_permissions.compare("r-xp"))) {
        return true;
      } else {
        return false;
      }
    }
    /*! Tells if the lib is shared or static */
    static bool IsStaticLib(const std::string &a_bin_name) { 
      if(a_bin_name.find(".so") != std::string::npos) {
        return false;
      } else {
        return true;
      }
    }

    /*! Tells if the binary is ELF format or not */
    static int IsELFFormat(const std::string &a_bin_name) {
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
    /*! Chomp the string */
    static std::string chomp(const std::string &a_str) {
      std::string ret = a_str;
      if(ret[ret.size()-1] == '\n') {
        ret[ret.size()-1] = '\0';
      }
      return ret;
    }
    /*! Get base name from the path */
    static std::string Basename(const std::string &a_str) {
      const char* b_name = basename(const_cast<char*>(a_str.data()));
      std::string ret;
      int i=-1;
      while(b_name[++i] != '\0') {
        if((b_name[i] >= 'a' && b_name[i] <= 'z') ||
           (b_name[i] >= 'A' && b_name[i] <= 'Z') ||
           (b_name[i] >= '0' && b_name[i] <= '9')) {
          ret += b_name[i];
        }
      }
      return ret;
    }
    /*! Remove all the useless symbols and filter only functions */
    static memproflite_ll_t_ RemoveUselessSymbols(asymbol **symbols, 
                                                  memproflite_ll_t_ count);
    static int compare_symbols (const void *ap, 
                                const void *bp);
    /*! Demangle the name */
    static std::string DemangleName(const char* a_mangled_name) { 
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
};

#endif    /* MEMPROFLITEEXECINFOWRITER_H */

