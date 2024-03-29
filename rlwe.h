#include <stdlib.h>
         
class RLWE // RLWE stands for 'ring learning with errors 
{          // the rings is the finite ring R = ( Z/p )[ x ] / ( x^N + 1 )
public:        
  uint8_t    n;        // n = 8
  uint16_t   N;        // N = 2^n = 256
  uint16_t   p;        // p = 12289 is prime, and p = 1 (mod 2N) 
  uint16_t   l;        // l   is the size of the error

  uint16_t pk1[ 256 ]; // pk1 is half of the public key
  uint16_t pk2[ 256 ]; // pk2 is half of the public key
  uint16_t  pt[ 256 ]; // pt  is the plain text
  
  RLWE( );
  void genkeys( );     // defines  the arrays pk, sk
  void writes( );      // defines  the array  pt
  void encrypts( );    // defines  the arrays c,t
  void decrypts( );    // rewrites the array  pt

private:
  uint16_t  sk[ 256 ];         // sk  is the secret key
  uint16_t ct1[ 256 ];         // ct1 is half of the cipher text
  uint16_t ct2[ 256 ];         // ct2 is half of the cipher text 
  uint16_t   e[ 256 ];         // e   is the error
  uint16_t   u[ 256 ];         // u   is an array of         'twiddle factors' 
  uint16_t   v[ 256 ];         // v   is an array of 'inverse twiddle factors' 

  void seed( );                // seeds   the random number generator
  void pk1def( );              // defines the array   pk1
  void udef( );                // defines the array   u
  void vdef( );                // defines the array   v
  
  void sample( uint16_t * x ); // defines a   small   x     

  void encode( );              // encodes the array   pt
  void decode( );              // decodes the array   pt

  void printprime( );
  void printkeys( );
  void printplaintext( );
  void printciphertext( );

  void  ntt( uint16_t * x );  // applies a number theoretic transform to x in R
  void intt( uint16_t * x );  // applies the inverse        transform to x in R
  
  void  add( uint16_t * x, uint16_t * y, uint16_t * z );      // x + y = z in R
  void  mul( uint16_t   x, uint16_t * y, uint16_t * z );      // x * y = z in R
  void  mul( uint16_t * x, uint16_t * y, uint16_t * z );      // x * y = z in R
};

uint8_t rev( uint8_t k ); // reverses the bits of k
