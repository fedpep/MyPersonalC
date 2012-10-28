#include<stdio.h>
#include<stdlib.h> 
#include<math.h>

#define N 100
#define N_BIT 14
#define N_GEN 60
#define STEP 0.01
#define PC 0.8
#define PM 1-PC


typedef struct{
	int x[N];
	double f[N];
	double f_sum;
	double f_norm[N];
	double f_norm_max;
	int selection[N];
} Generation;

double f(double x){
	x*=STEP;
	return 10*sin(0.5*x)*cos(3.0*x)*exp(-pow(x-20,2)/600)+10;
}

int mutation(n,b){
	int m;
	int mask=pow(2,b);
	
	m=n^mask;
	
	return m;
}

void crossover(int n1,int n2,int b1,int b2,int *c1,int *c2){
	int mask=0;
	int j;
	
	for(j=b1;j<b2+1;j++)  mask+=pow(2,j);
				
				
	*c1=(n2&(~mask))|(n1&mask);
	*c2=(n1&(~mask))|(n2&mask);
}


int main(){
	int size_x=pow(2,N_BIT);
	int i,k,j1,j2,b1,b2;
	int sel_type;
	double ball,p;
	Generation generations[N_GEN+1];
	double final[N],opt=0;
		
	srand(time(NULL));
		
	generations[0].f_sum=0;
	generations[0].f_norm_max=0;
	for(i=0 ; i<N ; i++){
		generations[0].x[i]=rand()%size_x;
		generations[0].f[i]=f(generations[0].x[i]);
		generations[0].f_sum+=generations[0].f[i];
	}
	for(i=0 ; i<N ; i++){
		generations[0].f_norm[i]=generations[0].f[i]/generations[0].f_sum;
		if(generations[0].f_norm[i]>generations[0].f_norm_max) generations[0].f_norm_max=generations[0].f_norm[i];
	}
	
	
	for(k=0 ; k<N_GEN ; k++){
		generations[k+1].f_sum=0;
		generations[k+1].f_norm_max=0;
		// SELECTION BEGIN
		for(i=0 ; i<N ; i++){
			sel_type=rand()%10;
			if(sel_type<4){//TOURNAMENT
				
				j1=rand()%N;
				j2=rand()%N;
						
				if(generations[k].f[j1]>generations[k].f[j2]){
					generations[k].selection[i]=generations[k].x[j1];
				}else{
					generations[k].selection[i]=generations[k].x[j2];
				}
				
				
			}else{//ROULETTE
				ball=((double)rand() / ((double)(RAND_MAX)+(double)(1)) )*generations[k].f_norm_max;
				
				j1=rand()%N;
				while(ball>generations[k].f_norm[j1]){
				       j1=rand()%N;
				}
			      
				generations[k].selection[i]=generations[k].x[j1];
			}
			
			
			
		}
		// SELECTION END
		
		// REPRODUCTION BEGIN
		for(i=0 ; i<N ; i++){
			
			j1=rand()%N;
			b1=rand()%N_BIT/2;
			p=((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
			
			 if(p<PM){	//MUTATION
				generations[k+1].x[i]=mutation(generations[k].selection[j1],b1);
				generations[k+1].f[i]=f(generations[k+1].x[i]);
				generations[k+1].f_sum+=generations[k+1].f[i];
			}
			else{	//CROSSOVER
				j2=rand()%N;
				p=((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
				b2=b1*(1-p)+(N_BIT-1)*p;
							
				crossover(generations[k].selection[j1],generations[k].selection[j2],b1,b2,&generations[k+1].x[i],&generations[k+1].x[i+1]);
				
				generations[k+1].f[i]=f(generations[k+1].x[i]);
				generations[k+1].f_sum+=generations[k+1].f[i];
				i++;
				generations[k+1].f[i]=f(generations[k+1].x[i]);
				generations[k+1].f_sum+=generations[k+1].f[i];
				
			}
		}
		for(i=0 ; i<N ; i++){
			generations[k+1].f_norm[i]=generations[k+1].f[i]/generations[k+1].f_sum;
			if(generations[k+1].f_norm[i]>generations[k+1].f_norm_max) generations[k+1].f_norm_max=generations[k+1].f_norm[i];
		}
		// REPRODUCTION END
		
	}
	
	for(i=0;i<N;i++){
		final[i]=STEP*generations[k].x[i];
		opt+=final[i];
		//printf("%f\n",final[i]);
	}
	opt/=N;
	
	printf("\nx_opt=%f\nfitness evals=%d\npopulation=%d\ngenerations=%d\n\n",opt,N*N_GEN,N,N_GEN);
	
	return 0;
}

