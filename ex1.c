#include <stdio.h>
#include <stdlib.h>

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
void print_tables();
void print_tables_sql();
void print_next_state_arr();
void print_output_arr();
void print_state_trans_arr();
int f0(unsigned int x2,unsigned int x1,unsigned int x0);
int f1(unsigned int x2,unsigned int x1,unsigned int x0);

int main(int argc, char *argv[])
{
  print_tables();
  print_tables_sql();
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
    fdo = fopen("out.txt","w");
  }

  //unsigned int input[INPUT_LENGTH] = {0,3,3,0,1,2,1,3,3,2,0,0,3,0,3,2,3,0,0,0};
  unsigned int len = 20;
  unsigned int input[] = {0,1,0,1,1,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0};
  unsigned int *output;

  cnv_enc(len,input,&output);
  //free(*output);
  for(int i=0;i<len;i++) {
    printf("%d",output[i]);
    fprintf(fdo,"%d",output[i]);
  }
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
    int x1 = (enc&2);
    int x0 = (enc&1);
    printf("%d%d  ",x0,x1>>1);
    arr[i] = enc;
  }
}

int cnv_enc_next(int x0)
{
  static int x2 =0, x1 = 0;
  int g0,g1;
  g0 = f0(x2,x1,x0);
  g1 = f1(x2,x1,x0);
  x2 = x1;
  x1 = x0;
  return (g0<<1)+g1;
}

int f0(unsigned int x2,unsigned int x1,unsigned int x0)
{
  int g0 = (x2+x1+x0)%2;
  return g0;
}

int f1(unsigned int x2,unsigned int x1,unsigned int x0)
{
  int g1 = (x2+x0)%2;
  return g1;
}

void print_tables()
{
  unsigned int g0,g1;
  unsigned int x0,x1,x2,x3,x4;
  unsigned int os;
  unsigned int is;

  for(int i=0;i<4*2;i++)
  {
    x2 = (i&4)>>2;
    x1 = (i&2)>>1;
    x0 = (i&1);
    g0 = f0(x2,x1,x0);
    g1 = f1(x2,x1,x0);
    os = (g1<<1)+g0;
    is = i&3;
    printf("%d%d %d/%d%d --> %d%d\n",x1,x0,x2,g0,g1,x2,x1);
  }

}

void print_next_state_arr()
{
  unsigned int next_0,next_1,prev=0;

  puts("unsigned int next_state_arr[NUM_STATES][2] =\n{");

  for(int i=1;i<5;i++)
  {
    next_0 = (prev&2)>>1;
    next_1 = next_0|2;
    printf("  {%d,%d}",next_0,next_1);
    prev = i;
    if(i<4)
    {
      puts(",");
    }
    else
    {
      puts("");
    }
  }
  puts("};");
}

void print_state_trans_arr()
{
  unsigned int next_0,next_1,prev=0;

  puts("unsigned int state_trans[NUM_STATES][4] =\n{");

  for(int i=1;i<5;i++)
  {
    next_0 = (prev&2)>>1;
    next_1 = next_0|2;
    printf("  {");
    int s[4] = {255,255,255,255};
    for(int j=0;j<4;j++)
    {
      if(j==next_0)
      {
         s[j] = 0;
      }
      else if(j==next_1)
      {
         s[j] = 1;
      }
      printf("%d",s[j]);
      if(j<3)
      {
        printf(",");
      }
      else
      {
        printf("}");
      }
    }

    if(i<4)
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
  unsigned int x0,x1,x2;
  unsigned int os;

  puts("unsigned int output_state_arr[NUM_STATES][2] =\n{");
  for(int i=0;i<4;i++)
  {
    int x1 = (i&2)>>1;
    int x2 = (i&1)>>0;
    int os00 = f0(x2,x1,0)<<1;
    int os01 = f1(x2,x1,0);
    int os10 = f0(x2,x1,1)<<1;
    int os11 = f1(x2,x1,1);
    int os_if_0 = os00|os01;
    int os_if_1 = os10|os11;
    printf("  {%d,%d}",os_if_0,os_if_1);
    if(i<3)
    {
      puts(",");
    }
    else
    {
      puts("");
    }
  }
  puts("};");

}

void print_tables_sql()
{
  unsigned int g0,g1;
  unsigned int x0,x1,x2;
  unsigned int os, next,prev=0;


  char output_state[64];
  char next_state[64];
  char machine[64];

  for(int i=0;i<4;i++)
  {
    int x1 = (i&2)>>1;
    int x2 = (i&1)>>0;
    int os00 = f0(x2,x1,0)<<1;
    int os01 = f1(x2,x1,0);
    int os10 = f0(x2,x1,1)<<1;
    int os11 = f1(x2,x1,1);
    int os_if_0 = os00|os01;
    int os_if_1 = os10|os11;
    int next_0 = (prev&2)>>1;
    int next_1 = next_0|2;
    //printf("%d%d %d/%d%d --> %d%d\n",(i&2)>>1,i&1,0,os00>>1,os01,(next_0&2)>>1,(next_0&1));
    //printf("%d%d %d/%d%d --> %d%d\n",(i&2)>>1,i&1,1,os10>>1,os11,(next_1&2)>.1,(next_1&1));
    //printf("insert into output_state(current,input_zero,input_one) values(%d,%d,%d)\n",i,os_if_0,os_if_1);
    //printf("insert into next_state(current,input_zero,input_one) values(%d,%d,%d)\n",i,next_0,next_1);
    prev = i;
    // i   x1   x2   if0   if1
    // 0   0    0     0
    // 1   1    0     0     2
    // 2   0    1     1     3        2
    // 3   1    1     1     3
    //
    // 00  00   00    00    00
    // 01  00   00    00    10
    // 10  01   00    00    10
    // 10  10   01    01    11
    // 11  10   01    01    11
  }
}
