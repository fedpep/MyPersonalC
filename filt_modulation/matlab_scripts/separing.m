clear all;

FT=3000000;
data=wavread('merged_bal_can8.wav');
fs=44100;
[DATA,F]=centeredFFT(data,fs);
plot(F,abs(DATA));

t=0:1/fs:(FT-1)/fs;
 d = fdesign.lowpass('N,Fp,Ap', 10, 5500, 1, fs);
 Hd = design(d, 'cheby1');
[b,a]=sos2tf(Hd.sosMatrix,Hd.ScaleValues);
%  d = fdesign.bandpass('N,Fp1,Fp2,Ap', 10, 1, 5000, 1, 44100);
%  designmethods(d);
%  Hd = design(d, 'cheby1');
% fx=filter(Hd,data);
%b = fir1(30,5/44.1);
%a=1;
%a=1.0e+02 *[0.010000000000000  -0.095399512124723   0.411513576558779  -1.056892850032184   1.789687331434189  -2.087748678900376   1.699079787273362  -0.952529583385082   0.352039075939482  -0.077452193576000   0.007703046948241];
%b=1.0e-08 *[0.001180978690242   0.011809786902415   0.053144041060869   0.141717442828985   0.248005524950724   0.297606629940869   0.248005524950724   0.141717442828985   0.053144041060869   0.011809786902415   0.001180978690242];

fx_m(:,1)=data(:,1)'.*cos(2*pi*10000*t);
%fx_m(:,2)=data(:,2)'.*cos(2*pi*18000*t);

fy_m(:,1)=data(:,1)';
%fy_m(:,2)=data(:,2)';

fx(:,1)=filtC(b,a,fx_m(:,1));
%fx(:,1)=fx_m(:,1);
%fx(:,2)=filtC(b,a,fx_m(:,2));
%fx(:,1)=filter(Hd,data(:,1)'.*cos(2*pi*15000*t));
%fx(:,2)=filter(Hd,data(:,2)'.*cos(2*pi*15000*t));
wavwrite(fx,fs,8,'separed_bal.wav');

fy(:,1)=filtC(b,a,fy_m(:,1));
%fy(:,1)=fy_m(:,1);
%fy(:,2)=filtC(b,a,fy_m(:,2));
%fy(:,1)=filter(Hd,data(:,1)'.*cos(2*pi*6000*t));
%fy(:,2)=filter(Hd,data(:,2)'.*cos(2*pi*6000*t));
wavwrite(fy,fs,8,'separed_can.wav');