#include <pthread.h>
#include <stdio.h>
#include "det.h"

#define SLEEP_TIME 8	//ms


unsigned long long fat(int n){
     if(n==1) return 1;
     else return n*fat(n-1);
}

int classe(int elementi[],int dim){
    int k,i;
    int app,clas=0;
    int aux[dim];
    
    for(k=0; k<dim; k++) aux[k]=elementi[k];
    for(k=0; k<dim; k++){
              if(k+1!=aux[k]){
                              app=aux[k];
                              for(i=k ; i<dim; i++)
                              if(k+1==aux[i]){         
                                              aux[k]=aux[i];      
                                              aux[i]=app;
                                              clas++;            
                                              }
                              }
                        } 
    return(clas);
}
  
  


void proded_perm(argom* arg){ 
    int i,k; 
    int app,proded=1;
 
    
    if(arg->index >= 1) {
        
        for(k=arg->index; k >= 0; k--) { 
        app=arg->elementi[k]; 
        arg->elementi[k]=arg->elementi[arg->index]; 
        arg->elementi[arg->index] = app; 
        arg->index--;
        proded_perm(arg); 
        arg->index++;
        app=arg->elementi[k]; 
        arg->elementi[k]=arg->elementi[arg->index]; 
        arg->elementi[arg->index] = app; 
        
        } 
    }
    else if(2*arg->nperm==fat(arg->dim) && arg->dim!=1) pthread_exit(0);
    else {
           
          arg->nperm++;
          
          for(i=0;  i < arg->dim; i++) 
                proded=proded*arg->mat[i][arg->elementi[i]-1];
          if(classe(arg->elementi , arg->dim)%2!=0) proded=-proded;
          *arg->det=*arg->det+proded;
          
          } 
    
          
    
}
    
void perc(argom* arg){
    int sec=0;
    unsigned long long totperm;
    totperm=fat(arg->dim);
	
    printf("\n");
    printf("\r%.2f%%  completato.",200*(float)arg->nperm/totperm);
    while(2*arg->nperm<totperm){
        printf("\r%.2f%% ", 200*(float)arg->nperm/totperm);
	sec++;
	fflush(stdin);
        usleep(SLEEP_TIME*1000);
    }
    printf("\r                     ");
    printf("\r%d sec.",sec*SLEEP_TIME/1000);
}
  
long Det(int **mat , int nElementi) { 
    int elementi1[nElementi],elementi2[nElementi]; 
    int i;
    long det,det1=0,det2=0;
    unsigned long long nperm1=0,nperm2=0;

    pthread_t thread1,thread2,thread3;
    argom arg1,arg2;
   
   
    arg1.elementi=elementi1;
    arg1.index=nElementi-1;
    arg1.dim=nElementi;
    arg1.mat=mat;
    arg1.det=&det1;
    arg1.nperm=nperm1;

    
    arg2.elementi=elementi2;
    arg2.index=nElementi-1;
    arg2.dim=nElementi;
    arg2.mat=mat;
    arg2.det=&det2;
    arg2.nperm=nperm2;
    
    for(i=0; i < nElementi; i++) {
                                  arg1.elementi[i]=i+1;
                                  arg2.elementi[i]=nElementi-i;
                                  //printf("%d %d\n", arg2.elementi[i],arg1.elementi[i]);
                                  }
    
    
    pthread_create(&thread1,NULL,(void*)&proded_perm,(void*)&arg1);
    pthread_create(&thread2,NULL,(void*)&proded_perm,(void*)&arg2);
    pthread_create(&thread3,NULL,(void*)&perc,(void*)&arg2);
    
      
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread3,NULL);
    
    
    det=(*arg1.det)+(*arg2.det);
    if(nElementi==1) det=det/2;

    return(det);
}
