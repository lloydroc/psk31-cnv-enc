#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// http://my.com.nthu.edu.tw/~jmwu/com5195/viterbi_example.pdf
// http://home.netcom.com/~chip.f/viterbi/algrthms2.html

/** WARNING
 * when we have 4 states they are
 * thought about as
 * 00
 * 01
 * 10
 * 11
 *
 * But I would think the left most
 * bit as x1 and the rightmost as x0.
 *
 * This IS NOT how it is. We will put
 * x0 the left bit as x0 and the right
 * as x1.
 *
 * I suppose it is the delay lines
 * always go from left to right.
 * if you had a delay of 3 the
 * left most is x-0 and the right
 * most is x-2.
 *
 * We have the same for the output
 * bits. When we output 10 you could
 * think this is g1 = 1 and g0 = 1
 * but it is again opposite where
 * g1 = 0 and g0 is 1. The g are
 * the generator polynomials.
 */

void cnv_enc(unsigned int len, unsigned int arr[], unsigned int *output[]);
int cnv_enc_next(int i);
void print_next_state_arr();
void print_output_arr();
void print_state_trans_arr();
int f0(int x4, int x3, int x2, int x1, int x0);
int f1(int x4, int x3, int x2, int x1, int x0);

int main(int argc, char *argv[])
{
  print_next_state_arr();
  print_output_arr();
  print_state_trans_arr();

  FILE* fdo;
  if(argc > 1)
  {
    fdo = fopen(argv[1],"w");
  }
  else
  {
    fdo = fopen("psk31_out.txt","w");
  }

  //unsigned int input[INPUT_LENGTH] = {0,3,3,0,1,2,1,3,3,2,0,0,3,0,3,2,3,0,0,0};
  unsigned int len = 20;
  unsigned int input[] = {0,1,0,1,1,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0};
  unsigned int *output;

  cnv_enc(len,input,&output);
  //free(*output);
  fprintf(fdo,"unsigned int input[INPUT_LENGTH] = {");
  for(int i=0;i<len;i++) {
    printf("%d",output[i]);
    fprintf(fdo,"%d",output[i]);
    if(i<len-1)
    {
      fprintf(fdo,",");
    }
  }
  fprintf(fdo,"};");
}

void cnv_enc(unsigned int len, unsigned int input[], unsigned int *output[])
{
  unsigned int *arr = calloc(len,sizeof(int));
  *output = arr;

  for(int i=0;i<len;i++)
  {
    printf("%d   ",input[i]);
  }
  puts("");
  for(int i=0;i<len;i++)
  {
    int enc = cnv_enc_next(input[i]);
    arr[i] = enc;
  }
}

int cnv_enc_next(int x0)
{
  static int x1=0,x2=0,x3=0,x4=0;
  int g0,g1;
  g0 = f0(x4,x3,x2,x1,x0);
  g1 = f1(x4,x3,x2,x1,x0);

  int state = 0;
  state |= x4<<4;
  state |= x3<<3;
  state |= x2<<2;
  state |= x1<<1;
  state |= x0<<0;

  x4 = x3;
  x3 = x2;
  x2 = x1;
  x1 = x0;

  printf("%2d -> %d%d (%d)\n",state,g0,g1,(g0<<1)|g1);

  return (g0<<1)|g1;
}

int f0(int x4, int x3, int x2, int x1, int x0)
{
  int g0 = (x4+x2+x1+x0)%2;
  return g0;
}

int f1(int x4, int x3, int x2, int x1, int x0)
{
  int g1 = (x4+x3+x0)%2;
  return g1;
}

void print_next_state_arr()
{
  unsigned int next;
  puts("unsigned int next_state_arr[NUM_STATES][2] =\n{");

  for(int i=0;i<32;i++)
  {
    next = 31&(i<<1);
    printf("  {%2d,%2d}",next,next|1);
    if(i<32)
    {
      printf(", // state %d\n",i);
    }
    else
    {
      printf("  // state %d\n",i);
    }
  }
  puts("};");
}

void print_state_trans_arr()
{
  unsigned int next_0,next_1,prev=0;
  unsigned int *s;
  s = calloc(32,sizeof(int));

  puts("unsigned int state_trans[NUM_STATES][32] =\n{");

  for(int i=1;i<33;i++)
  {
    next_0 = 31&(prev<<1);
    next_1 = next_0|1;
    printf("  {");
    for(int j=0;j<32;j++)
    {
      s[j] = 255;
      if(j==next_0)
      {
         s[j] = 0;
      }
      else if(j==next_1)
      {
         s[j] = 1;
      }
      printf("%2d",s[j]);
      if(j<31)
      {
        printf(",");
      }
      else
      {
        printf("}, // state %d",i);
      }
    }

    if(i<32)
    {
      puts(",");
    }
    else
    {
      puts("");
    }
    prev = i;
  }
  puts("};");
}

void print_output_arr()
{
  unsigned int g0,g1;
  unsigned int x1,x2,x3,x4;
  unsigned int os;

  puts("unsigned int output_state_arr[NUM_STATES][2] =\n{");
  for(int i=0;i<32;i++)
  {

    int x4 = (i&16)>>4;
    int x3 = (i& 8)>>3;
    int x2 = (i& 4)>>2;
    int x1 = (i& 2)>>1;

    int os00 = f0(x4,x3,x2,x1,0)<<1;
    int os01 = f1(x4,x3,x2,x1,0);
    int os10 = f0(x4,x3,x2,x1,1)<<1;
    int os11 = f1(x4,x3,x2,x1,1);
    int os_if_0 = os00|os01;
    int os_if_1 = os10|os11;
    printf("  {%d,%d}",os_if_0,os_if_1);
    if(i<31)
    {
      printf(", // state %d\n",i);
    }
    else
    {
      printf("  // state %d\n",i);
    }
  }
  puts("};");

}

/*void print_output_arr()
{
  unsigned int g0,g1;
  unsigned int x0,x1,x2,x3,x4;
  unsigned int os;

  puts("unsigned int output_state_arr[NUM_STATES][2] =\n{");
  for(int i=0;i<32;i++)
  {
    int x4 = (i&16)>>4;
    int x3 = (i& 8)>>3;
    int x2 = (i& 4)>>2;
    int x1 = (i& 2)>>1;
    int x0 = (i& 1)>>0;
    int os00 = f0(x4,x3,x2,x1,0)<<1;
    int os01 = f1(x4,x3,x2,x1,0);
    int os10 = f0(x4,x3,x2,x1,1)<<1;
    int os11 = f1(x4,x3,x2,x1,1);
    int os_if_0 = os00|os01;
    int os_if_1 = os10|os11;
    printf("  {%d,%d}",os_if_0,os_if_1);
    if(i<31)
    {
      printf(", // state %d\n",i);
    }
    else
    {
      printf("  // state %d\n",i);
    }
  }
  puts("};");

}*/
