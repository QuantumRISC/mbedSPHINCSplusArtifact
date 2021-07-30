#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include "Tpm.h"
#include "BaseTypes.h"
#include "TpmTypes.h"
#include "CreateLoaded_fp.h"
#include "CreatePrimary_fp.h"
#include "Create_fp.h"
#include "ExecCommand_fp.h"
#include "Marshal_fp.h"
#include "PQC_Private_fp.h"
#include "PQC_Public_fp.h"
#include "Startup_fp.h"


#include "_TPM_Init_fp.h"
#include <fcntl.h>
#include <getopt.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

uint8_t buf[20480] = {0};
uint8_t stream_buf[1024 * 100] = {0};
uint32_t stream_fill;
uint64_t write_time = 0;
uint64_t read_time = 0;
int g_scheme = 0;


static void pabort(const char *s) {
  perror(s);
  abort();
}

uint64_t stopwatch() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (((uint64_t) tv.tv_sec) * 1000000)+tv.tv_usec;
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 50000;
static uint16_t delay = 5000;

static uint64_t gen_time = 0;
static uint64_t sign_time = 0;
static uint64_t verify_time = 0;
static uint64_t comm_time_tmp = 0;
static uint64_t gen_comm_time = 0;
static uint64_t sign_comm_time = 0;
static uint64_t verify_comm_time = 0;

static void transfer(int fd, uint8_t *tx, uint8_t *rx, uint32_t size) {
  int ret;
  //usleep(1000);
  struct spi_ioc_transfer tr = {
      .tx_buf = (long long unsigned)tx,
      .rx_buf = (long long unsigned)rx,
      .len = size,
      .delay_usecs = delay,
      .speed_hz = speed,
      .bits_per_word = bits,
  };
  //  phex("\ntx",tx,size);
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1)
    pabort("can't send spi message");
  // phex("rx",rx,size);
}

static void print_usage(const char *prog) {
  printf("Usage: %s [-DsbdlHOLC3]\n", prog);
  puts("  -D --device   device to use (default /dev/spidev1.1)\n"
       "  -s --speed    max speed (Hz)\n"
       "  -d --delay    delay (usec)\n"
       "  -b --bpw      bits per word \n"
       "  -l --loop     loopback\n"
       "  -H --cpha     clock phase\n"
       "  -O --cpol     clock polarity\n"
       "  -L --lsb      least significant bit first\n"
       "  -C --cs-high  chip select active high\n"
       "  -3 --3wire    SI/SO signals shared\n"
       "  -S --sphics-scheme <id>\n"
       "            0 - sphincs_sha256_128f_robust\n"
       "            1 - sphincs_sha256_128s_robust\n"
       "            2 - sphincs_sha256_192f_robust\n"
       "            3 - sphincs_sha256_192s_robust\n"
       "            4 - sphincs_sha256_256f_robust\n"
       "            5 - sphincs_sha256_256s_robust\n"
       "            6 - sphincs_sha256_128f_simple\n"
       "            7 - sphincs_sha256_128s_simple\n"
       "            8 - sphincs_sha256_192f_simple\n"
       "            9 - sphincs_sha256_192s_simple\n"
       "           10 - sphincs_sha256_256f_simple\n"
       "           11 - sphincs_sha256_256s_simple\n"
       "           12 - sphincs_shake256_128f_robust\n"
       "           13 - sphincs_shake256_128s_robust\n"
       "           14 - sphincs_shake256_192f_robust\n"
       "           15 - sphincs_shake256_192s_robust\n"
       "           16 - sphincs_shake256_256f_robust\n"
       "           17 - sphincs_shake256_256s_robust\n"
       "           18 - sphincs_shake256_128f_simple\n"
       "           19 - sphincs_shake256_128s_simple\n"
       "           20 - sphincs_shake256_192f_simple\n"
       "           21 - sphincs_shake256_192s_simple\n"
       "           22 - sphincs_shake256_256f_simple\n"
       "           23 - sphincs_shake256_256s_simple\n"
       );
  exit(1);
}

