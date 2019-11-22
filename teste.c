#include<stdio.h>
#include<stdint.h>

#define N 10 		//numero de bits para a parte fracionaria
#define K (1 << (N-1)) //fator de arredondamento
#define Float_i6f10(x) (int16_t)(x*(float)(1<<N)) 
#define i6f10_Float(x) (float)(x/(float)(1<<N))

int16_t Add_i6f10(int16_t a, int16_t b){
  int32_t temp;
  int16_t result;

  temp = (int32_t)a + (int32_t)b;

  if(temp > INT16_MAX) temp = INT16_MAX;
  if(temp < INT16_MIN) temp = INT16_MIN;

  result = (int16_t)temp;

  return result;
}

int main(){
  int16_t x,y;
  float a=3.5, b=4.2;
  x = Float_i6f10(a);
  y = Float_i6f10(b);
  x = Add_i6f10(x,y);
  a = i6f10_Float(x);
  printf("%d %f",x, a);
  return 0;
}
