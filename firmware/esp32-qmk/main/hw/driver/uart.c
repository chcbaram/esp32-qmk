#include "uart.h"



#ifdef _USE_HW_UART
#include "driver/uart.h"
#ifdef _USE_HW_CDC
#include "cdc.h"
#endif



#define UART_RX_Q_BUF_LEN       512



typedef struct
{
  bool is_open;

  uart_port_t   port;
  uint32_t      baud;
  uart_config_t config;
  QueueHandle_t queue;

  uint8_t       wr_buf[UART_RX_Q_BUF_LEN];
} uart_tbl_t;


static uart_tbl_t uart_tbl[UART_MAX_CH];


bool uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open = false;
  }

  return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false;


  if (uartIsOpen(ch) == true && uartGetBaud(ch) == baud)
  {
    return true;
  }

  switch(ch)
  {
    case _DEF_UART1:
      uart_tbl[ch].port = UART_NUM_0;
      uart_tbl[ch].baud = baud;
      uart_tbl[ch].config.baud_rate = baud;
      uart_tbl[ch].config.data_bits = UART_DATA_8_BITS;
      uart_tbl[ch].config.parity    = UART_PARITY_DISABLE;
      uart_tbl[ch].config.stop_bits = UART_STOP_BITS_1;
      uart_tbl[ch].config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
      uart_tbl[ch].config.source_clk = UART_SCLK_APB;


      uart_driver_install(UART_NUM_0, UART_RX_Q_BUF_LEN*2, UART_RX_Q_BUF_LEN*2, 0, NULL, 0);
      uart_param_config(UART_NUM_0, &uart_tbl[ch].config);

      //Set UART pins (using UART0 default pins ie no changes.)
      uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);


      uart_tbl[ch].is_open = true;
      ret = true;
      break;

    case _DEF_UART2:
      uart_tbl[ch].is_open = true;
      uart_tbl[ch].baud = baud;
      ret = true;
      break;      
  }

  return ret;
}

bool uartIsOpen(uint8_t ch)
{
  return uart_tbl[ch].is_open;
}

bool uartClose(uint8_t ch)
{
  uart_tbl[ch].is_open = false;
  return true;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;
  size_t len;


  if (uart_tbl[ch].is_open != true) return 0;

  switch(ch)
  {
     case _DEF_UART1:    
      uart_get_buffered_data_len(uart_tbl[ch].port, &len);
      ret = len;
      break;

    case _DEF_UART2:
      #ifdef _USE_HW_CDC
      ret = cdcAvailable();
      #endif
      break;
  }

  return ret;
}

bool uartFlush(uint8_t ch)
{
  uint32_t pre_time;

  pre_time = millis();
  while(uartAvailable(ch))
  {
    if (millis()-pre_time >= 10)
    {
      break;
    }
    uartRead(ch);
  }

  switch(ch)
  {
     case _DEF_UART1:    
      uart_flush(uart_tbl[ch].port);
      break;
  }

  return true;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:     
      uart_read_bytes(uart_tbl[ch].port, &ret, 1, 10);
      break;
      
    case _DEF_UART2:
      #ifdef _USE_HW_CDC
      ret = cdcRead();
      #endif
      break;
  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;

  if (uart_tbl[ch].is_open != true) return 0;


  switch(ch)
  {
    case _DEF_UART1:          
      ret = uart_write_bytes(uart_tbl[ch].port, (const char*)p_data, (size_t)length);
      break;
      
    case _DEF_UART2:
      #ifdef _USE_HW_CDC 
      ret = cdcWrite(p_data, length);
      #endif
      break;
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, const char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  return uart_tbl[ch].baud;
}


#endif
