


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/macros.h"

void bin2hex(char *s, void *p, int len) {
  int i;
  printf("%s : ", s);
  for (i=0; i<len; i++) {
    printf ("%02x ", ((uint8_t*)p)[i]);
  }
  printf("\n\n");
}

// 1234 or 0001 0010 0011 0100
// 56B9 or 0101 0110 1011 1001

uint8_t sub_byte(uint8_t x)
{
  const uint8_t p_sbox[16] = 
  { 0xc, 0x5, 
    0x6, 0xb, 
    0x9, 0x0, 
    0xa, 0xd,
    0x3, 0xe, 0xf, 0x8, 0x4, 0x7, 0x1, 0x2 };

  const uint8_t rr_sbox[16] = 
  { 0x0, 0x8, 0x6, 0xd, 0x5, 0xf, 0x7, 0xc,
    0x4, 0xe, 0x2, 0x3, 0x9, 0x1, 0xb, 0xa };
    
  return (rr_sbox[(x & 0xF0) >> 4] << 4) | 
          rr_sbox[(x & 0x0F)];    
}

void rr_sbox(uint8_t *x) {
  int i;
  for (i=0; i<4; i++) x[i] = sub_byte(x[i]);
}

void present_sbox(uint8_t X[4], uint8_t Y[4]) {
  register uint8_t T1,T2,T3,T4;
  
    T1   = X[2] ^ X[1];
    T2   = X[1] & T1;
    T3   = X[0] ^ T2;
    Y[3] = X[3] ^ T3;
    T2   = T1   & T3;
    T1  ^= Y[3];
    T2  ^= X[1];
    T4   = X[3] | T2;
    Y[2] = T1   ^ T4;
    T2  ^= ~X[3];
    Y[0] = Y[2] ^ T2;
    T2  |= T1;
    Y[1] = T3   ^ T2;
}

void sbox(uint8_t *x)
{
    uint8_t t;
    
    t = x[3];

    x[3] &= x[2];
    x[3] ^= x[1];
    x[1] |= x[2];
    x[1] ^= x[0];
    x[0] &= x[3];
    x[0] ^=  t; 
       t &= x[1];
    x[2] ^=  t;
}

uint8_t R0(uint8_t x) {      
    return x ^ ROTL8(ROTL8(x, 1) ^ x,  1);
}

uint8_t F0(uint8_t x) {
    return ROTL8(x, 1) ^ ROTL8(x, 2) ^ ROTL8(x, 7);
}

uint8_t F1(uint8_t x) {
    return ROTL8(x, 3) ^ ROTL8(x, 4) ^ ROTL8(x, 6);
}

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define H(x, y, z) ((x) ^ (y) ^ (z))

#define FX(x, y, z) (((x) ^ (y) ^ (z)))
#define FF(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z))) 
#define GG(x, y, z) ((z)  ^ ((x) & ((y) ^ (z))))

#define P0(x) x ^ ROTL32(x,  9) ^ ROTL32(x, 17)
#define P1(x) x ^ ROTL32(x, 15) ^ ROTL32(x, 23)

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define EP0(x) (ROTR32(x,2) ^ ROTR32(x,13) ^ ROTR32(x,22))
#define EP1(x) (ROTR32(x,6) ^ ROTR32(x,11) ^ ROTR32(x,25))

#define SIG0(x) (ROTR32(x,7) ^ ROTR32(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTR32(x,17) ^ ROTR32(x,19) ^ ((x) >> 10))

typedef union _w64_t {
  uint8_t b[8];
  uint32_t w[2];
  uint64_t q;
} w64_t;

typedef union _w128_t {
  uint8_t b[16];
  uint32_t w[4];
  uint64_t q[2];
} w128_t;

uint64_t X0 (void *data, void *key) {
  uint32_t x0, x1, t;
  w128_t   *k=(w128_t*)key;
  w64_t    *x=(w64_t*)data;
  int      i;
  
  x0 = x->w[0] ^ k->w[0]; 
  x1 = x->w[1] ^ k->w[1];
  
  for (i=1; i<=16; i++) {
    t  = ROTL32(x1 * (x1 + x1 + 1), 5);
    x0 = ROTL32(x0 ^ i, t) + k->w[i%4];
    XCHG(x0, x1);
  }
  x->w[0] = x0 ^ k->w[2]; 
  x->w[1] = x1 ^ k->w[3];
  return x->q;
}

uint64_t Q(w64_t *p){
    uint64_t q;
    uint32_t r;
    int      j;

    q = 0;    
    r = 0x30201000;  
    
    for (j=0; j<64; j++) {
      q |= ((p->q >> j) & 1) << (r & 255);      
      r = ROTR32(r+1, 8);
    }
    return q;
}

uint64_t X1 (void *data, void *key) {
    uint32_t t;
    w128_t   *k=(w128_t*)key;
    w64_t    *x=(w64_t*)data;
    uint64_t i;
    
    x->q ^= k->q[0];
    
    for (i=1; i<=8; i++) {
      x->q = Q(x) ^ i;
      XCHG(x->w[0], x->w[1]);
    }
    
    x->q ^= k->q[1];
    
    return x->q;
}

int main(void) 
{
  uint8_t x[4]={0x1,0x2,0x3,0x4};
  uint8_t y[4]={0x1,0x2,0x3,0x4};
  uint8_t z[4];
  uint8_t b[4]={0,0,0,1};
  uint8_t key[16], plain[8];
  int i;
  
  for (i=0; i<16; i++) {
    key[i]=(i+1);
    if (i<8) plain[i]=(i+1);
  }
  
  printf("%016llX\n", X0(plain, key));
  printf("%016llX\n", X1(plain, key));
  return 0;
  
  for (i=0; i<4; i++) {
    printf("%02x ", R0(R0(x[i])));    
  }
  putchar('\n');
  for (i=0; i<4; i++) {
    printf("%02x ", F0(x[i]));    
  }
  putchar('\n');
  for (i=0; i<4; i++) {
    printf("%02x ", F1(x[i]));    
  }  
  
  present_sbox(b, z);
  bin2hex("z", z, 4);
  rr_sbox(x);  
  bin2hex("x", x, 4);
  return 0;
}
  