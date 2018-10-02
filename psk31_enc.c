#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// convolutional encoder function
// arr is an array of length len
// output will be an array of length len with the encoded bits
// where the output 2 bits where the msb is g0 and the lsb is g1
void cnv_enc(unsigned int len, unsigned int arr[], unsigned int *output[]);
int cnv_enc_next(int i);

// generator functions
int g0(int x4, int x3, int x2, int x1, int x0);
int g1(int x4, int x3, int x2, int x1, int x0);

void read_input(unsigned int *input, unsigned int *len);

int main(int argc, char *argv[])
{
  // print to stdout unless a file name is provided
  FILE* fdo = stdout;
  if(argc > 2)
  {
    printf("%s [filename]\n",argv[0]);
    return 1;
  }
  else if(argc == 2 && (strcmp("--help",argv[1])==0 || strcmp("-h",argv[1])==0))
  {
    printf("%s [filename]\n",argv[0]);
    return 1;
  }
  else if(argc == 2)
  {
    fdo = fopen(argv[1],"w");
  }
  // our sample input stream
  unsigned int input[1024];
  unsigned int len;
  read_input(input,&len);

  // the 2-bit output of our encoder
  // note the bit0 is g1 and bit1 is g0
  unsigned int *output;

  cnv_enc(len,input,&output);
  for(int i=0;i<len;i++) {
    fprintf(fdo,"%d",output[i]);
  }
  if(fdo != stdout) {
    fclose(fdo);
  }
  free(output);
  return EXIT_SUCCESS;
}

void read_input(unsigned int *input, unsigned int *len)
{
  int n = 0;
  char buf[1];

  while(read(0, buf, sizeof(buf))>0) {
    switch(buf[0])
    {
      case '0':
        input[n++] = 0;
        break;
      case '1':
        input[n++] = 1;
        break;
      case '\n':
        *len = n;
        return;
        break;
      default:
        break;
    }
  }
}


void cnv_enc(unsigned int len, unsigned int input[], unsigned int *output[])
{
  unsigned int *arr = calloc(len,sizeof(int));
  *output = arr;

  for(int i=0;i<len;i++)
  {
    int enc = cnv_enc_next(input[i]);
    arr[i] = enc;
  }
}

int cnv_enc_next(int x0)
{
  static int x1=0,x2=0,x3=0,x4=0;
  int r0,r1;
  r0 = g0(x4,x3,x2,x1,x0);
  r1 = g1(x4,x3,x2,x1,x0);

  x4 = x3;
  x3 = x2;
  x2 = x1;
  x1 = x0;

  return (r0<<1)|r1;
}

int g0(int x4, int x3, int x2, int x1, int x0)
{
  int g0 = (x4+x2+x1+x0)%2;
  return g0;
}

int g1(int x4, int x3, int x2, int x1, int x0)
{
  int g1 = (x4+x3+x0)%2;
  return g1;
}
