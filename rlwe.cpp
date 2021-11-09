#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "rlwe.h"

RLWE::RLWE() {
  n = 8;
  N = 256;             // N = 2^n
  p = 12289;           // p is prime and p = 1 (mod 2N)
  l = 12;              // l is the size of the error
  seed();
  udef();
  vdef();
  genkeys();
  
  printf("\nring learning with errors\n");
  printprime();
  printkeys();
}

void RLWE::seed() {
  srand(time(NULL));
}

void RLWE::udef() {
  uint8_t  u1 = 3;     // u1 is a primitive 2Nth root of unity
  uint16_t uk = 1;     // in Z / p
  uint16_t k;

  for(k = 0; k < N; k++) {
    u[rev(k)] = uk;    // u stores powers of u1 in bit reversed order
    uk *= u1;
    uk %= p;
  }
}

void RLWE::vdef() {
  uint16_t v1 = 8193;  // v1 = 1 / u1 (mod p)
  uint32_t vk = 1;
  uint16_t k;

  for(k = 0; k < N; k++) {
    v[rev(k)] = vk;    // v stores powers of v1 in bit reversed order
    vk *= v1;
    vk %= p;
  }
}

void RLWE::genkeys() {
  adef();
  lil(sk);
  lil(e);
  mul(a, sk, pk);
  add(pk, e, pk);
}

void RLWE::adef() {
  uint16_t k;
  
  for(k = 0; k < N; k++) {
    a[k] = rand() % p;
  }
}

void RLWE::lil(uint16_t *x) {
  uint16_t k;
  
  for(k = 0; k < N; k++) {
    x[k] = rand() % l;
  }
}

void RLWE::writes() {
  uint16_t k;
  
  for(k = 0; k < N; k++) {
    pt[k] = 0;
  }

  printf("\n\nwrite a short message and press enter\n");
  uint8_t length = 0; 
  uint8_t character = getchar();                            
  while(length < 32 && character != '\n') {               
    length++;
    for(k = 1; k <= 8; k++) {
      pt[8 * length - k] = character & 1;
      character >>= 1;
    }
    character = getchar();                                     
  }

  printf("\nplaintext\n");
  printplaintext();
}

void RLWE::encode() {
  mul((p - 1) / 2, pt, pt);
}

void RLWE::decode() {
  uint16_t k;
  
  for(k = 0; k < N; k++) {
    pt[k] = (4 * pt[k] + p - 1) / (2 * p - 2) % 2;
  }
}

void RLWE::encrypts() {
  encode();
  lil(e);
  mul(a,   e,  c);
  mul(pk,  e,  t);
  lil(e);
  add(c,   e,  c);
  add(pt,  t,  t);
  lil(e);
  add(e,   t,  t);
  mul(p-1, t,  t); 

  printf("encryption\n");
  printciphertext();
}

void RLWE::decrypts() {
  mul(c, sk, pt);
  add(t, pt, pt);
  decode();

  printf("\n\ndecryption\n");
  printplaintext();
}

void RLWE::printprime() {
  printf("\nprime p\n%i", p);
}

void RLWE::printkeys() {
  uint16_t k;
  
  printf("\n\nrandom a\n");
  for(k = 0; k < N; k++) {
    printf("%i, ", a[k]);
  }                 
                                             
  printf("\n\npublic key\n");
  for(k = 0; k < N; k++) {
    printf("%i, ", pk[k]);
  }
  
  printf("\n\nsecret key\n");
  for(k = 0; k < N; k++) {
    printf("%i, ", sk[k]);                              
  }       
}

void RLWE::printplaintext() {
  uint16_t k;
  uint8_t  character;
  uint8_t  i;
  
  for(k = 0; k < 32; k++) {
    character = pt[8 * k];
    for(i = 1; i < 8; i++) {
      character <<= 1;
      character += pt[8 * k + i];
    }
    printf("%c", character);
  }
  printf("\n");
  for(k = 0; k < N; k++) {
    printf("%i", pt[k]);
  }
  printf("\n\n");
}

