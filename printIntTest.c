#include <math.h>

void writeInt(int fd, int val){

  int nbDigits(unsigned int x){
    if ( x == 0){
      return 1;
    }else{
      return (int)log10(val)+1;
    }
  }	
  
  int n = nbDigits(val);
  char string[n];
  unsigned int i = n;
  
  string[0]='0';
  while ( i > 0 ) {
    i--;
    string[i] = '0' + (val % 10);
    val = val / 10;
 }
  write(0,&string,n);
  return;
}

