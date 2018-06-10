#include "util.h"


String Util::find(String req, String param) {
  //  String req=String(reqB);
  int start = req.indexOf(param + "=");
  start = req.indexOf("=", start);
  if (start < 0) return "";
  start++;
  int jj = req.indexOf("&", start);
  int kk = req.indexOf(" ", start);
  int end = jj;

  if ( jj < 0  ||  (kk > 0 && kk < jj) ) end = kk;

  String val = req.substring(start, end);

  return val;
}

