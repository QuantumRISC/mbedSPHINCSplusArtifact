#include <tis.h>
#include <string.h>
volatile TIS g_TIS;

void XMEMSET(void *dst, int v, int len){
  memset(dst,v,len);
}

void *XMEMCPY(void *dst, void*src, size_t len){
  return memcpy(dst,src,len);
}
// initialize the TIS state to sane start values
void TIS_init(uint8_t *fifo) {
  g_TIS.fifo = fifo;
  g_TIS.isRead = 0;
  g_TIS.nbytes = 0;
  g_TIS.address = 0;
  g_TIS.addrbytes = 0;
  g_TIS.transfered_bytes = 0;
  g_TIS.fifo_rsp_max = 0;
  memset(g_TIS.sts,0xff,sizeof(g_TIS.sts));
  //uint32_t sts = TIS_STS_U32;
  g_TIS.sts[0] = TIS_STS_VALID;
  g_TIS.io_stream_pos = 0;
  g_TIS.io_stream_fill = 0;
  ///TIS_STS_SET_U32(sts);
}

// helper function for the interrupt driven communication routine
static void read_write_spi(volatile uint8_t *data, uint8_t byte) {
  spi_write(SPI1,data[0]);
  if (g_TIS.isRead == 0) {
    *data = byte;
  }
}

// this is the interrupt handler for TIS on the stm32f4 
void spi1_isr() {
  uint8_t byte = spi_read(SPI1);
  if (g_TIS.nbytes > 0) {
    if (g_TIS.addrbytes == 3) {
      switch((g_TIS.address)) {
        case ACCESS_ADDRESS:  
          read_write_spi(&(g_TIS.access[g_TIS.transfered_bytes]), byte);
          break;
        case INT_CAP_ADDRESS:
          read_write_spi(&(g_TIS.int_cap[g_TIS.transfered_bytes]), byte);
          break;
        case STS_ADDRESS:  
          read_write_spi(&(g_TIS.sts[g_TIS.transfered_bytes]), byte);
          break;
        case FIFO_ADDRESS:
          read_write_spi(&(g_TIS.fifo[g_TIS.fifo_fill]), byte);
          g_TIS.fifo_fill ++;
          if ((g_TIS.fifo_fill >= g_TIS.fifo_rsp_max) && (g_TIS.fifo_rsp_max > 0)) 
            g_TIS.sts[0] &= ~(TIS_STS_DATA_AVAILABLE);
          break;
        case IOSTREAM_SIZE_ADDRESS: // this is read only
          spi_write(SPI1,(g_TIS.io_stream_fill >> (g_TIS.transfered_bytes*8))&0xff);
          break;
        case 0x0050 ... 0x0053:
          read_write_spi(&(g_TIS.io_stream[g_TIS.io_stream_pos]), byte);
          g_TIS.io_stream_pos ++;
          break;
        default:
          spi_write(SPI1,0x23);
      }
      g_TIS.nbytes --;
      g_TIS.transfered_bytes ++;
      if (g_TIS.nbytes == 0) {
        g_TIS.lastaddress = (g_TIS.lastaddress << 8) | g_TIS.address;
        g_TIS.addrbytes = 0;
        g_TIS.transfered_bytes = 0;
      }
    } else {
      g_TIS.address = (g_TIS.address<<8)|byte;
      g_TIS.addrbytes++;
    }
  } else {
    // read/write and length
    g_TIS.nbytes = (byte & 0x3f);
    g_TIS.isRead = (byte >> 7)&1;
    g_TIS.address = 0;
    g_TIS.addrbytes = 0;
    spi_write(SPI1,0x86);
  }
}
