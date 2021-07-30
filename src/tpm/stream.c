#include <utils.h>
#ifdef OVERRIDE_STREAM_READWRITE
#include <tis.h>
#include <string.h>
#include <hal.h>
uint64_t read_kcycles = 0;
uint64_t write_kcycles = 0;
uint8_t transfer_buffer[1024];
uint32_t transfer_buffer_fill = 0;
uint32_t transfer_bytes_remaining = 0;
#define MIN(a,b) (((a)<(b))?(a):(b))
// take out data from the buffer or call read_stream_transfer if not enough data is available
void read_stream(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len) {
  (void)ctx;
  if (data_len > transfer_buffer_fill) {
    memcpy(data,transfer_buffer,transfer_buffer_fill);
    data_len -= transfer_buffer_fill;
    data     += transfer_buffer_fill;
    transfer_bytes_remaining -= transfer_buffer_fill;
    transfer_buffer_fill = 0;
    read_stream_transfer(NULL,transfer_buffer+transfer_buffer_fill,MIN(sizeof(transfer_buffer),transfer_bytes_remaining));
    //transfer_bytes_remaining -= MIN(sizeof(transfer_buffer),transfer_bytes_remaining);
    transfer_buffer_fill = MIN(sizeof(transfer_buffer),transfer_bytes_remaining);
  }
  // take data from beginning of transfer_buffer
  memcpy(data,transfer_buffer,data_len);
  transfer_buffer_fill -= data_len;
  transfer_bytes_remaining -= data_len;
  // memmove transfer_buffer
  memmove(transfer_buffer,transfer_buffer+data_len,transfer_buffer_fill);
}
// this function fills the buffer with data from the host-system
void read_stream_transfer(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len)
{
  uint32_t start = hal_get_time();
  //printf("reached %s:%d\n",__FILE__,__LINE__);
  (void)ctx;
  // ready the stream
  memset((void *)g_TIS.io_stream,0x00,sizeof(g_TIS.io_stream));
  g_TIS.io_stream_fill = data_len;
  g_TIS.io_stream_pos = 0;

  g_TIS.sts[3] |= TIS_STS_IO_STREAM_WANT_READ;
  while(g_TIS.io_stream_fill > g_TIS.io_stream_pos) {
    asm volatile ("nop");
  }
  g_TIS.sts[3] = 0;
  g_TIS.io_stream_fill = 0;
  g_TIS.io_stream_pos = 0;
  memcpy(data,(void *)g_TIS.io_stream,data_len);
  uint32_t stop = hal_get_time();
  read_kcycles += stop-start;
//  phex("streamin",data,data_len);
}
// this function is used to flush previous writes once the buffer is full enough
void write_stream_transfer(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len)
{
  //printf("write_stream_transfer(%p,%p,%d);\n",ctx,data,data_len);
  uint32_t start = hal_get_time();
 // phex("streamout",data,data_len);
  //fflush(stdout);
  (void)ctx;
  // same interface as with read
  memset((void *)g_TIS.io_stream,0x00,sizeof(g_TIS.io_stream));
  memcpy((void *)g_TIS.io_stream,data,data_len);
  g_TIS.io_stream_fill = data_len;
  g_TIS.io_stream_pos = 0;

  g_TIS.sts[3] |= TIS_STS_IO_STREAM_WANT_WRITE;
  while(g_TIS.io_stream_fill > g_TIS.io_stream_pos) {
    g_TIS.sts[3] |= TIS_STS_IO_STREAM_WANT_WRITE;
      asm volatile("nop");
      asm volatile ("nop");
      asm volatile ("nop");
      asm volatile ("nop");
  };
  g_TIS.sts[3] = 0;
  g_TIS.io_stream_fill = 0;
  g_TIS.io_stream_pos = 0;
  uint32_t stop = hal_get_time();
  write_kcycles += stop-start;
}
// fill buffer with data to be send to the host system
void write_stream(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len) {
  (void)ctx;
  if ((transfer_buffer_fill+data_len) > sizeof(transfer_buffer)) {
    write_stream_transfer(NULL, transfer_buffer, transfer_buffer_fill);
    transfer_buffer_fill = 0;
  }
  memcpy(transfer_buffer+transfer_buffer_fill,data,data_len);
  transfer_buffer_fill += data_len;
}
#endif
