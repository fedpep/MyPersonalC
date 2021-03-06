#include <stdio.h> 
#include <stdlib.h> 
#include "det.h"


int** AllocaMatrice(int righe, int colonne){
	int i;
	int **mat;

	mat = (int**)calloc(righe,sizeof(int*));
	if (mat == NULL){
		puts("Spazio insufficiente");
		exit(0);
	}

	for (i=0; i<righe; i++){
		mat[i] = (int*)calloc(colonne,sizeof(int));
		if (mat[i] == NULL){
			puts("Spazio insufficiente");
			exit(0);
		}
	}	
	
	return(mat);
}


void CaricaMatrice(int** mat , int dim){
    register int i,j;

    for(i=0; i < dim; i++)
                      for(j=0; j < dim; j++){
                                        //printf("\nInserisci l'elemento %d %d :", i+1,j+1);
                                        //scanf("%d", &mat[i][j]);
                                        mat[i][j]=rand()%9;
                                        }
}

void StampaMatrice(int** mat , int dim){
     register int i,j;
     
     for(i=0;i<dim;i++){
                        for(j=0;j<dim;j++) printf("%d ", mat[i][j]);
                        printf("\n");
                        }
}

void DeallocaMatrice(int **mat , int dim){
     register int i;
     
     for(i=0;i<dim;i++) free(mat[i]);
     free(mat);
}

int main(){
    int nElementi;
    int **mat;
    long det;
    register int i;
    
    printf("\nCALCOLO DEL DETERMINANTE DI UNA MATRICE QUADRATA\n\n");
    do{  
       printf("\nInserire le dimensioni della matrice (0 per finire) ==> "); 
       scanf("%d", &nElementi);
    
       if(nElementi>0){
                    mat=AllocaMatrice(nElementi , nElementi);
    
                    printf("\nInserimento matrice %d x %d:\n", nElementi, nElementi);
    
                    CaricaMatrice(mat , nElementi);
    
                    printf("\n\n");
		    //printf("\nfat: %u\n", fat(nElementi));
                    StampaMatrice(mat , nElementi);
                   
                    det=Det(mat , nElementi);
    
                    DeallocaMatrice(mat,nElementi);
    
                    printf("\nIl determinante e' %ld.\n\n", det);
                    }
       else if(nElementi<0) printf("\nOu, a chi mi pigghi pou culu?\n");
    }while(nElementi!=0);
    
return 0;
}
    
    
    
    
    
    
    
