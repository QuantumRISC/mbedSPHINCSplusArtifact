#include <string.h>

#include "utils.h"
#include "params.h"
#include "hash.h"
#include "thash.h"
#include "address.h"

#ifdef TARGET_UNIX
# include "stdlib.h"
#else
# include "hal.h"
#endif

/**
 * Converts the value of 'in' to 'outlen' bytes in big-endian byte order.
 */
void ull_to_bytes(unsigned char *out, unsigned int outlen,
                  unsigned long long in)
{
    int i;

    /* Iterate over out in decreasing order, for big-endianness. */
    for (i = outlen - 1; i >= 0; i--) {
        out[i] = in & 0xff;
        in = in >> 8;
    }
}

void u32_to_bytes(unsigned char *out, uint32_t in)
{
    out[0] = (unsigned char)(in >> 24);
    out[1] = (unsigned char)(in >> 16);
    out[2] = (unsigned char)(in >> 8);
    out[3] = (unsigned char)in;
}

void i32_to_bytes(unsigned char *out, int32_t in)
{
  out[0] = (in >> 24) & 0xFF;
  out[1] = (in >> 16) & 0xFF;
  out[2] = (in >> 8) & 0xFF;
  out[3] = in & 0xFF;
}

/**
 * Converts the inlen bytes in 'in' from big-endian byte order to an integer.
 */
unsigned long long bytes_to_ull(const unsigned char *in, unsigned int inlen)
{
    unsigned long long retval = 0;
    unsigned int i;

    for (i = 0; i < inlen; i++) {
        retval |= ((unsigned long long)in[i]) << (8*(inlen - 1 - i));
    }
    return retval;
}

/**
 * Computes a root node given a leaf and an auth path.
 * Expects address to be complete other than the tree_height and tree_index.
 */
void compute_root(unsigned char *root, const unsigned char *leaf,
                  uint32_t leaf_idx, uint32_t idx_offset,
                  const unsigned char *auth_path, uint32_t tree_height,
                  const unsigned char *pub_seed, uint32_t addr[8])
{
    uint32_t i;
    unsigned char buffer[2 * SPX_N];

    /* If leaf_idx is odd (last bit = 1), current path element is a right child
       and auth_path has to go left. Otherwise it is the other way around. */
    if (leaf_idx & 1) {
        memcpy(buffer + SPX_N, leaf, SPX_N);
        memcpy(buffer, auth_path, SPX_N);
    }
    else {
        memcpy(buffer, leaf, SPX_N);
        memcpy(buffer + SPX_N, auth_path, SPX_N);
    }
    auth_path += SPX_N;

    for (i = 0; i < tree_height - 1; i++) {
        leaf_idx >>= 1;
        idx_offset >>= 1;
        /* Set the address of the node we're creating. */
        set_tree_height(addr, i + 1);
        set_tree_index(addr, leaf_idx + idx_offset);

        /* Pick the right or left neighbor, depending on parity of the node. */
        if (leaf_idx & 1) {
            thash(buffer + SPX_N, buffer, 2, pub_seed, addr);
            memcpy(buffer, auth_path, SPX_N);
        }
        else {
            thash(buffer, buffer, 2, pub_seed, addr);
            memcpy(buffer + SPX_N, auth_path, SPX_N);
        }
        auth_path += SPX_N;
    }

    /* The last iteration is exceptional; we do not copy an auth_path node. */
    leaf_idx >>= 1;
    idx_offset >>= 1;
    set_tree_height(addr, tree_height);
    set_tree_index(addr, leaf_idx + idx_offset);
    thash(root, buffer, 2, pub_seed, addr);

}


void compute_root_streaming(streaming_ctx_t *stream_ctx, unsigned char *root, const unsigned char *leaf,
                            uint32_t leaf_idx, uint32_t idx_offset, uint32_t tree_height,
                            const unsigned char *pub_seed, uint32_t addr[8])
{
  uint32_t i;
  unsigned char buffer[2 * SPX_N];
  unsigned char auth_path[SPX_N]; // current element of the streamed auth path
  read_stream(stream_ctx, auth_path, SPX_N);

  /* If leaf_idx is odd (last bit = 1), current path element is a right child
     and auth_path has to go left. Otherwise it is the other way around. */
  if (leaf_idx & 1) {
    memcpy(buffer + SPX_N, leaf, SPX_N);
    memcpy(buffer, auth_path, SPX_N);
  }
  else {
    memcpy(buffer, leaf, SPX_N);
    memcpy(buffer + SPX_N, auth_path, SPX_N);
  }
//  auth_path += SPX_N;


  for (i = 0; i < tree_height - 1; i++) {
    read_stream(stream_ctx, auth_path, SPX_N);
    leaf_idx >>= 1;
    idx_offset >>= 1;
    /* Set the address of the node we're creating. */
    set_tree_height(addr, i + 1);
    set_tree_index(addr, leaf_idx + idx_offset);

    /* Pick the right or left neighbor, depending on parity of the node. */
    if (leaf_idx & 1) {
      thash(buffer + SPX_N, buffer, 2, pub_seed, addr);
      memcpy(buffer, auth_path, SPX_N);
    }
    else {
      thash(buffer, buffer, 2, pub_seed, addr);
      memcpy(buffer + SPX_N, auth_path, SPX_N);
    }
//    auth_path += SPX_N;
  }

  /* The last iteration is exceptional; we do not copy an auth_path node. */
  leaf_idx >>= 1;
  idx_offset >>= 1;
  set_tree_height(addr, tree_height);
  set_tree_index(addr, leaf_idx + idx_offset);
  thash(root, buffer, 2, pub_seed, addr);

}

