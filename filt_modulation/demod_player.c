#include <alsa/asoundlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define F_ORD 10

char* 	path;
int 	chunk_size;
int	subchunk_size;
short 	format;
short 	channels;
int   	sample_rate;
int   	byte_rate;
short 	block_align;
short 	bits_per_sample;
int	data_size;

unsigned char *data;
double *fdata;
unsigned int *x;

char *device = "default";
snd_output_t *output = NULL;

int fm;

double a[]={0.010000000000000,  -0.078393428001590,   0.288774700868020,  -0.655991300356510,   1.015389651389734,  -1.117379273018170,   0.884544166823302,  -0.497209314429208,  0.189949430282953,  -0.044562863221653,   0.004881218540060};
double b[]={0.002601405638911,   0.026014056389113,   0.117063253751011,   0.312168676669361,   0.546295184171382,   0.655554221005659,   0.546295184171382,   0.312168676669361,   0.117063253751011,   0.026014056389113,   0.002601405638911};
//double a[]={1.000000000000000,  -4.235874693657500,  10.562514039169676, -18.299700118656716,  23.902605725650819, -24.221901982408603,  19.185109718114241,  -11.724052392394618,   5.320968853586267,  -1.641960546821811,   0.272006418791551};
//double b[]={0.000104195434675,   0.001041954346754,   0.004688794560395,   0.012503452161054,   0.021881041281844,   0.026257249538212,   0.021881041281844,  0.012503452161054,   0.004688794560395,   0.001041954346754,   0.000104195434675};
	
void init_filter_params(void){
  int i;		
		
  for(i=0;i<F_ORD+1;i++){
    a[i]*=100.0;
    b[i]*=0.0001;
  }
       
}

void filtC(unsigned char *x, double *fx, int i){		
  double prod_a=0.0,
    prod_b=0.0;
  int j;

  if(i<F_ORD){
    fx[i]=0.0;
  }
  else{
    for(j=F_ORD;j>=0;j--){
      prod_b+=b[j]*x[i-j-1];
    }
  
    for(j=F_ORD;j>=1;j--){
      prod_a+=a[j]*fx[i-j];
    }
    
    fx[i]=-prod_a+prod_b;
    //printf("%f",fx[i]);
  }
		
}


void read_wav(void){
  FILE *fp;
  
    
  fp=fopen(path, "rb");
  
  //printf("Reading wav file...\n"); // for debugging only
  
	
  fseek(fp,4,0);
  fread(&chunk_size, sizeof(int),1,fp); // read the chunk_size
  
  fseek(fp,16,0);
  fread(&subchunk_size, sizeof(int),1,fp); // read the Subchunk_size
  
  fseek(fp,20,0);
  fread(&format, sizeof(short),1,fp); // read the file format.  This should be 1 for PCM
  
  fseek(fp,22, 0);
  fread(&channels, sizeof(short),1,fp); // read the # of channels (1 or 2)
  
  fseek(fp,24, 0);
  fread(&sample_rate, sizeof(int),1,fp); // read the sample_rate
  
  fseek(fp,28, 0);
  fread(&byte_rate, sizeof(int),1,fp); // read the byte_rate
  
  fseek(fp,32, 0);
  fread(&block_align, sizeof(short),1,fp); // read the block_align
  
  fseek(fp,34, 0);
  fread(&bits_per_sample, sizeof(short),1,fp); // read the bits_per_sample
  
  fseek(fp,40, 0);
  fread(&data_size, sizeof(int),1,fp); // read the size of the data

  
  fseek(fp,40,0);
  fread(&data_size, sizeof(int),1,fp); // read the size of the data
  
  data = (char*)malloc(sizeof(char)*data_size);
  fseek(fp,44,0);
  fread(data, sizeof(unsigned char),data_size,fp);		// read the data chunk
  
  fclose(fp); // close the input file
	       		
}

void save_wav(char* outpath){
  FILE *fp;
  
  fp=fopen(outpath, "wb+");
  
  // write the wav file per the wav file format
  fwrite ("RIFF",sizeof(char),4,fp);
  fwrite (&chunk_size, sizeof(char),4,fp);
  fwrite ("WAVE",sizeof(char),4,fp);
  fwrite ("fmt ",sizeof(char),4,fp);
  fwrite (&subchunk_size,sizeof(char),4,fp);
  fwrite (&format,sizeof(char),2,fp);
  fwrite (&channels,sizeof(char),2,fp);
  fwrite (&sample_rate,sizeof(char),4,fp);
  fwrite (&byte_rate,sizeof(char),4,fp);
  fwrite (&block_align,sizeof(char),2,fp);
  fwrite (&bits_per_sample,sizeof(char),2,fp);
  fwrite ("data",sizeof(char),4,fp);
  fwrite (&data_size,sizeof(char),4,fp);
  fwrite (x,sizeof(char),data_size,fp);

  fclose(fp);
}

void getSummary(void){
		printf(" format: %d\n channels: %d\n sample_rate: %d\n byte_rate: %d\n block_align: %d\n bits_per_sample: %d\n data_size: %d\n", format, channels, sample_rate, byte_rate, block_align, bits_per_sample, data_size);
}

void listener(void){
  while(1){
    printf("fm: ");
    scanf("%d", &fm);
  }
}

int main( int argc, char *argv[] ){
	int err;
        unsigned int i,k=0;
        snd_pcm_t *handle;
        snd_pcm_sframes_t frames;
	char outpath[51];
	double t,ts;	
	pthread_t th_keyb;

	if (argc < 3) {
		printf("usage: %s <filename> <fm>\n",argv[0]);
		return 0;
	}
	path=argv[1];
	fm=atoi(argv[2]);
	
	read_wav();
	getSummary();
	init_filter_params();

	pthread_create((void*) &th_keyb, NULL , (void*) &listener, NULL);
	
	
	if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = snd_pcm_set_params(handle,SND_PCM_FORMAT_U8,SND_PCM_ACCESS_RW_INTERLEAVED,1,44100,1,0)) < 0) {
			printf("Playback open error: %s\n", snd_strerror(err));
			exit(EXIT_FAILURE);
	}

	x= (unsigned int*)malloc(sizeof(unsigned int)*(data_size>>2));
	fdata= (double*)malloc(sizeof(double)*data_size);

	ts=1.0/sample_rate;
	x[0]=0;

	for (i = 0; i <data_size; i++){
	  
	  data[i]=(unsigned char)(128.0+(double)(data[i]-128)*cos(2*M_PI*fm*t));
	  t+=ts;

	  filtC(data,fdata,i);
	  
	  x[k]+=((unsigned int)(fdata[i]))<<(8*(i%4));
	    
		  
	  if(i!=0 && i%4==0){
	    frames = snd_pcm_writei(handle,&x[k],4);  
	    k++;
	    x[k]=0;
	  }

		
	  if (frames < 4){
	    frames = snd_pcm_recover(handle, frames, 1);
	    //printf("recov\n");
	  }
	  
	}
	snd_pcm_close(handle);
	
	free(data);
	free(fdata);
	free(x);

	printf("\n");
	return 0;
}
