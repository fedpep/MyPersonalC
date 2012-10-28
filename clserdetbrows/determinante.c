#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include "server.h"
#include "det.h"
#include "httpstr.h"

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

void RichiediMatrice(int dim , char *buf , int sa){
      char str[200];
      register int i,j,k=0;

      sprintf(buf ,"<HTML>\n<BODY>\n<HEAD>\n<TITLE>CALCOLO DEL DETERMINANTE DI UNA MATRICE QUADRATA</TITLE>\n</HEAD>\nInserimento matrice %d x %d\n<FORM METHOD=\"POST\">\n<TABLE>\n", dim, dim);
                                              
                               
      for(i=0 ; i<dim; i++){ 
                           sprintf(str , "<TR>");
                           strcat(buf , str);                  
                           for(j=0 ; j<dim ; j++){
                                                 sprintf(str , "<TD><INPUT type=\"TEXT\" name=\"E%d\" maxlength=\"1\" size=\"1\"></TD>" , k);
                                                 strcat(buf , str);
                                                 k++;
                                                 }
                           sprintf(str , "</TR>\n");
                           strcat(buf , str);                                                                 
                           }
      sprintf(str , "</TABLE>\n<INPUT type=\"SUBMIT\" value=\"OK\">\n</FORM>\n</BODY>\n</HTML>");
      strcat(buf , str);

      stampastr(buf , sa , 1);
}

void CaricaMatrice(int** mat , int dim , char *buf){
    char aux[10];
    register int i,j;
    int offset=0;
    int k=0;

    for(i=0; i < dim; i++)
                      for(j=0; j < dim; j++){ //offset=extint(buf , &mat[i][j] , offset); 
                                             sprintf(aux , "E%d", k);
                                             k++;
                                             findvar(buf , aux , &mat[i][j]);
                                             //printf("\n(%d,%d)=%d", i , j , mat[i][j]);
                                             }
}

void StampaMatrice(int** mat , int dim , char buf[]){
     int i,j;
     char str[20];
     

                    
                    for(i=0 ; i<dim; i++){ 
                                                 sprintf(str , "<TR>");
                                                 strcat(buf , str);                  
                                                 for(j=0 ; j<dim ; j++){
                                                                         sprintf(str , "<TD>%d</TD>" , mat[i][j]); 
                                                                         strcat(buf , str);
                                                                         }
                                                 sprintf(str , "</TR>\n");
                                                 strcat(buf , str);
                                                 }                        
}

void DeallocaMatrice(int **mat , int dim){
     register int i;
     
     for(i=0;i<dim;i++) free(mat[i]);
     free(mat);
}


void determinante(int sa){
           int nElementi;
           int **mat;
           int det;
           char str[4096];
           char buf[30000];
           int close=0;
           
       
       while(!close){   
           printf("\n\n");      
           leggistr(buf , sa);
           printf(buf);
           
           wordstr(buf , 1 , str);     
           

           if(strcmp(str , "GET")==0){
              wordstr(buf , 2 , str);

              if(strcmp(str , "/")==0) strcpy(buf , "<HTML>\n<BODY>\n<HEAD>\n<TITLE>CALCOLO DEL DETERMINANTE DI UNA MATRICE QUADRATA</TITLE>\n</HEAD>\n<FORM METHOD=\"POST\">\nInserire le dimensioni della matrice:<INPUT type=\"TEXT\" name=\"nElementi\" maxlength=\"3\" size=\"3\"><INPUT type=\"SUBMIT\" value=\"OK\">\n</FORM>\n</BODY>\n</HTML>");
              stampastr(buf ,sa , 1); 
                                       
           }
           else{    
              //leggistr(buf , sa);          
              //printf("\n\n");
              //printf(buf);
              //printf("\n\n");
              
              
              //wordstr(buf , 6 , str);
                                               
                                                
               if(findvar(buf , "nElementi" , &nElementi)){

                //extint(str , &nElementi , 0);
              
                
                
                if(nElementi>0){
                               mat=AllocaMatrice(nElementi , nElementi);
                                   
                               RichiediMatrice(nElementi , buf , sa);
                               }  
                else if(nElementi<=0){
                            sprintf(buf , "<HTML>\n<BODY>\n<HEAD>\n<TITLE>CALCOLO DEL DETERMINANTE DI UNA MATRICE QUADRATA</TITLE>\n</HEAD>\nOu, a chi mi pigghi pou culu?\n</BODY>\n</HTML>");
                            stampastr(buf , sa , 0);
                            close=1;
                            }
               }

               else if(findvar(buf , "E0" , NULL)){
                               
                    CaricaMatrice(mat , nElementi , buf);
    
                    det=Det(mat , nElementi, sa);
    
                    sprintf(buf ,"<HTML>\n<BODY>\n<HEAD>\n<TITLE>CALCOLO DEL DETERMINANTE DI UNA MATRICE QUADRATA</TITLE>\n</HEAD>\n<TABLE CELLSPACING=\"18\">\n");                                 
                    StampaMatrice(mat , nElementi , buf);
                    
                    sprintf(str ,"</TABLE>\n<BR>Il determinante e' %d.</BR>\n</BODY>\n</HTML>", det);
                    strcat(buf , str);
                    
                    stampastr(buf , sa , 0);
                    DeallocaMatrice(mat,nElementi);
                    
                    close=1;
                    }
               
           }
           
       }
}  
