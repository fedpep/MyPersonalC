
typedef struct argomenti{
     int *elementi;
     int index; int dim; int **mat; long *det; unsigned long long nperm;
} argom;



int classe(int elementi[],int dim);
void proded_perm1(argom* arg);
void proded_perm2(argom* arg);
void perc(argom* arg);
long Det(int **mat , int nElementi);
unsigned long long fat(int n);
