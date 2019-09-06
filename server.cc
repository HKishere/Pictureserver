#include "httplib.h"

int main(){
  using namespace httplib;

  Server svr;
  
  svr.set_base_dir("./index");
  svr.listen("0.0.0.0",3389);

  return 0;
}
