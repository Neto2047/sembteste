//GitHub: HenriqueIni
//www.blogcyberini.com

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "uart.h"
#include <xc.h>


void gaussSolver(int n, int A[42][42], double b[42], double X[42]) {
    int i, j, k, l, m; //declaracao de varáveis para serem usadas nos loopings
    //ETAPA DE ESCALONAMENTO: faz a pivotacao e torna a matriz de icognitas 'A' em triangular superior
    for (k = 0; k < n - 1; k++) { //Percorre os valores dos indices da diagonal principal
        double max = fabs(A[k][k]); //max recebe o valor em modulo na linha e coluna de k(diagonal)
        int maxIndex = k; //o indice da diagonal é posto como o atual indice com o maior valor
        /*procura o maior k-ésimo coeficiente em módulo: Nessa interacao sera feita a pivotacao(troca de linhas), caso exista algum
       indice cujo o valor armazenado em modulo seja maior que o valor do atual indice max(na primeira iteracao
         valor maximo atual eh o da diagonal)*/
        for (i = k + 1; i < n; i++) { //Checa as linhas i da coluna k+1(abaixo do pivo)
            if (max < fabs(A[i][k])) { //caso exista algum indice com valor em modulo maior que o atual 'max'
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
                double temp = A[k][j]; 
                A[k][j] = A[maxIndex][j]; 
                A[maxIndex][j] = temp; 
            }
            double temp = b[k]; //realiza o mesmo swap no vetor 'b', para que os termos idependentes continuem referenciando suas respectivas linhas
            b[k] = b[maxIndex]; //swap
            b[maxIndex] = temp; //swap
        }
        //Se A[k][k] é zero, então a matriz dos coeficiente é singular
        //det A = 0
        if (A[k][k] == 0) { //Caso depois de todas as iteracoes anteriores, o indice A[k][k] tiver zero como seu valor 
            //printf("A matriz dos coeficientes e singular\n"); 
            return; //O sistema linear ou eh impossivel ou eh possivel e indeterminado(nao tem solucao unica), logo nao pode ser resolvido.
        } else {
            //realiza o escalonamento: 
            for (m = k + 1; m < n; m++) { //caso A[k][k] seja diferente de 0, é feita as operacoes para tornar a matriz 'A' em uma matriz diagonal superior
                double F = -A[m][k] / A[k][k];
                A[m][k] = 0; //economiza uma iteracao, ja que as operacoes garantem que os valores das linhas da coluna k abixo do pivo serao 0
                b[m] = b[m] + F * b[k]; //realiza as operacoes na linha m do vetor de termos independetes 'b'.
                for (l = k + 1; l < n; l++) { //realiza as operacoes para toda as colunas e linhas da matriz 'A' seguintes ao pivo.
                    A[m][l] = A[m][l] + F * A[k][l];
                }
            }
        }
    }
    /*ETAPA DE RESOLUÇÃO DO SISTEMA: Nessa etapa, primeiro fazemos as operacoes para descobrir a resolucao de x[n](ultima linha da matriz)
	,em seguida, a partir desse resultado, eh feito a resolucao de x[n-1], logo depois x[n-2] e assim por diante ate x[0].(substituicoes retroativas)*/
    for (i = n - 1; i >= 0; i--) {
        X[i] = b[i]; //vetor resolução recebe os novos valores calculados do vetor dos termos independentes
        for (j = i + 1; j < n; j++) { //substituicao retroativa
            X[i] = X[i] - X[j] * A[i][j]; 
        }
        X[i] = X[i] / A[i][i];//a ultima linha do vetor de resolucao tera como resultado o valor da linha ,dividido pela valor do ultimo elemento da matriz(A[n][n])
    }
}
//Código de testes
int A[42][42] = {{-2 , -3 ,  4 , -9 , -5 , -1 ,  1 ,  4 , -2 , -4 , -8 ,  8 ,  5 ,  2 ,  3 ,  8 , -8 , -4 , -9 , -5 ,  9 , -6 , -5 ,  7 ,  6 , -4 , -5 , -3 , -8 , -6 , -2 , -7 ,  2 ,  9 ,  8 , -1 ,  6 , -8 ,  3 , -5 ,  2 ,  1}, { 1 , -5 ,  0 ,  2 ,  9 ,  1 , -3 , -1 , -6 ,  8 , -1 ,  9 , -8 ,  4 ,  6 ,  1 ,  3 , -3 ,  2 ,  4 , -5 ,  6 ,  1 , -4 , -4 , -4 , -3 ,  2 ,  4 , -9 ,  6 ,  0 ,  7 ,  5 ,  2 ,  6 , -1 , -4 ,  3 ,  8 , -5 , -9}, {-6 ,  1 ,  1 ,  6 , -9 , -2 ,  9 , -5 ,  3 ,  3 , -5 ,  0 , -5 ,  9 ,  4 , -5 ,  4 ,  2 ,  0 ,  0 , -2 ,  8 , -2 ,  5 , -8 ,  7 ,  0 ,  9 , -2 , -4 ,  1 ,  9 ,  6 ,  1 , -9 ,  7 , -6 , -7 ,  0 , -6 ,  9 ,  0}, { 8 ,  0 ,  2 ,  8 ,  1 , -7 , -3 , -7 , -3 , -7 ,  5 , -5 ,  3 , -7 , -3 ,  9 , -8 , -3 ,  4 ,  2 , -7 , -5 , -1 ,  6 ,  4 ,  8 , -8 ,  4 , -9 ,  7 , -3 ,  5 ,  6 , -5 , -3 ,  6 ,  9 ,  0 ,  3 , -8 ,  3 , -9}, {-6 , -2 ,  2 ,  2 ,  4 ,  2 , -6 ,  1 ,  5 , -6 , -3 , -4 ,  1 ,  9 ,  2 ,  5 ,  7 , -7 , -3 ,  2 ,  0 , -3 , -6 , -9 , -1 ,  3 , -5 , -1 , -9 ,  6 , -3 , -5 , -7 ,  0 ,  7 ,  3 ,  9 ,  8 ,  9 ,  8 ,  6 ,  6 }, {-2 ,  3 ,  6 ,  9 , -1 ,  1 , -6 , -9 ,  0 ,  7 , -3 ,  1 ,  8 ,  3 , -6 , -6 , -7 ,  0 ,  4 , -1 ,  8 ,  3 ,  2 , -8 ,  7 , -8 ,  9 ,  2 , -9 , -4 ,  9 ,  2 , -5 , -4 , -6 ,  7 ,  5 ,  6 ,  0 , -8 ,  2 ,  9}, { 6 , -2 , -1 , -7 , -1 ,  2 ,  9 , -3 ,  9 , -4 , -2 , -2 ,  9 ,  3 , -7 ,  5 , -1 ,  7 , -6 ,  7 , -5 ,  4 , -1 ,  6 , -8 ,  5 ,  4 , -8 , -2 , -7 ,  9 , -5 ,  7 , -6 ,  2 ,  5 , -8 ,  3 ,  1 ,  1 ,  6 , -5 }, { 7 ,  9 ,  6 ,  8 , -7 ,  1 , -8 ,  8 ,  1 ,  6 ,  4 ,  4 , -3 ,  1 ,  1 , -5 , -9 ,  3 ,  5 , -7 , -4 ,  8 , -8 ,  4 ,  2 ,  8 ,  9 ,  0 , -2 , -1 , -1 ,  7 ,  7 , -7 ,  7 , -5 , -7 , -5 ,  0 , -1 , -4 , -5}, {-5 , -9 , -3 , -5 , -1 , -9 , -3 ,  2 ,  9 , -9 ,  6 ,  0 ,  5 ,  7 , -1 , -6 ,  6 , -6 ,  4 ,  2 , -2 ,  1 ,  3 ,  9 ,  8 ,  6 , -6 , -4 , -1 , -9 , -9 ,  9 , -6 ,  2 ,  1 ,  7 ,  3 , -9 ,  5 , -2 ,  4 ,  3}, {-2 , -9 , -1 , -7 , -4 ,  4 , -2 , -9 ,  3 ,  4 , -8 , -5 , -3 ,  1 ,  9 , -6 ,  3 , -2 , -5 ,  8 , -9 , -7 , -8 ,  4 , -9 , -6 ,  6 , -6 ,  6 , -6 ,  4 ,  2 ,  8 ,  8 ,  8 ,  1 , -1 ,  7 , -8 ,  2 , -8 ,  5}, {-1 ,  4 ,  7 ,  2 , -4 ,  5 ,  6 , -9 ,  4 , -7 ,  7 ,  3 , -6 ,  0 , -3 , -5 , -1 , -4 , -9 , -4 , -3 ,  1 ,  9 , -8 ,  7 , -5 , -4 , -8 , -8 ,  8 , -3 , -9 , -6 ,  9 ,  1 ,  8 , -1 ,  8 ,  6 , -4 ,  3 ,  6 }, { 3 , -8 , -7 ,  9 ,  1 ,  2 ,  0 , -8 ,  1 , -1 ,  8 , -8 , -7 ,  5 ,  9 , -7 ,  6 ,  4 ,  5 , -3 , -9 ,  2 ,  8 , -4 ,  0 ,  0 ,  7 , -4 ,  1 ,  5 , -2 ,  7 ,  8 ,  4 , -2 ,  0 , -1 ,  5 ,  2 , -2 , -3 , -1}, {2 , -6 , -8 , -9 ,  3 , -3 , -9 , -9 ,  2 ,  6 , -7 ,  2 ,  9 ,  2 , -5 , -6 , -9 ,  5 , -4 , -7 ,  8 ,  4 ,  0 ,  8 , -5 ,  8 ,  5 , -4 , -5 , -6 ,  4 ,  5 , -4 ,  7 ,  4 ,  8 ,  1 , -8 ,  1 , -5 ,  2 ,  2}, {9 ,  6 , -5 , -9 , -8 , -6 ,  5 ,  2 ,  3 ,  5 ,  3 , -2 ,  8 ,  8 ,  8 , -8 , -5 ,  2 ,  4 , -6 , -9 , -5 ,  6 , -7 , -4 , -7 ,  9 ,  0 , -5 ,  7 , -1 ,  4 , -9 , -2 ,  5 , -6 , -4 ,  5 ,  4 ,  6 ,  2 , -6}, {3 ,  5 , -5 , -7 , -1 ,  5 , -3 ,  5 , -5 ,  7 ,  9 , -6 , -8 ,  3 , -7 , -9 , -8 , -4 ,  1 , -3 ,  2 , -9 ,  6 , -8 ,  1 , -6 ,  5 ,  1 ,  3 ,  9 ,  3 ,  4 , -2 , -9 ,  7 , -1 , -3 , -9 ,  2 ,  4 ,  7 ,  8}, { 1 ,  4 ,  2 ,  2 ,  8 , -1 ,  8 ,  3 , -6 , -5 ,  7 , -6 ,  5 ,  1 ,  3 , -8 ,  4 ,  4 , -8 ,  0 ,  3 ,  6 ,  4 , -5 , -2 , -9 ,  7 , -2 , -6 ,  1 ,  6 , -5 , -7 ,  6 , -7 , -5 ,  4 ,  3 , -7 ,  7 ,  3 , -9}, { 8 ,  7 ,  8 ,  4 ,  9 ,  2 , -7 ,  0 , -7 , -3 , -4 ,  2 ,  3 , -8 ,  7 ,  5 ,  8 ,  1 , -4 ,  6 ,  1 , -8 , -2 ,  8 , -9 ,  8 ,  6 , -5 ,  9 ,  7 ,  7 ,  7 ,  5 ,  4 , -8 ,  5 , -7 , -6 , -9 , -5 , -7 ,  3}, {-1 , -1 , -5 ,  8 ,  5 ,  1 , -8 , -7 , -3 ,  3 , -5 ,  5 , -3 , -2 , -5 , -3 , -2 , -8 , -3 ,  7 ,  6 , -7 , -5 ,  8 , -2 ,  7 , -3 , -6 , -3 ,  6 , -1 ,  6 , -6 , -2 ,  7 , -2 ,  2 , -9 , -4 ,  4 ,  5 , -3}, {-4 ,  6 , -8 , -3 ,  4 ,  1 ,  3 ,  7 ,  6 ,  7 ,  1 ,  3 ,  4 ,  3 ,  5 ,  6 , -7 ,  0 , -2 , -4 ,  4 , -3 , -8 ,  3 ,  6 ,  4 , -3 , -8 , -5 ,  3 , -6 , -4 ,  8 ,  3 ,  3 ,  5 , -1 ,  8 ,  0 , -6 ,  1 ,  5}, { 7 , -2 ,  2 ,  0 , -4 ,  3 , -4 ,  6 ,  5 , -1 ,  5 , -1 , -2 ,  7 ,  2 , -1 , -9 , -7 , -5 ,  8 ,  5 , -2 ,  9 ,  5 , -1 ,  2 , -2 ,  6 , -7 , -4 ,  2 ,  3 , -3 ,  0 , -7 ,  2 , -5 ,  0 ,  2 ,  6 , -9 ,  9}, {-3 ,  8 ,  6 , -2 ,  1 , -3 ,  4 ,  2 ,  7 , -2 , -4 ,  9 ,  5 ,  6 , -6 ,  5 , -7 ,  3 ,  7 ,  2 ,  0 ,  1 , -5 , -3 , -3 , -8 , -6 ,  9 , -5 ,  6 ,  1 ,  3 , -3 , -6 ,  6 ,  7 ,  8 ,  3 ,  8 ,  4 ,  6 , -4}, { 3 ,  8 ,  5 ,  5 , -1 , -6 , -2 ,  2 , -5 ,  0 ,  5 ,  4 ,  6 ,  8 , -3 , -3 ,  3 , -3 ,  2 , -7 , -5 , -1 , -4 , -2 ,  4 ,  7 ,  1 , -2 ,  1 ,  5 ,  6 ,  4 , -1 , -7 , -9 ,  7 , -5 ,  4 , -1 , -7 ,  9 , -3}, { 9 ,  0 , -9 , -5 , -3 , -6 ,  7 , -5 , -8 ,  2 ,  5 , -9 , -5 , -7 ,  6 ,  3 ,  3 , -6 , -7 ,  3 , -7 ,  6 ,  4 ,  1 , -2 ,  4 , -6 ,  6 , -4 , -6 , -4 , -2 ,  0 , -8 ,  8 , -6 ,  6 , -5 , -6 , -4 ,  2 , -8}, { 3 , -3 , -5 ,  0 ,  6 ,  2 ,  2 , -2 ,  3 , -2 ,  7 ,  3 , -5 , -2 ,  4 , -5 ,  0 , -8 , -9 , -4 , -1 ,  9 ,  3 ,  8 , -8 , -3 , -3 ,  7 ,  4 , -7 ,  4 ,  8 , -1 , -6 ,  7 , -3 ,  4 ,  1 ,  9 , -8 , -2 , -4}, {-8 ,  5 ,  6 , -7 ,  0 ,  7 ,  8 ,  9 ,  3 , -7 ,  3 , -6 ,  8 , -5 ,  4 , -2 , -8 , -6 , -3 ,  1 ,  7 ,  8 ,  2 , -1 ,  7 , -1 ,  7 , -2 , -7 ,  0 , -6 ,  1 , -6 , -4 , -5 , -7 , -4 ,  5 ,  8 , -2 ,  1 , -3}, {7 , -3 ,  8 , -6 , -4 , -6 , -5 , -5 ,  0 , -3 ,  7 ,  6 , -3 ,  1 ,  5 ,  4 ,  5 ,  2 ,  0 ,  0 ,  9 , -2 , -2 , -9 , -4 ,  6 ,  6 ,  7 , -3 , -9 ,  6 ,  3 ,  7 , -8 ,  4 , -1 ,  9 , -1 ,  6 , -1 ,  0 , -1}, {-3 ,  7 ,  1 ,  8 ,  0 , -4 , -3 , -8 ,  4 ,  7 , -7 ,  3 ,  0 , -8 , -2 ,  4 ,  2 , -6 ,  7 ,  6 ,  9 ,  6 , -8 , -4 , -3 ,  4 , -7 , -1 ,  3 ,  0 , -9 ,  4 ,  3 ,  5 , -5 , -9 , -6 ,  0 ,  9 ,  8 , -8 , -2}, {-5 ,  7 , -5 , -7 , -4 ,  6 ,  1 ,  2 ,  9 , -6 , -3 , -2 , -6 , -4 ,  5 , -7 , -3 ,  3 , -8 ,  9 ,  8 , -5 ,  3 , -3 , -7 ,  9 ,  0 , -4 , -9 , -8 ,  0 ,  5 ,  8 , -1 , -9 ,  6 ,  7 ,  1 ,  7 ,  6 ,  5 ,  9}, {-4 ,  2 , -4 ,  5 ,  4 , -9 , -5 ,  8 , -2 ,  4 ,  5 , -9 , -1 , -5 ,  7 ,  3 , -9 ,  9 ,  0 , -4 , -9 , -2 , -3 , -3 , -9 ,  8 ,  8 , -8 , -2 ,  1 , -6 ,  3 , -8 ,  2 , -9 , -6 , -2 , -1 , -1 ,  0 ,  4 ,  2}, { 4 , -9 , -3 ,  0 ,  6 , -4 ,  5 , -3 ,  6 , -5 ,  1 , -3 ,  4 ,  7 ,  6 , -3 , -7 , -9 , -2 ,  6 ,  3 ,  9 ,  0 , -2 , -7 ,  7 , -4 ,  2 ,  2 , -1 ,  6 ,  5 , -7 , -8 ,  0 , -5 ,  5 , -4 ,  3 ,  8 ,  1 , -7}, { 1 ,  0 , -8 ,  5 ,  7 ,  8 ,  2 ,  8 ,  5 , -9 ,  2 ,  9 , -4 ,  0 ,  4 , -9 , -2 , -6 ,  1 , -9 , -6 , -5 , -9 , -3 ,  0 ,  5 , -6 ,  1 , -9 ,  3 , -1 , -2 ,  6 ,  6 , -5 , -7 , -7 ,  1 ,  4 ,  8 ,  6 , -3}, {8 , -9 ,  6 , -6 , -1 , -5 , -2 ,  0 , -3 ,  3 , -6 , -6 , -2 , -3 , -1 ,  4 ,  3 ,  0 , -3 ,  3 , -3 ,  7 ,  3 , -1 ,  8 ,  4 ,  3 ,  3 , -7 ,  5 ,  9 ,  7 , -7 , -4 ,  7 ,  0 , -9 , -9 ,  7 ,  0 ,  8 ,  5}, {6 ,  1 , -7 ,  3 , -7 ,  2 , -4 , -8 ,  9 , -4 ,  9 , -2 , -5 ,  5 ,  9 , -2 , -6 , -9 , -9 , -5 ,  5 ,  8 ,  3 , -6 , -6 ,  6 , -1 , -5 , -2 ,  3 , -8 , -9 , -8 ,  6 ,  7 ,  8 ,  2 ,  4 , -8 ,  6 , -5 ,  5}, {-8 , -3 ,  0 , -4 , -1 , -7 ,  1 , -2 ,  4 , -9 ,  7 , -2 ,  9 ,  7 ,  8 ,  8 , -9 ,  3 , -2 ,  5 , -8 ,  5 ,  5 ,  0 ,  0 , -1 ,  2 , -9 ,  7 , -4 , -3 ,  9 , -5 ,  5 ,  4 ,  9 ,  2 ,  4 ,  8 ,  9 ,  0 ,  8}, { 2 ,  6 , -7 , -8 ,  8 ,  5 ,  7 , -5 , -2 , -9 ,  9 , -1 ,  1 , -8 , -4 ,  3 ,  9 , -8 ,  6 , -3 , -9 , -9 , -8 ,  5 , -4 , -1 ,  2 ,  5 ,  4 , -1 , -8 , -6 , -5 , -9 ,  3 , -2 ,  3 , -8 ,  8 ,  1 , -6 , -9}, {8 , -3 , -6 ,  4 ,  2 , -6 ,  1 , -1 , -3 ,  6 , -7 ,  4 ,  9 ,  2 ,  6 ,  3 , -1 ,  0 ,  6 , -2 ,  9 ,  5 ,  7 ,  5 , -2 ,  1 ,  5 ,  7 , -1 , -1 ,  7 , -7 ,  9 ,  4 ,  8 ,  0 , -7 , -2 , -4 ,  7 ,  1 , -1}, {8 , -6 ,  4 , -6 , -6 , -2 , -4 , -5 ,  9 , -8 , -8 , -6 , -5 ,  0 , -8 , -3 ,  1 ,  6 ,  6 , -7 ,  0 ,  6 , -8 , -5 , -2 ,  4 , -2 , -9 , -3 ,  0 ,  5 ,  3 , -1 , -4 ,  8 , -4 , -1 ,  2 ,  9 ,  7 , -8 ,  3}, {-1 ,  5 , -9 ,  7 , -4 ,  2 , -4 , -2 ,  2 ,  1 , -6 , -8 , -2 ,  6 ,  3 , -5 ,  7 , -4 , -6 , -5 , -4 , -9 , -6 , -4 , -1 ,  5 ,  5 , -5 ,  0 ,  6 ,  1 , -4 , -8 , -1 , -7 , -1 ,  4 ,  6 ,  2 ,  0 , -7 ,  8}, {-3 ,  0 ,  9 ,  9 , -5 , -8 ,  2 , -1 ,  6 , -4 ,  3 , -7 ,  7 , -7 ,  7 , -7 , -8 ,  7 ,  5 ,  9 , -4 , -8 ,  0 ,  4 ,  6 ,  1 ,  8 ,  3 ,  0 ,  1 , -5 ,  9 , -2 , -4 , -4 ,  5 ,  1 ,  5 ,  8 , -5 ,  1 ,  8}, {8 , -2 , -2 , -8 ,  1 ,  3 ,  4 ,  8 , -9 , -9 ,  0 , -7 ,  8 ,  6 , -5 ,  5 ,  4 , -8 , -3 , -8 ,  0 , -2 , -9 ,  5 , -3 ,  9 , -3 , -3 ,  2 ,  2 , -1 , -4 ,  0 ,  8 ,  4 , -6 ,  7 ,  6 ,  7 ,  7 , -7 , -5}, {-9 , -3 , -4 , -2 , -3 , -3 ,  8 ,  8 ,  4 , -7 ,  1 , -6 ,  5 , -9 ,  8 ,  6 ,  7 , -2 ,  2 ,  4 ,  5 ,  6 ,  6 , -2 , -3 ,  4 , -2 , -6 , -8 , -2 , -4 ,  8 ,  8 ,  6 ,  6 , -6 , -8 , -5 ,  9 , -4 ,  3 ,  6}, {5 ,  5 , -7 , -1 ,  2 , -4 , -6 , -3 , -6 , -2 , -7 ,  5 ,  3 ,  7 ,  9 ,  2 ,  9 , -7 , -8 , -7 , -8 , -1 ,  1 , -7 , -3 ,  1 ,  0 ,  7 ,  6 ,  6 ,  7 ,  5 ,  1 , -2 , -6 , -7 ,  5 , -4 , -9 , -8 ,  6 , -5}};
int main(){
    UART_iniciar();
    while(1){
    int n = 42;
    double b[42] = {4, 3, 9, 5, 3, -4, 2, 8, 5, 4, 2, -5, -8, -7, -3, -6, -6, -8, 1, -6, 5, 3, -3, 6, 9, 2, 7, -7, 9, -6, -4, 9, -5, -1, 8, -7, 9, -2, -4, 6, -8, -8};
    double x[42];
    gaussSolver(n, A, b, x);
    UART_Escrever_Texto("Hello!");
    for( n = 0; n < 42; n++)
    {
        char o[15];
        sprintf(o, "x%d = %f\n", n+1, x[n]);
        UART_Escrever_Texto(o);
    }
    }
    //printf("x1 = %f\nx2 = %f\nx3 = %f\nx4 = %f\nx5 = %f\nx6 = %f\nx7 = %f\nx8 = %f\nx9 = %f\nx10 = %f\nx11 = %f\nx12 = %f\nx13 = %f\nx14 = %f\nx15 = %f\nx16 = %f\nx17 = %f\nx18 = %f\nx19 = %f\nx20 = %f\nx21 = %f\nx22 = %f\nx23 = %f\nx24 = %f\nx25 = %f\nx26 = %f\nx27 = %f\nx28 = %f\nx29 = %f\nx30 = %f\nx31 = %f\nx32 = %f\nx33 = %f\nx34 = %f\nx35 = %f\nx36 = %f\nx37 = %f\nx38 = %f\nx39 = %f\nx40 = %f\nx41 = %f\nx42 = %f\n", x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8], x[9], x[10], x[11], x[12], x[13], x[14], x[15], x[16], x[17], x[18], x[19], x[20], x[21], x[22], x[23], x[24], x[25], x[26], x[27], x[28], x[29], x[30], x[31], x[32], x[33], x[34], x[35], x[36], x[37], x[38], x[39], x[40], x[41]);
    return 0;
}