static void parse_opts(int argc, char *argv[]) {
  while (1) {
    static const struct option lopts[] = {
        {"device", 1, 0, 'D'}, {"speed", 1, 0, 's'}, {"delay", 1, 0, 'd'},
        {"bpw", 1, 0, 'b'},    {"loop", 0, 0, 'l'},  {"cpha", 0, 0, 'H'},
        {"cpol", 0, 0, 'O'},   {"lsb", 0, 0, 'L'},   {"cs-high", 0, 0, 'C'},
        {"3wire", 0, 0, '3'},  {"no-cs", 0, 0, 'N'}, {"ready", 0, 0, 'R'},
        {"sphincs-scheme",1,0,'S'},
        {NULL, 0, 0, 0},
    };
    int c;

    c = getopt_long(argc, argv, "D:s:d:b:S:lHOLC3NR", lopts, NULL);

    if (c == -1)
      break;

    switch (c) {
    case 'D':
      device = optarg;
      break;
    case 's':
      speed = atoi(optarg);
      break;
    case 'd':
      delay = atoi(optarg);
      break;
    case 'b':
      bits = atoi(optarg);
      break;
    case 'l':
      mode |= SPI_LOOP;
      break;
    case 'H':
      mode |= SPI_CPHA;
      break;
    case 'O':
      mode |= SPI_CPOL;
      break;
    case 'L':
      mode |= SPI_LSB_FIRST;
      break;
    case 'C':
      mode |= SPI_CS_HIGH;
      break;
    case '3':
      mode |= SPI_3WIRE;
      break;
    case 'N':
      mode |= SPI_NO_CS;
      break;
    case 'R':
      mode |= SPI_READY;
      break;
    case 'S':
      g_scheme = atoi(optarg);
      break;
    default:
      print_usage(argv[0]);
      break;
    }
  }
}

#define FIFO_ADDRESS 0x0024
#define STS_ADDRESS 0x0018
#define STREAM_ADDRESS 0x0050
#define TPM_MAX_SLICE 32

static void tis_read_write(int fd, int read, uint32_t address, uint8_t *data,
                           uint8_t len) {
  uint8_t window[0x100];
  uint8_t windowrx[0x100];
  memset(window, 0x00, len + 5);
  memset(windowrx, 0x00, len + 5);
  window[0] = ((read > 0) << 7) | (len & 0x3f);
  window[1] = (address >> 16) & 0xff;
  window[2] = (address >> 8) & 0xff;
  window[3] = (address)&0xff;

  memcpy(window + 4, data, len);
  if (read > 0) {
    len++;
  }
  transfer(fd, window, windowrx, len + 4);
  if (read > 0) {
    memcpy(data, windowrx + 5, len);
    // phex("read",data,len);
  }
}

void tis_write_to_fifo(int fd, uint8_t *data, int len) {
  while (len > 0) {
    printf(".");
    int writeBytes = (len > TPM_MAX_SLICE) ? TPM_MAX_SLICE : len;
    tis_read_write(fd, 0, FIFO_ADDRESS, data, writeBytes);
    len -= writeBytes;
    data += writeBytes;
  }
}

void tis_set_go(int fd) {
  uint8_t sts[10] = {0};
  sts[0] = (1 << 5) | (1 << 7);
  tis_read_write(fd, 0, STS_ADDRESS, sts, 4);
}

uint32_t tis_get_sts(int fd) {
  uint8_t data[10] = {0};
  uint32_t sts = 0;
  tis_read_write(fd, 1, STS_ADDRESS, data, 4);
  sts = (sts << 8) | data[3];
  sts = (sts << 8) | data[2];
  sts = (sts << 8) | data[1];
  sts = (sts << 8) | data[0];
  return sts;
}

uint32_t tis_get_stream_size(int fd) {
  uint8_t data[10] = {0};
  uint32_t size = 0;
  tis_read_write(fd, 1, 0x0040, data, 4);
  size = (size << 8) | data[3];
  size = (size << 8) | data[2];
  size = (size << 8) | data[1];
  size = (size << 8) | data[0];
  return size;
}

#define b2u32(x) (((x)[0] << 24) | ((x)[1] << 16) | ((x)[2] << 8) | ((x)[3]))
int tis_receive_response(int fd, uint8_t *data, int *size, int max_size) {
  tis_read_write(fd, 1, FIFO_ADDRESS, data, 6);
  uint8_t *x = data + 2;
  printf("\n data %02x%02x%02x%02x\n", x[0], x[1], x[2], x[3]);
  *size = b2u32(x);
  printf("rsp size is %d\n", *size);
  if (*size > 10000 || *size < 0);
    phex("rsp",x,6);
  int to_read = (*size) - 6;
  uint8_t *target = data + 6;
  while (to_read > 32) {
    tis_read_write(fd, 1, FIFO_ADDRESS, target, 32);
    target += 32;
    to_read -= 32;
  }
  tis_read_write(fd, 1, FIFO_ADDRESS, target, to_read);
  return 0;
}

// in fd
// out stream
// returns size of stream
int32_t read_stream(int fd, uint8_t *pstream) {
  uint64_t start = stopwatch();
  uint8_t *save = pstream;
  // volatile uint32_t sts = 0;
  uint32_t stream_size = tis_get_stream_size(fd);
  // printf("[=] stream fifo fill is %d\n",stream_size);
  // sts = tis_get_sts(fd);
  int to_read = stream_size;
  while (to_read > 32) {
    int bytes = to_read;
    if (bytes > 32)
      bytes = 32;
    tis_read_write(fd, 1, STREAM_ADDRESS, pstream, bytes);
    to_read -= bytes;
    pstream += bytes;
  }
  tis_read_write(fd, 1, STREAM_ADDRESS, pstream, to_read);
  // phex("stream data",save,stream_size);
  read_time += (stopwatch() - start);
  return stream_size;
}

