#include <stdio.h>
#include <stdlib.h>
#include "rlwe.h"

int main( ) 
{
  RLWE alice;

  alice.genkeys( );
  alice.writes( );
  alice.encrypts( );
  alice.decrypts( );
  
  return 0;
}
