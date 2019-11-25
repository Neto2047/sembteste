//GitHub: HenriqueIni
//www.blogcyberini.com

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <xc.h>
#include "uart.h"

#pragma config FEXTOSC = OFF    // External Oscillator Selection (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_64MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCK bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCK bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (ULPBOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT Disabled; SWDTEN is ignored

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG4
#pragma config WRTB = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-30000Bh) not write-protected)
#pragma config WRTC = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)

// CONFIG5L
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

// CONFIG5H

//#pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//#include <xc.h>

// F_OSC
#define _XTAL_FREQ 64000000

#define K 42 //numero de equacoes
#define Nb 10

#define Kround (1 << (Nb-1)) //fator de arredondamento
#define float_i6f10(x) (int16_t)(x*(float)(1<<Nb))
#define i6f10_float(x) (float)(x/(float)(1<<Nb))

int16_t i6f10Add(int16_t a, int16_t b){
  int32_t temp;
  int16_t res;

  temp = (int32_t)a + (int32_t)b;

  if(temp < INT16_MIN) temp = INT16_MIN;
  if(temp > INT16_MAX) temp = INT16_MAX;

  res = (int16_t)temp;

  return res;
}

int16_t i6f10Sub(int16_t a, int16_t b){
  return (a-b);
}

int16_t i6f10Mult(int16_t a, int16_t b){
  int32_t temp;
  int16_t res;

  temp = (int32_t)a * (int32_t)b;
  temp += Kround;
  temp = (int32_t)(temp/(float)(1<<N));
  if(temp > INT16_MAX) temp = INT16_MAX;
  if(temp < INT16_MIN) temp = INT16_MIN;

  res = (int16_t)temp;
  return temp;
}

int16_t i6f10Div(int16_t a, int16_t b){
  int32_t temp;
  int16_t res;

  temp = (int32_t)a<<N;
  temp = temp + (b>>1);
  res = (int16_t)(temp/(int32_t)b);

  return res;
}

