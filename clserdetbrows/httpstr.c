#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int strpiece(char str[] , int start , int offset , char piece[]){
                 int i,j;

                 for(i=start , j=0 ; i<start+offset ; i++ , j++){
                                                             piece[j]=str[i];
                                                             if(piece[j]=='\0') return 1;
                                                             }
                 return 0;

}

int wordstr(char str[], int n , char word[] ){
           int i,j=0,k=0;
                        
           for(i=0 ; k!=n && str[i]!='\0' ; i++){
                                 if(str[i]==' ' || str[i]=='\n' || str[i]=='=' || str[i]=='&'){
                                                 k++;
                                                 if(k!=n) j=0;
                                                 }
                                 else{
                                      word[j]=str[i];
                                      j++;
                                      }
           }                          
           
           
           word[j]='\0';
           if(str[i]=='\0') return 1;
           else return 0;

}  

int findvar(char str[] , char word[] , int *n){
          char aux[200];
          int i,end=0;
          
        
          for(i=1 ; i==1 || strcmp(aux , word)!=0 && !end ; i++){
                                                         end=wordstr(str , i , aux);
                                                         /*printf(aux);
                                                         printf("\n");*/ 
                                                        }
          if(strcmp(aux , word)==0){
                           //printf("\nTROVATO: ");
                           //printf(aux);
                           wordstr(str , i , aux);
                           if(n!=NULL) *n=atoi(aux);
                           return 1;
          }
          else{
               //printf("\nNON TROVATO: %s\n", word);
               return 0;
               }
}     


/*int extint(char str[] , int *n , int offset){
     char app[5];   
     int i,j,k;

          for(i=offset ; str[i]!='=' ; i++);
          for(j=i+1,k=0; str[j]!='\0' && str[j]!='&' ; j++ , k++) app[k]=str[j];
                                    
     app[k]='\0';
     
     *n=atoi(app);
     
     return(j);
}*/

