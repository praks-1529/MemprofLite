/*!****************************************************************************
 *
 *  \file       main2.cc
 *
 *  \brief
 *      
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      09/05/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <MemprofLiteExecInfoWriter.h>
#include <iostream>
using namespace std;
int main() {
  MapItemCollection map;
  map.Insert(5, "5");
  map.Insert(10, "10");
  map.Insert(15, "15");
  map.Insert(20, "20");
  map.Insert(25, "40");
  map.Insert(30, "45");
  cout << "output=";
  cout << map.QueryAddr(2) << " "; //Before range
  cout << map.QueryAddr(5) << " "; // exact range
  cout << map.QueryAddr(7) << " "; // Inbetween range
  cout << map.QueryAddr(10) << " "; // Inbetween range
  cout << map.QueryAddr(13) << " "; // Inbetween range
  cout << map.QueryAddr(15) << " "; // Inbetween range
  cout << map.QueryAddr(17) << " "; // Inbetween range
  cout << map.QueryAddr(20) << " "; // Inbetween range
  cout << map.QueryAddr(24) << " ";
  cout << map.QueryAddr(25) << " ";
  cout << map.QueryAddr(28) << " ";
  cout << map.QueryAddr(30) << " ";
  cout << map.QueryAddr(32) << " ";
  cout << endl;
}
