#ifndef _MINER_H_
#define _MINER_H_

#include <ac_int.h>
#include <ac_channel.h>
#include <mc_scverify.h>

/* Data Type */
typedef ac_int<1, false>   u1_t;
typedef ac_int<4, false>   u4_t;
typedef ac_int<8, false>   u8_t;
typedef ac_int<32, false>  u32_t;
typedef ac_int<64, false>  u64_t;
typedef ac_int<96, false>  u96_t;
typedef ac_int<256, false> u256_t;
typedef ac_int<512, false> u512_t;
typedef ac_int<640, false> header_t;

#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

#define HASH_SIZE 512 // size of one hash
#define HEADER_SIZE 80 // size of a header (without padding)
#define PAD_START 463 // 512 - 128 - 1

/* Functions */
u256_t sha256(header_t header);

void sha256_update(u512_t data, u32_t hash[8]);

u256_t get_output_hash(u32_t hash[8]);

#endif
