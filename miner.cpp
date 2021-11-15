#include "miner.h"


void sha256_update(u512_t data, u32_t hash[8]) {
    static const u32_t k[64] = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
		0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
		0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
		0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
		0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
		0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};
    
    /* Temporary Variables*/
    u32_t a, b, c, d, e, f, g, h;

    /* W-series Temporary Variables */
    u32_t w[64];

    /* Copy Current Hash Value to Temporary Variables */
    a = hash[0], b = hash[1], c = hash[2], d = hash[3];
    e = hash[4], f = hash[5], g = hash[6], h = hash[7];

    /* Message expander */
    ME: for (u32_t i = 0; i < 64; i++) {
        if (i < 16) {
            w[i] = data.slc<32>(480 - i * 32);
        } else {
            w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
        }
    }

    /* Message compressor */
    MC: for (u32_t i = 0; i < 64; i++) {
        /* Temporal Summation */
        u32_t t1 = h + EP1(e) + CH(e, f, g) + k[i] + w[i];
        u32_t t2 = EP0(a) + MAJ(a, b, c);

        /* Swap Values*/
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    /* Update Hash Value with Temporary Variables */
    hash[0] += a, hash[1] += b, hash[2] += c, hash[3] += d;
    hash[4] += e, hash[5] += f, hash[6] += g, hash[7] += h;
}

u256_t get_output_hash(u32_t hash[8]) {
    u256_t final_hash;
    CONCAT: for (int i = 0; i < 8; i++) {
        final_hash.set_slc(i*32, hash[7 - i]);
    }
    return final_hash;
}

u32_t flip_nonce(u32_t nonce) {
    u32_t out_nonce;
    for (int i = 0; i < 4; i++) {
        out_nonce.set_slc(i*8, nonce.slc<8>((3-i)*8));
    }
    return out_nonce;
}

#pragma hls_design top
u256_t sha256(header_t header) {

    /* Initialize Hash Value */
    u32_t hash[8];
    hash[0] = 0x6a09e667, hash[1] = 0xbb67ae85;
    hash[2] = 0x3c6ef372, hash[3] = 0xa54ff53a;
    hash[4] = 0x510e527f, hash[5] = 0x9b05688c;
    hash[6] = 0x1f83d9ab, hash[7] = 0x5be0cd19;

    u32_t first_hash[8];
    first_hash[0] = 0x6a09e667, first_hash[1] = 0xbb67ae85;
    first_hash[2] = 0x3c6ef372, first_hash[3] = 0xa54ff53a;
    first_hash[4] = 0x510e527f, first_hash[5] = 0x9b05688c;
    first_hash[6] = 0x1f83d9ab, first_hash[7] = 0x5be0cd19;

    /* update for first chunk*/
    u512_t data = header.slc<512>(128);    // Data of One Chunk
    u32_t nonce = flip_nonce(header.slc<32>(0));
    const u96_t const_bits = header.slc<96>(32);
    sha256_update(data, first_hash);

    for (int i = 0; i < 100; i++) {
        // std::string nonce_string = nonce.to_string(AC_HEX, false, true);
		// nonce_string = nonce_string.substr(2);
        // std::cout << "nonce: " << nonce_string << std::endl;
        // reset the hash
        hash[0] = first_hash[0], hash[1] = first_hash[1];
        hash[2] = first_hash[2], hash[3] = first_hash[3];
        hash[4] = first_hash[4], hash[5] = first_hash[5];
        hash[6] = first_hash[6], hash[7] = first_hash[7];

        /* update for second chunk */
        data = 0;                                      // Clear
        data.set_slc(416, const_bits);
        data.set_slc(384, flip_nonce(nonce)); // set the nonce

        /* We have to
        *   1. Append 1 at the tail of message (Need 1 Byte)
        *   2. Fill the length of context at the end of chunk (Need 8 Byte).
        /* 1. Append 1 at the tail of message*/
        data.set_slc(383, (u1_t) 1);
        /* 2. Fill the length of context at the end of chunk*/
        data.set_slc(0, (u64_t) 640);  // bit length

        /* mid-update */
        sha256_update(data, hash);

        u256_t mid_hash = get_output_hash(hash);

        data = 0; // Clear
        data.set_slc(256, mid_hash);

        /* We have to
        *   1. Append 1 at the tail of message (Need 1 Byte)
        *   2. Fill the length of context at the end of chunk (Need 8 Byte).
        /* 1. Append 1 at the tail of message*/
        data.set_slc(255, (u1_t) 1);
        /* 2. Fill the length of context at the end of chunk*/
        data.set_slc(0, (u64_t) 256);  // bit length

        // reset the hash values
        hash[0] = 0x6a09e667, hash[1] = 0xbb67ae85;
        hash[2] = 0x3c6ef372, hash[3] = 0xa54ff53a;
        hash[4] = 0x510e527f, hash[5] = 0x9b05688c;
        hash[6] = 0x1f83d9ab, hash[7] = 0x5be0cd19;

        sha256_update(data, hash);
        nonce++; // increment the nonce
    }

    return get_output_hash(hash);
}
