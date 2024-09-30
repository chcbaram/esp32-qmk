#include "ble.h"


#ifdef _USE_HW_BLE
#include "cli.h"


static bool is_init = false;


bool bleInit(void)
{
  bool ret;


  ret = bleHidInit();
  is_init = ret;

  logPrintf("[%s] bleInit()\n", is_init?"OK":"E_");   

  return ret;
}



#endif