void RLWE::printciphertext() {
  uint16_t k;

  for(k = 0; k < N; k++) {
    printf("%i, ", c[k]);
  }
  printf("\n\n");
  for(k = 0; k < N; k++) {
    printf("%i, ", t[k]);
  }
}

// the following function is 
// a negacyclic decimation-in-time number theoretic transform.
// it    is essentially algorithm 1 from longa-naehrig 2016
// which is essentially algorithm 7 from poeppelmann-oder-gueneysu 2015
void RLWE::ntt(uint16_t *x) {
  uint8_t  i;
  uint16_t j,  k;
  uint8_t  r,  s,  t;
  uint32_t    xs, xt;
  
  for(i = 0; i < n; i++) {
    for(j = 0; j < (1 << i); j++) {         // j <      2^i
      for(k = 0; k < (N >> (i + 1)); k++) { // k <  N / 2^(i+1)
          r  =       (1 << i) + j;          // r =      2^i     + j
	  s  =  j  * (N >> i) + k;          // s = jN / 2^i     + k
	  t  = (N >> (i + 1)) + s;          // t =  N / 2^(i+1) + s
         xs  = x[s] + u[r] *      x[t];
	 xt  = x[s] + u[r] * (p - x[t]);
        x[s] = xs % p;
        x[t] = xt % p;
      }
    }
  }
}

// the following function is 
// a negacyclic decimation-in-frequency number theoretic transform.
// it is the inverse of the function ntt.
// it    is essentially algorithm 2 from longa-naehrig 2016
// which is essentially algorithm 8 from poeppelmann-oder-gueneysu 2015
void RLWE::intt(uint16_t *x) {
  uint8_t   i;
  uint16_t  j,  k;
  uint8_t   r,  s,  t;
  uint32_t     xs, xt;
  uint32_t     xk;

  for(i = 0; i < n; i++) {
    for(j = 0; j < (1 << i); j++) {         // j <     2^i
      for(k = 0; k < (N >> (i + 1)); k++) { // k < N / 2^(i+1)
          r  = (N >> (i + 1)) + k;          // r = N / 2^(i+1) + k
	  s  = (k << (i + 1)) + j;          // s = k * 2^(i+1) + j
	  t  =       (1 << i) + s;          // t =     2^i     + s
	 xs  =  x[s] +      x[t];
	 xt  = (x[s] + (p - x[t])) * v[r];
	x[s] = xs % p;
	x[t] = xt % p;
      }
    }
  }

  for(k = 0; k < N; k++) {
     xk  = 12241 * x[k];                    // 12241 = 1 / N (mod p)
    x[k] = xk % p;
  }
}

void RLWE::add(uint16_t *x, uint16_t *y, uint16_t *z) {
  uint16_t k;
  
  for(k = 0; k < N; k++) {
    z[k] = (x[k] + y[k]) % p;
  }
}

void RLWE::mul(uint16_t  x, uint16_t *y, uint16_t *z) {
  uint32_t k;
  
  for(k = 0; k < N; k++) {
    z[k] = (x * y[k]) % p;
  }
}

void RLWE::mul(uint16_t *x, uint16_t *y, uint16_t *z) {
  uint16_t  k;
  uint32_t zk;
  
  ntt(x);
  ntt(y);
  for(k = 0; k < N; k++) {
    zk = x[k] * y[k];
    z[k] = zk % p;
  }
  intt(x);
  intt(y);
  intt(z);
}

// the following function reverses the bits in a byte.
// it is from 'bit twiddling hacks' by sean eron anderson
// http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64BitsDiv
// where it is attributed to rich schroeppel.
uint8_t rev(uint8_t k) {
   return (k * 0x0000000202020202 & 0x0000010884422010) % 1023;
}

