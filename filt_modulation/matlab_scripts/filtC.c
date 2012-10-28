#include <stdio.h>
#include "mex.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	int i,j;
	double *b,*a,*x;
	double *fx;
	double prod_a,prod_b;
	int dim_a,dim_b,dim_x;
	
	b = mxGetPr(prhs[0]);
	dim_b=mxGetN(prhs[0]);
	
	a = mxGetPr(prhs[1]);
	dim_a=mxGetN(prhs[1]);
	
	x = mxGetPr(prhs[2]);
	dim_x=mxGetM(prhs[2]);
	
	printf("%d,%d,%d\n",dim_b,dim_a,dim_x);
	
	
	plhs[0] = mxCreateDoubleMatrix(dim_x, 1, mxREAL);
	fx = mxGetPr(plhs[0]);

	for(i=0;i<dim_b-1;i++){
		fx[i]=0;
	}
	for(i=dim_b-1;i<dim_x;i++){
		prod_a=0;
		prod_b=0;
		
		for(j=dim_b-1;j>=0;j--){
			prod_b+=b[j]*x[i-j-1];
		}
		
		for(j=dim_b-1;j>=1;j--){
			prod_a+=a[j]*fx[i-j];
		}
		
		fx[i]=-prod_a+prod_b;
	}
	
}
