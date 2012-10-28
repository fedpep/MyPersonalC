typedef struct argomenti{
           int *elementi;
           int index; int dim; int **mat; int *det; unsigned long nperm; 
           int sa;
            } argom;

int classe(int elementi[],int dim);
unsigned long fat(int n);
void proded_perm1(argom* arg);
void proded_perm2(argom* arg);
void perc(argom* arg);
int Det(int **mat , int nElementi , int sa);