/**
 * For a given leaf index, computes the authentication path and the resulting
 * root node using Merkle's TreeHash algorithm.
 * Expects the layer and tree parts of the tree_addr to be set, as well as the
 * tree type (i.e. SPX_ADDR_TYPE_HASHTREE or SPX_ADDR_TYPE_FORSTREE).
 * Applies the offset idx_offset to indices before building addresses, so that
 * it is possible to continue counting indices across trees.
 */
void treehash(unsigned char *root, unsigned char *auth_path,
              const unsigned char *sk_seed, const unsigned char *pub_seed,
              uint32_t leaf_idx, uint32_t idx_offset, uint32_t tree_height,
              void (*gen_leaf)(
                 unsigned char* /* leaf */,
                 const unsigned char* /* sk_seed */,
                 const unsigned char* /* pub_seed */,
                 uint32_t /* addr_idx */, const uint32_t[8] /* tree_addr */),
              uint32_t tree_addr[8])
{
    unsigned char stack[(tree_height + 1)*SPX_N];
    unsigned int heights[tree_height + 1];
    unsigned int offset = 0;
    uint32_t idx;
    uint32_t tree_idx;

    for (idx = 0; idx < (uint32_t)(1 << tree_height); idx++) {
        /* Add the next leaf node to the stack. */
        gen_leaf(stack + offset*SPX_N,
                 sk_seed, pub_seed, idx + idx_offset, tree_addr);
      offset++;
        heights[offset - 1] = 0;

        /* If this is a node we need for the auth path.. */
        if ((leaf_idx ^ 0x1) == idx) {
            memcpy(auth_path, stack + (offset - 1)*SPX_N, SPX_N);
        }

        /* While the top-most nodes are of equal height.. */
        while (offset >= 2 && heights[offset - 1] == heights[offset - 2]) {
            /* Compute index of the new node, in the next layer. */
            tree_idx = (idx >> (heights[offset - 1] + 1));

            /* Set the address of the node we're creating. */
            set_tree_height(tree_addr, heights[offset - 1] + 1);
            set_tree_index(tree_addr,
                           tree_idx + (idx_offset >> (heights[offset-1] + 1)));
            /* Hash the top-most nodes from the stack together. */

            thash(stack + (offset - 2)*SPX_N,
                         stack + (offset - 2)*SPX_N, 2, pub_seed, tree_addr);

            offset--;
            /* Note that the top-most node is now one layer higher. */
            heights[offset - 1]++;

            /* If this is a node we need for the auth path.. */
            if (((leaf_idx >> heights[offset - 1]) ^ 0x1) == tree_idx) {
                memcpy(auth_path + heights[offset - 1]*SPX_N,
                       stack + (offset - 1)*SPX_N, SPX_N);
            }
        }
    }
    memcpy(root, stack, SPX_N);
}


#ifdef TARGET_UNIX
extern char STREAM_SIG_OUT_FILE[255];
#endif

/*
 * streaming functions
 */
#include "api.h"
void init_streaming_ctx(streaming_ctx_t *ctx, stream_direction_flag_t direction_flag, stream_mode_t stream_mode, unsigned char *OPTIONAL_sig_buf)
{

#ifdef TARGET_UNIX
  if(direction_flag == WRITE_SIG_FLAG)
  {
    ctx->fp = fopen(STREAM_SIG_OUT_FILE, "wb");
  }
  else
  {
    ctx->fp = fopen(STREAM_SIG_OUT_FILE, "rb");
  }
#else
  (void)direction_flag;
#endif

  ctx->stream_mode = stream_mode;
  if(stream_mode == STREAM_LOCAL_BUFFER)
  {
    ctx->sig = OPTIONAL_sig_buf;
    ctx->cur_sig_len = 0;
  }
}



void destroy_streaming_ctx(streaming_ctx_t *ctx)
{
#ifdef TARGET_UNIX
  fclose(ctx->fp);
#else
  (void)ctx;
#endif
}

/*
 * Send/Write Protocol:
 *  - Send ANY char to signal that you want to write SPX_N (32) bytes
 *  - Currently, everything is a multiple of SPX_N (or chosen as SPX_N like msg length)
 *  - Can be replaced with a more intricate protocol but serves the purpose for now:
 *    - do not mindlessly flood data to the other party
 */
#ifndef OVERRIDE_STREAM_READWRITE
void write_stream(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len)
{
  if (!ctx) return;
  if(ctx->stream_mode == STREAM_SERIAL)
  {
#ifdef TARGET_UNIX
    fwrite(data, sizeof(unsigned char), data_len, ctx->fp);
#endif
#ifdef TARGET_STM32F4
    for(unsigned i = 0; i < data_len; i++)
    {
      if(!(i % SPX_N))
      {
        hal_getchar();
      }
      hal_putchar(data[i]);
    }
#endif
  }
  else if(ctx->stream_mode == STREAM_LOCAL_BUFFER)
  {
    memcpy(ctx->sig + ctx->cur_sig_len, data, data_len);
    ctx->cur_sig_len += data_len;
  }
}

void read_stream(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len)
{
  if(ctx->stream_mode == STREAM_SERIAL)
  {
#ifdef TARGET_UNIX
    size_t read = fread(data, sizeof(unsigned char), data_len, ctx->fp);
    if(read != data_len)
    {
      printf("ERROR: Unexpected read size from file (read_stream)\n");
      exit(-1);
    }
#endif
#ifdef TARGET_STM32F4
    for(unsigned i = 0; i < data_len; i++)
    {
      if(!(i % SPX_N))
      {
        hal_putchar(0);
      }
      data[i] = hal_getchar();
    }
#endif
  }
  else if(ctx->stream_mode == STREAM_LOCAL_BUFFER)
  {
    memcpy(data, ctx->sig + ctx->cur_sig_len, data_len);
    ctx->cur_sig_len += data_len;
  }
}
#endif