void write_stream(int fd, uint8_t *buf, int fill) {
  // printf("entered write_stream(%d)\n",fill);

  uint32_t pos = 0;
  uint64_t start;
  while (fill > pos) {
    start = stopwatch();
    int want_read_bytes = tis_get_stream_size(fd);
    write_time += (stopwatch()-start);
    while (want_read_bytes) {
      start = stopwatch();
      int bytes = want_read_bytes;
      if (bytes > 48)
        bytes = 48;
      tis_read_write(fd, 0, STREAM_ADDRESS, &(buf[pos]), bytes);
      //   phex("writing out",&(buf[pos]),bytes);
      want_read_bytes -= bytes;
      pos += bytes;
      write_time += (stopwatch()-start);
    }
  }
}

// in: fd
// in/out: data,size
int tis_execute_command(int fd, uint8_t *data, int *size, int max_size) {
  uint8_t *stream = stream_buf;
  tis_write_to_fifo(fd, data, *size);
  printf("\n[+] Command Transmitted\n");
  tis_set_go(fd);
  printf("\n[+] TPM GO set\n");
  memset(data, 0, max_size); // zero out data for debug purposes
  volatile uint32_t sts = 0;
  do {
    //usleep(1000);
    sts = tis_get_sts(fd);
    //usleep(1000);
    int stream_info = (sts >> 24);
    // printf("%d\n",stream_info);
    if (stream_info == 2) { // want write
      int32_t streampos = read_stream(fd, stream);
      // phex("stream",stream,streampos);
      stream += streampos;
      stream_fill += streampos;
    }
    if (stream_info == 1) { // want read
      write_stream(fd, stream_buf,stream_fill); // this will write the entire stream but in portions
                           // demanded by the TPM
    }
  } while (((sts >> 4) & 1) == 0);
  printf("\n[+] Data Available\n");
  return tis_receive_response(fd, data, size, max_size);
}

int init_spi() {}

