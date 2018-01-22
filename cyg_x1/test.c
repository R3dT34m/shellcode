


#include <stdint.h>
#include <stdio.h>
#include <string.h>

void bin2hex(char *s, void *p, int len) {
  int i;
  printf("%s : ", s);
  for (i=0; i<len; i++) {
    printf ("%02x ", ((uint8_t*)p)[i]);
  }
  printf("\n\n");
}

uint8_t sub_byte(uint8_t x)
{
  const uint8_t p_sbox[16] = 
  { 0xc, 0x5, 0x6, 0xb, 0x9, 0x0, 0xa, 0xd,
    0x3, 0xe, 0xf, 0x8, 0x4, 0x7, 0x1, 0x2 };

  const uint8_t rr_sbox[16] = 
  { 0x0, 0x8, 0x6, 0xd, 0x5, 0xf, 0x7, 0xc,
    0x4, 0xe, 0x2, 0x3, 0x9, 0x1, 0xb, 0xa };
    
  return (p_sbox[(x & 0xF0) >> 4] << 4) | 
          p_sbox[(x & 0x0F)];    
}

void rr_sbox(uint8_t *x) {
  int i;
  for (i=0; i<4; i++) x[i] = sub_byte(x[i]);
}

void present_sbox(uint8_t X[4], uint8_t Y[4]) {
  register uint8_t T1,T2,T3,T4;
  
  T1 = X[2] ^ X[1];
  T2 = X[1] & T1;
  T3 = X[0] ^ T2;
  Y[3] = X[3] ^ T3;
  T2 = T1 & T3;
  T1 ^= Y[3];
  T2 ^= X[1];
  T4 = X[3] | T2;
  Y[2] = T1 ^ T4;
  T2 ^= ~X[3];
  Y[0] = Y[2] ^ T2;
  T2 |= T1;
  Y[1] = T3 ^ T2;
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

int main(void) 
{
  uint8_t x[4]={0x1,0x2,0x3,0x4};
  uint8_t y[4]={0x1,0x2,0x3,0x4};
  uint8_t z[4];
  
  present_sbox(x, z);
  bin2hex("z", z, 4);
  rr_sbox(x);  
  bin2hex("x", x, 4);
  return 0;
}
  