void gaussSolver(int n, int16_t A[K][K], int16_t b[K], int16_t X[K]) {
    int i, j, k, l, m; //declaracao de varáveis para serem usadas nos loopings
    int16_t F;
    int16_t min=0, max=0;
    //ETAPA DE ESCALONAMENTO: faz a pivotacao e torna a matriz de icognitas 'A' em triangular superior
    for (k = 0; k < n - 1; k++) { //Percorre os valores dos indices da diagonal principal
         max = abs(A[k][k]); //max recebe o valor em modulo na linha e coluna de k(diagonal)
        int maxIndex = k; //o indice da diagonal é posto como o atual indice com o maior valor
        /*procura o maior k-ésimo coeficiente em módulo: Nessa interacao sera feita a pivotacao(troca de linhas), caso exista algum
       indice cujo o valor armazenado em modulo seja maior que o valor do atual indice max(na primeira iteracao
         valor maximo atual eh o da diagonal)*/
        for (i = k + 1; i < n; i++) { //Checa as linhas i da coluna k+1(abaixo do pivo)
            if (max < abs(A[i][k])) { //caso exista algum indice com valor em modulo maior que o atual 'max'
                max = fabs(A[i][k]);  //esse valor passara a ser o novo 'max'
                maxIndex = i; //i passa a ser o novo indice 'max'
            }
        }
        if (maxIndex != k) { //Se na iteracao anterior for encontrado algum indice com valor 'max' maior que o k(pivo inicial da diagonal)
            /*
             troca a equação k pela equação com o
             maior k-ésimo coeficiente em módulo(Pivotacao completa)
             */
            for (j = 0; j < n; j++) { //realiza a o swap da linha k(primeiro pivo) pela linha i(atual max)
                int16_t temp = A[k][j]; 
                A[k][j] = A[maxIndex][j]; 
                A[maxIndex][j] = temp; 
            }
            int16_t temp = b[k]; //realiza o mesmo swap no vetor 'b', para que os termos idependentes continuem referenciando suas respectivas linhas
            b[k] = b[maxIndex]; //swap
            b[maxIndex] = temp; //swap
        }
        //Se A[k][k] é zero, então a matriz dos coeficiente é singular
        //det A = 0
        if (A[k][k] == 0) { //Caso depois de todas as iteracoes anteriores, o indice A[k][k] tiver zero como seu valor 
            printf("A matriz dos coeficientes e singular\n"); 
            return; //O sistema linear ou eh impossivel ou eh possivel e indeterminado(nao tem solucao unica), logo nao pode ser resolvido.
        } else {
            //realiza o escalonamento: 
            for (m = k + 1; m < n; m++) { //caso A[k][k] seja diferente de 0, é feita as operacoes para tornar a matriz 'A' em uma matriz diagonal superior
                F = -i6f10Div(A[m][k],A[k][k]);
                A[m][k] = 0; //economiza uma iteracao, ja que as operacoes garantem que os valores das linhas da coluna k abixo do pivo serao 0
                b[m] = i6f10Add(b[m],i6f10Mult(F,b[k])); //realiza as operacoes na linha m do vetor de termos independetes 'b'.
                for (l = k + 1; l < n; l++) { //realiza as operacoes para toda as colunas e linhas da matriz 'A' seguintes ao pivo.
                    A[m][l] = i6f10Mult(A[m][l],i6f10Mult(F,A[k][l]));
                }
            }
        }
    }
    /*ETAPA DE RESOLUÇÃO DO SISTEMA: Nessa etapa, primeiro fazemos as operacoes para descobrir a resolucao de x[n](ultima linha da matriz)
	,em seguida, a partir desse resultado, eh feito a resolucao de x[n-1], logo depois x[n-2] e assim por diante ate x[0].(substituicoes retroativas)*/
    for (i = n - 1; i >= 0; i--) {
        X[i] = b[i]; //vetor resolução recebe os novos valores calculados do vetor dos termos independentes
        for (j = i + 1; j < n; j++) { //substituicao retroativa
            X[i] = i6f10Sub(X[i],i6f10Mult(X[j],A[i][j])); 
        }
        X[i] = i6f10Div(X[i],A[i][i]);//a ultima linha do vetor de resolucao tera como resultado o valor da linha ,dividido pela valor do ultimo elemento da matriz(A[n][n])
    }
}
//Código de testes
const double A[K][K] = {{-2 , -3 ,  4 , -9 , -5 , -1 ,  1 ,  4 , -2 , -4 , -8 ,  8 ,  5 ,  2 ,  3 ,  8 , -8 , -4 , -9 , -5 ,  9 , -6 , -5 ,  7 ,  6 , -4 , -5 , -3 , -8 , -6 , -2 , -7 ,  2 ,  9 ,  8 , -1 ,  6 , -8 ,  3 , -5 ,  2 ,  1}, { 1 , -5 ,  0 ,  2 ,  9 ,  1 , -3 , -1 , -6 ,  8 , -1 ,  9 , -8 ,  4 ,  6 ,  1 ,  3 , -3 ,  2 ,  4 , -5 ,  6 ,  1 , -4 , -4 , -4 , -3 ,  2 ,  4 , -9 ,  6 ,  0 ,  7 ,  5 ,  2 ,  6 , -1 , -4 ,  3 ,  8 , -5 , -9}, {-6 ,  1 ,  1 ,  6 , -9 , -2 ,  9 , -5 ,  3 ,  3 , -5 ,  0 , -5 ,  9 ,  4 , -5 ,  4 ,  2 ,  0 ,  0 , -2 ,  8 , -2 ,  5 , -8 ,  7 ,  0 ,  9 , -2 , -4 ,  1 ,  9 ,  6 ,  1 , -9 ,  7 , -6 , -7 ,  0 , -6 ,  9 ,  0}, { 8 ,  0 ,  2 ,  8 ,  1 , -7 , -3 , -7 , -3 , -7 ,  5 , -5 ,  3 , -7 , -3 ,  9 , -8 , -3 ,  4 ,  2 , -7 , -5 , -1 ,  6 ,  4 ,  8 , -8 ,  4 , -9 ,  7 , -3 ,  5 ,  6 , -5 , -3 ,  6 ,  9 ,  0 ,  3 , -8 ,  3 , -9}, {-6 , -2 ,  2 ,  2 ,  4 ,  2 , -6 ,  1 ,  5 , -6 , -3 , -4 ,  1 ,  9 ,  2 ,  5 ,  7 , -7 , -3 ,  2 ,  0 , -3 , -6 , -9 , -1 ,  3 , -5 , -1 , -9 ,  6 , -3 , -5 , -7 ,  0 ,  7 ,  3 ,  9 ,  8 ,  9 ,  8 ,  6 ,  6 }, {-2 ,  3 ,  6 ,  9 , -1 ,  1 , -6 , -9 ,  0 ,  7 , -3 ,  1 ,  8 ,  3 , -6 , -6 , -7 ,  0 ,  4 , -1 ,  8 ,  3 ,  2 , -8 ,  7 , -8 ,  9 ,  2 , -9 , -4 ,  9 ,  2 , -5 , -4 , -6 ,  7 ,  5 ,  6 ,  0 , -8 ,  2 ,  9}, { 6 , -2 , -1 , -7 , -1 ,  2 ,  9 , -3 ,  9 , -4 , -2 , -2 ,  9 ,  3 , -7 ,  5 , -1 ,  7 , -6 ,  7 , -5 ,  4 , -1 ,  6 , -8 ,  5 ,  4 , -8 , -2 , -7 ,  9 , -5 ,  7 , -6 ,  2 ,  5 , -8 ,  3 ,  1 ,  1 ,  6 , -5 }, { 7 ,  9 ,  6 ,  8 , -7 ,  1 , -8 ,  8 ,  1 ,  6 ,  4 ,  4 , -3 ,  1 ,  1 , -5 , -9 ,  3 ,  5 , -7 , -4 ,  8 , -8 ,  4 ,  2 ,  8 ,  9 ,  0 , -2 , -1 , -1 ,  7 ,  7 , -7 ,  7 , -5 , -7 , -5 ,  0 , -1 , -4 , -5}, {-5 , -9 , -3 , -5 , -1 , -9 , -3 ,  2 ,  9 , -9 ,  6 ,  0 ,  5 ,  7 , -1 , -6 ,  6 , -6 ,  4 ,  2 , -2 ,  1 ,  3 ,  9 ,  8 ,  6 , -6 , -4 , -1 , -9 , -9 ,  9 , -6 ,  2 ,  1 ,  7 ,  3 , -9 ,  5 , -2 ,  4 ,  3}, {-2 , -9 , -1 , -7 , -4 ,  4 , -2 , -9 ,  3 ,  4 , -8 , -5 , -3 ,  1 ,  9 , -6 ,  3 , -2 , -5 ,  8 , -9 , -7 , -8 ,  4 , -9 , -6 ,  6 , -6 ,  6 , -6 ,  4 ,  2 ,  8 ,  8 ,  8 ,  1 , -1 ,  7 , -8 ,  2 , -8 ,  5}, {-1 ,  4 ,  7 ,  2 , -4 ,  5 ,  6 , -9 ,  4 , -7 ,  7 ,  3 , -6 ,  0 , -3 , -5 , -1 , -4 , -9 , -4 , -3 ,  1 ,  9 , -8 ,  7 , -5 , -4 , -8 , -8 ,  8 , -3 , -9 , -6 ,  9 ,  1 ,  8 , -1 ,  8 ,  6 , -4 ,  3 ,  6 }, { 3 , -8 , -7 ,  9 ,  1 ,  2 ,  0 , -8 ,  1 , -1 ,  8 , -8 , -7 ,  5 ,  9 , -7 ,  6 ,  4 ,  5 , -3 , -9 ,  2 ,  8 , -4 ,  0 ,  0 ,  7 , -4 ,  1 ,  5 , -2 ,  7 ,  8 ,  4 , -2 ,  0 , -1 ,  5 ,  2 , -2 , -3 , -1}, {2 , -6 , -8 , -9 ,  3 , -3 , -9 , -9 ,  2 ,  6 , -7 ,  2 ,  9 ,  2 , -5 , -6 , -9 ,  5 , -4 , -7 ,  8 ,  4 ,  0 ,  8 , -5 ,  8 ,  5 , -4 , -5 , -6 ,  4 ,  5 , -4 ,  7 ,  4 ,  8 ,  1 , -8 ,  1 , -5 ,  2 ,  2}, {9 ,  6 , -5 , -9 , -8 , -6 ,  5 ,  2 ,  3 ,  5 ,  3 , -2 ,  8 ,  8 ,  8 , -8 , -5 ,  2 ,  4 , -6 , -9 , -5 ,  6 , -7 , -4 , -7 ,  9 ,  0 , -5 ,  7 , -1 ,  4 , -9 , -2 ,  5 , -6 , -4 ,  5 ,  4 ,  6 ,  2 , -6}, {3 ,  5 , -5 , -7 , -1 ,  5 , -3 ,  5 , -5 ,  7 ,  9 , -6 , -8 ,  3 , -7 , -9 , -8 , -4 ,  1 , -3 ,  2 , -9 ,  6 , -8 ,  1 , -6 ,  5 ,  1 ,  3 ,  9 ,  3 ,  4 , -2 , -9 ,  7 , -1 , -3 , -9 ,  2 ,  4 ,  7 ,  8}, { 1 ,  4 ,  2 ,  2 ,  8 , -1 ,  8 ,  3 , -6 , -5 ,  7 , -6 ,  5 ,  1 ,  3 , -8 ,  4 ,  4 , -8 ,  0 ,  3 ,  6 ,  4 , -5 , -2 , -9 ,  7 , -2 , -6 ,  1 ,  6 , -5 , -7 ,  6 , -7 , -5 ,  4 ,  3 , -7 ,  7 ,  3 , -9}, { 8 ,  7 ,  8 ,  4 ,  9 ,  2 , -7 ,  0 , -7 , -3 , -4 ,  2 ,  3 , -8 ,  7 ,  5 ,  8 ,  1 , -4 ,  6 ,  1 , -8 , -2 ,  8 , -9 ,  8 ,  6 , -5 ,  9 ,  7 ,  7 ,  7 ,  5 ,  4 , -8 ,  5 , -7 , -6 , -9 , -5 , -7 ,  3}, {-1 , -1 , -5 ,  8 ,  5 ,  1 , -8 , -7 , -3 ,  3 , -5 ,  5 , -3 , -2 , -5 , -3 , -2 , -8 , -3 ,  7 ,  6 , -7 , -5 ,  8 , -2 ,  7 , -3 , -6 , -3 ,  6 , -1 ,  6 , -6 , -2 ,  7 , -2 ,  2 , -9 , -4 ,  4 ,  5 , -3}, {-4 ,  6 , -8 , -3 ,  4 ,  1 ,  3 ,  7 ,  6 ,  7 ,  1 ,  3 ,  4 ,  3 ,  5 ,  6 , -7 ,  0 , -2 , -4 ,  4 , -3 , -8 ,  3 ,  6 ,  4 , -3 , -8 , -5 ,  3 , -6 , -4 ,  8 ,  3 ,  3 ,  5 , -1 ,  8 ,  0 , -6 ,  1 ,  5}, { 7 , -2 ,  2 ,  0 , -4 ,  3 , -4 ,  6 ,  5 , -1 ,  5 , -1 , -2 ,  7 ,  2 , -1 , -9 , -7 , -5 ,  8 ,  5 , -2 ,  9 ,  5 , -1 ,  2 , -2 ,  6 , -7 , -4 ,  2 ,  3 , -3 ,  0 , -7 ,  2 , -5 ,  0 ,  2 ,  6 , -9 ,  9}, {-3 ,  8 ,  6 , -2 ,  1 , -3 ,  4 ,  2 ,  7 , -2 , -4 ,  9 ,  5 ,  6 , -6 ,  5 , -7 ,  3 ,  7 ,  2 ,  0 ,  1 , -5 , -3 , -3 , -8 , -6 ,  9 , -5 ,  6 ,  1 ,  3 , -3 , -6 ,  6 ,  7 ,  8 ,  3 ,  8 ,  4 ,  6 , -4}, { 3 ,  8 ,  5 ,  5 , -1 , -6 , -2 ,  2 , -5 ,  0 ,  5 ,  4 ,  6 ,  8 , -3 , -3 ,  3 , -3 ,  2 , -7 , -5 , -1 , -4 , -2 ,  4 ,  7 ,  1 , -2 ,  1 ,  5 ,  6 ,  4 , -1 , -7 , -9 ,  7 , -5 ,  4 , -1 , -7 ,  9 , -3}, { 9 ,  0 , -9 , -5 , -3 , -6 ,  7 , -5 , -8 ,  2 ,  5 , -9 , -5 , -7 ,  6 ,  3 ,  3 , -6 , -7 ,  3 , -7 ,  6 ,  4 ,  1 , -2 ,  4 , -6 ,  6 , -4 , -6 , -4 , -2 ,  0 , -8 ,  8 , -6 ,  6 , -5 , -6 , -4 ,  2 , -8}, { 3 , -3 , -5 ,  0 ,  6 ,  2 ,  2 , -2 ,  3 , -2 ,  7 ,  3 , -5 , -2 ,  4 , -5 ,  0 , -8 , -9 , -4 , -1 ,  9 ,  3 ,  8 , -8 , -3 , -3 ,  7 ,  4 , -7 ,  4 ,  8 , -1 , -6 ,  7 , -3 ,  4 ,  1 ,  9 , -8 , -2 , -4}, {-8 ,  5 ,  6 , -7 ,  0 ,  7 ,  8 ,  9 ,  3 , -7 ,  3 , -6 ,  8 , -5 ,  4 , -2 , -8 , -6 , -3 ,  1 ,  7 ,  8 ,  2 , -1 ,  7 , -1 ,  7 , -2 , -7 ,  0 , -6 ,  1 , -6 , -4 , -5 , -7 , -4 ,  5 ,  8 , -2 ,  1 , -3}, {7 , -3 ,  8 , -6 , -4 , -6 , -5 , -5 ,  0 , -3 ,  7 ,  6 , -3 ,  1 ,  5 ,  4 ,  5 ,  2 ,  0 ,  0 ,  9 , -2 , -2 , -9 , -4 ,  6 ,  6 ,  7 , -3 , -9 ,  6 ,  3 ,  7 , -8 ,  4 , -1 ,  9 , -1 ,  6 , -1 ,  0 , -1}, {-3 ,  7 ,  1 ,  8 ,  0 , -4 , -3 , -8 ,  4 ,  7 , -7 ,  3 ,  0 , -8 , -2 ,  4 ,  2 , -6 ,  7 ,  6 ,  9 ,  6 , -8 , -4 , -3 ,  4 , -7 , -1 ,  3 ,  0 , -9 ,  4 ,  3 ,  5 , -5 , -9 , -6 ,  0 ,  9 ,  8 , -8 , -2}, {-5 ,  7 , -5 , -7 , -4 ,  6 ,  1 ,  2 ,  9 , -6 , -3 , -2 , -6 , -4 ,  5 , -7 , -3 ,  3 , -8 ,  9 ,  8 , -5 ,  3 , -3 , -7 ,  9 ,  0 , -4 , -9 , -8 ,  0 ,  5 ,  8 , -1 , -9 ,  6 ,  7 ,  1 ,  7 ,  6 ,  5 ,  9}, {-4 ,  2 , -4 ,  5 ,  4 , -9 , -5 ,  8 , -2 ,  4 ,  5 , -9 , -1 , -5 ,  7 ,  3 , -9 ,  9 ,  0 , -4 , -9 , -2 , -3 , -3 , -9 ,  8 ,  8 , -8 , -2 ,  1 , -6 ,  3 , -8 ,  2 , -9 , -6 , -2 , -1 , -1 ,  0 ,  4 ,  2}, { 4 , -9 , -3 ,  0 ,  6 , -4 ,  5 , -3 ,  6 , -5 ,  1 , -3 ,  4 ,  7 ,  6 , -3 , -7 , -9 , -2 ,  6 ,  3 ,  9 ,  0 , -2 , -7 ,  7 , -4 ,  2 ,  2 , -1 ,  6 ,  5 , -7 , -8 ,  0 , -5 ,  5 , -4 ,  3 ,  8 ,  1 , -7}, { 1 ,  0 , -8 ,  5 ,  7 ,  8 ,  2 ,  8 ,  5 , -9 ,  2 ,  9 , -4 ,  0 ,  4 , -9 , -2 , -6 ,  1 , -9 , -6 , -5 , -9 , -3 ,  0 ,  5 , -6 ,  1 , -9 ,  3 , -1 , -2 ,  6 ,  6 , -5 , -7 , -7 ,  1 ,  4 ,  8 ,  6 , -3}, {8 , -9 ,  6 , -6 , -1 , -5 , -2 ,  0 , -3 ,  3 , -6 , -6 , -2 , -3 , -1 ,  4 ,  3 ,  0 , -3 ,  3 , -3 ,  7 ,  3 , -1 ,  8 ,  4 ,  3 ,  3 , -7 ,  5 ,  9 ,  7 , -7 , -4 ,  7 ,  0 , -9 , -9 ,  7 ,  0 ,  8 ,  5}, {6 ,  1 , -7 ,  3 , -7 ,  2 , -4 , -8 ,  9 , -4 ,  9 , -2 , -5 ,  5 ,  9 , -2 , -6 , -9 , -9 , -5 ,  5 ,  8 ,  3 , -6 , -6 ,  6 , -1 , -5 , -2 ,  3 , -8 , -9 , -8 ,  6 ,  7 ,  8 ,  2 ,  4 , -8 ,  6 , -5 ,  5}, {-8 , -3 ,  0 , -4 , -1 , -7 ,  1 , -2 ,  4 , -9 ,  7 , -2 ,  9 ,  7 ,  8 ,  8 , -9 ,  3 , -2 ,  5 , -8 ,  5 ,  5 ,  0 ,  0 , -1 ,  2 , -9 ,  7 , -4 , -3 ,  9 , -5 ,  5 ,  4 ,  9 ,  2 ,  4 ,  8 ,  9 ,  0 ,  8}, { 2 ,  6 , -7 , -8 ,  8 ,  5 ,  7 , -5 , -2 , -9 ,  9 , -1 ,  1 , -8 , -4 ,  3 ,  9 , -8 ,  6 , -3 , -9 , -9 , -8 ,  5 , -4 , -1 ,  2 ,  5 ,  4 , -1 , -8 , -6 , -5 , -9 ,  3 , -2 ,  3 , -8 ,  8 ,  1 , -6 , -9}, {8 , -3 , -6 ,  4 ,  2 , -6 ,  1 , -1 , -3 ,  6 , -7 ,  4 ,  9 ,  2 ,  6 ,  3 , -1 ,  0 ,  6 , -2 ,  9 ,  5 ,  7 ,  5 , -2 ,  1 ,  5 ,  7 , -1 , -1 ,  7 , -7 ,  9 ,  4 ,  8 ,  0 , -7 , -2 , -4 ,  7 ,  1 , -1}, {8 , -6 ,  4 , -6 , -6 , -2 , -4 , -5 ,  9 , -8 , -8 , -6 , -5 ,  0 , -8 , -3 ,  1 ,  6 ,  6 , -7 ,  0 ,  6 , -8 , -5 , -2 ,  4 , -2 , -9 , -3 ,  0 ,  5 ,  3 , -1 , -4 ,  8 , -4 , -1 ,  2 ,  9 ,  7 , -8 ,  3}, {-1 ,  5 , -9 ,  7 , -4 ,  2 , -4 , -2 ,  2 ,  1 , -6 , -8 , -2 ,  6 ,  3 , -5 ,  7 , -4 , -6 , -5 , -4 , -9 , -6 , -4 , -1 ,  5 ,  5 , -5 ,  0 ,  6 ,  1 , -4 , -8 , -1 , -7 , -1 ,  4 ,  6 ,  2 ,  0 , -7 ,  8}, {-3 ,  0 ,  9 ,  9 , -5 , -8 ,  2 , -1 ,  6 , -4 ,  3 , -7 ,  7 , -7 ,  7 , -7 , -8 ,  7 ,  5 ,  9 , -4 , -8 ,  0 ,  4 ,  6 ,  1 ,  8 ,  3 ,  0 ,  1 , -5 ,  9 , -2 , -4 , -4 ,  5 ,  1 ,  5 ,  8 , -5 ,  1 ,  8}, {8 , -2 , -2 , -8 ,  1 ,  3 ,  4 ,  8 , -9 , -9 ,  0 , -7 ,  8 ,  6 , -5 ,  5 ,  4 , -8 , -3 , -8 ,  0 , -2 , -9 ,  5 , -3 ,  9 , -3 , -3 ,  2 ,  2 , -1 , -4 ,  0 ,  8 ,  4 , -6 ,  7 ,  6 ,  7 ,  7 , -7 , -5}, {-9 , -3 , -4 , -2 , -3 , -3 ,  8 ,  8 ,  4 , -7 ,  1 , -6 ,  5 , -9 ,  8 ,  6 ,  7 , -2 ,  2 ,  4 ,  5 ,  6 ,  6 , -2 , -3 ,  4 , -2 , -6 , -8 , -2 , -4 ,  8 ,  8 ,  6 ,  6 , -6 , -8 , -5 ,  9 , -4 ,  3 ,  6}, {5 ,  5 , -7 , -1 ,  2 , -4 , -6 , -3 , -6 , -2 , -7 ,  5 ,  3 ,  7 ,  9 ,  2 ,  9 , -7 , -8 , -7 , -8 , -1 ,  1 , -7 , -3 ,  1 ,  0 ,  7 ,  6 ,  6 ,  7 ,  5 ,  1 , -2 , -6 , -7 ,  5 , -4 , -9 , -8 ,  6 , -5}};

