#ifndef SPHINCSAPI_H
#include <api.h>
#include <api.h>
#include <thash.h>
#include <rng.h>
extern uint64_t write_kcycles;
extern uint64_t read_kcycles;
extern spx_param_set_e PARAMETER_SET_LIST[];
extern uint8_t transfer_buffer[1024];
extern uint32_t transfer_buffer_fill;
extern uint32_t transfer_bytes_remaining;
#endif