#define TPM_TIS_CLIENT_TEST
#ifdef TPM_TIS_CLIENT_TEST
int main(int argc, char *argv[]) {
  int ret = 0;
  int fd;

  parse_opts(argc, argv);

  fd = open(device, O_RDWR);
  if (fd < 0)
    pabort("can't open device");

  /*
   * spi mode
   */
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
    pabort("can't set spi mode");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
    pabort("can't get spi mode");
  /*
   * bits per word
   */
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't get bits per word");

  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't set max speed hz");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't get max speed hz");

  printf("spi mode: %d\n", mode);
  printf("bits per word: %d\n", bits);
  printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);

  uint32_t requestSize = sizeof(buf);
  uint32_t responseSize = sizeof(buf);
  uint8_t *response = buf;
  uint8_t *request = buf;
  uint32_t parent_handle = 0x80000000;
  g_forceFailureMode = 0;

  Create_In create_param = {                                                                                                                                                                                                                    
      .parentHandle = parent_handle,                                                                                                                                                                                                                           
      .inSensitive =                                                                                                                                                                                            
          {                                                                                                                                                                 
              .size = 4,                                                                                                                                                                                                        
              .sensitive = {                                                                                                                                                                                                                    
                .userAuth = {{                                                                                                                                                                                                                          
                    .size = 0,                                                                                                                                                                                                                          
                    .buffer = {0}                                                                                                                                                                                                                              
                }},                                                                                                                                                                                         
                .data =  {{                                                                                                                                                                                                                 
                    .size = 0,                                                                                                                                                                                                                          
                    .buffer = {},                                                                                                                                                                                                                              
                }}                                                                                                                                                                                      
              }                                                                                                                                                                             
          },                                                                                                                                                                    
      .inPublic = {                                                                                                                                                                                         
                   .size = 0,                                                                                                                                                                                                                       
                   .publicArea = {.type = 0x45,                                                                                                                                                                                                                
                                  .nameAlg = TPM_ALG_SHA256,                                                                                                                                                                                                   
                                  .objectAttributes =                                  
                                      (1 << 1) | TPMA_OBJECT_userWithAuth |            
                                      TPMA_OBJECT_fixedParent |                        
                                      TPMA_OBJECT_sensitiveDataOrigin |                
                                      TPMA_OBJECT_decrypt,                             
                                  .authPolicy = {{                                     
                                      .size = 0,                                       
                                  }},
                                  .parameters.pqcDetail = g_scheme,
                                                                // the scheme                                                                                                                                                                                  
                                  .unique.pqc.t = {.size = 4, .buffer = {0}}}},                                                                                                                                                                                
      .outsideInfo.t = {.size = 0, .buffer = {0}},                                                                                                                                                                                                             
      .creationPCR = {                                                                                                                                                                                                  
          .count = 0,                                                                                                                                                                                               
      }};

  TPMS_AUTH_COMMAND authCmd1 = {.sessionHandle = TPM_RS_PW,
                                .nonce = {{.size = 0, .buffer = {}}},
                                .sessionAttributes =
                                    (0
                                     // TPMA_SESSION_CONTINUESESSION |,
                                     // TPMA_SESSION_AUDITRESET |,
                                     // TPMA_SESSION_AUDITRESET |
                                     // TPMA_SESSION_DECRYPT |
                                     // TPMA_SESSION_ENCRYPT |
                                     // TPMA_SESSION_AUDIT
                                     ),
                                .hmac = {{.size = 0, .buffer = {}}}};

  TPMS_AUTH_COMMAND *authCmdArray[] = {&authCmd1};

  CreateLoaded_In create_loaded_param;
  CreateLoaded_Out create_loaded_rsp;
  create_loaded_param.parentHandle = create_param.parentHandle;
  create_loaded_param.inSensitive = (TPM2B_SENSITIVE_CREATE){
      .size = 4,
      .sensitive = {.userAuth = {{.size = 0, .buffer = {0}}},
                    .data = {{
                        .size = 0,
                        .buffer = {},
                    }}}};
  //  create_loaded_param.inPublic = create_param.inPublic;
  int32_t template_size = 1024;
  uint8_t *tmp = create_loaded_param.inPublic.t.buffer;
  TPMT_PUBLIC_Marshal(&(create_param.inPublic.publicArea), &tmp,
                      &template_size);
  printf("template_size=%d\n", template_size);
  create_loaded_param.inPublic.t.size = 1024 - template_size;

  printf("sizeof(TPMU_SENSITIVE_CREATE) = %d\n", sizeof(TPMU_SENSITIVE_CREATE));
  requestSize = sizeof(buf);
  prepareTPM_CreateLoaded(&create_loaded_param, request, &requestSize,
                          &authCmd1);

  phex("TPM_CreateLoaded cmd bytes", request, requestSize);
  gen_time = stopwatch();
  tis_execute_command(fd, buf, &requestSize, sizeof(buf));
  gen_time = stopwatch() - gen_time;
  phex("TPM repsonse bytes", response, requestSize);

  printf("TPM-CreateLoaded-time is %5.3fs\n", gen_time/1.0e6);
  //*
  PQC_Private_In private_in = {
      .keyHandle = 0x80000001,
      .sig =
          {
              .scheme.pqc = 0,
              .data.t = {.buffer = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff},
                         .size = 32},
          },
  };

  requestSize = sizeof(buf);
  responseSize = sizeof(buf);
  request = buf;
  response = buf;
  preparePQC_PRIVATE(&private_in, request, &requestSize, &authCmd1);
  phex("PQC_Private cmd bytes", buf, requestSize);
  sign_time = stopwatch();
  tis_execute_command(fd, buf, &requestSize, sizeof(buf));
  sign_time = stopwatch() - sign_time;
  phex("PQC_Private rsp bytes", response, requestSize);
  printf("PQC-Private-time is %5.3f s\n", sign_time/1.0e6);
  // printf("[=]streamed data is %d bytes",stream_fill);
  // phex("",stream_buf,stream_fill);
  //*/
  //*
  PQC_Public_In in = {
      .keyHandle = 0x80000001,
      .sig =
          {
              .scheme.pqc = 0,
              .data.t = {.buffer = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff},
                         .size = 32},
          },
  };

  request = buf;
  response = buf;
  requestSize = sizeof(buf);
  responseSize = sizeof(buf);
  preparePQC_PUBLIC(&in, request, &requestSize);
  phex("PQC_Public cmd bytes", buf, requestSize);
  //  stream_fill = 49852;
  //  for (int i = 0; i < stream_fill; i+=4) {
  //    stream_buf[i+0] =  i>>24;
  //    stream_buf[i+1] =  i>>16;
  //    stream_buf[i+2] =  i>>8;
  //    stream_buf[i+3] =  i>>0;
  //  }
  //  phex("stream",stream_buf,stream_fill);
  //stream_buf[100] ^= 1;
  printf("stream_fill %d\n",stream_fill);
  verify_time = stopwatch();
  tis_execute_command(fd, buf, &requestSize, sizeof(buf));
  verify_time = stopwatch() - verify_time;
  printf("PQC-Public-time is %5.3fs\n", verify_time/1.0e6);
  phex("PQC_Public rsp bytes", response, requestSize);

  printf("write-time was %5.3fs\n",write_time/1.0e6);
  printf("read-time was %5.3fs\n", read_time/1.0e6);
  return 0;
}
#endif