const double b[K] = {4, 3, 9, 5, 3, -4, 2, 8, 5, 4, 2, -5, -8, -7, -3, -6, -6, -8, 1, -6, 5, 3, -3, 6, 9, 2, 7, -7, 9, -6, -4, 9, -5, -1, 8, -7, 9, -2, -4, 6, -8, -8}; 

int main(){
    UART_iniciar();
    
    int n = 42;
    
    int16_t x[K];
    int16_t newA[K][K];
    int16_t newb[K];
    for(int i=0;i<K;++i){
	newb[i] = float_i6f10(b[i]);
	for(int j=0;j<K;++j){
	    newA[i][j] = float_i6f10(A[i][j]);
	}
    }
    gaussSolver(n, newA, newb, x);
    

    sprintf(o, "x1 = %f\nx2 = %f\nx3 = %f\nx4 = %f\nx5 = %f\nx6 = %f\nx7 = %f\nx8 = %f\nx9 = %f\nx10 = %f\nx11 = %f\nx12 = %f\nx13 = %f\nx14 = %f\nx15 = %f\nx16 = %f\nx17 = %f\nx18 = %f\nx19 = %f\nx20 = %f\nx21 = %f\nx22 = %f\nx23 = %f\nx24 = %f\nx25 = %f\nx26 = %f\nx27 = %f\nx28 = %f\nx29 = %f\nx30 = %f\nx31 = %f\nx32 = %f\nx33 = %f\nx34 = %f\nx35 = %f\nx36 = %f\nx37 = %f\nx38 = %f\nx39 = %f\nx40 = %f\nx41 = %f\nx42 = %f\n", i6f10_float(x[0]), i6f10_float(x[1]),i6f10_float(x[2]),i6f10_float(x[3]),i6f10_float(x[4]),i6f10_float(x[5]),i6f10_float(x[6]),i6f10_float(x[7]),i6f10_float(x[8]),i6f10_float(x[9]),i6f10_float(x[10]),i6f10_float(x[11]),i6f10_float(x[12]),i6f10_float(x[13]),i6f10_float(x[14]),i6f10_float(x[15]),i6f10_float(x[16]),i6f10_float(x[17]),i6f10_float(x[18]),i6f10_float(x[19]),i6f10_float(x[20]),i6f10_float(x[21]),i6f10_float(x[22]),i6f10_float(x[23]),i6f10_float(x[24]),i6f10_float(x[25]),i6f10_float(x[26]),i6f10_float(x[27]),i6f10_float(x[28]),i6f10_float(x[29]),i6f10_float(x[30]),i6f10_float(x[31]),i6f10_float(x[32]),i6f10_float(x[33]),i6f10_float(x[34]),i6f10_float(x[35]),i6f10_float(x[36]),i6f10_float(x[37]),i6f10_float(x[38]),i6f10_float(x[39]),i6f10_float(x[40]),i6f10_float(x[41]));

    UART_Escrever_Texto(o);
    
  return 0;
}
