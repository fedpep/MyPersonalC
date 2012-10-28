clear all;


x=wavread('canto_servo_pastore.wav');
x=x(1:3000000,:);
y=wavread('ballata_amore_cieco.wav');
y=y(1:3000000,:);

fs=44100;
ORD=10;
FT=3000000;
   d = fdesign.lowpass('N,Fp,Ap', ORD, 5500, 1, fs);
  Hd = design(d, 'cheby1');
  [b,a]=sos2tf(Hd.sosMatrix,Hd.ScaleValues);

%a=1;
%b = fir1(30,5/44.1);
%a=1.0e+02 *[0.010000000000000  -0.095399512124723   0.411513576558779  -1.056892850032184   1.789687331434189  -2.087748678900376   1.699079787273362  -0.952529583385082   0.352039075939482  -0.077452193576000   0.007703046948241];
%b=1.0e-08 *[0.001180978690242   0.011809786902415   0.053144041060869   0.141717442828985   0.248005524950724   0.297606629940869   0.248005524950724   0.141717442828985   0.053144041060869   0.011809786902415   0.001180978690242];

fx=filter(b,a,x);
fy=filter(b,a,y);
%fx(:,1)=filtC(b,a,x(:,1));
%fx(:,2)=filtC(b,a,x(:,2));
%fy(:,1)=filtC(b,a,y(:,1));
%fy(:,2)=filtC(b,a,y(:,2));
%fx=filter(Hd,x);
%fy=filter(Hd,y);
% 
%   fx(1:ORD,1)=zeros();
%   fx(1:ORD,2)=zeros();
%   fy(1:ORD,1)=zeros();
%   fy(1:ORD,2)=zeros();
%   for i=ORD+1:1:FT
%      fx(i,1)= b*x(i-ORD:i,1)-fliplr(a(2:ORD+1))*fx(i-ORD:i-1,1);
%      fx(i,2)= b*x(i-ORD:i,2)-fliplr(a(2:ORD+1))*fx(i-ORD:i-1,2);
%      fy(i,1)= b*y(i-ORD:i,1)-fliplr(a(2:ORD+1))*fy(i-ORD:i-1,1);
%      fy(i,2)= b*y(i-ORD:i,2)-fliplr(a(2:ORD+1))*fy(i-ORD:i-1,2);
%      
%      if(mod(i,1000)==0)
%          fprintf('%f\r',i/FT*100);
%      end
%   end

%[X_f,F]=centeredFFT(fx,fs);
%[Y_f,F]=centeredFFT(fy,fs);

t=0:1/fs:(FT-1)/fs;
fx_m(:,1)=fx(:,1)'.*cos(2*pi*10000*t);
fx_m(:,2)=fx(:,2)'.*cos(2*pi*10000*t);

fy_m(:,1)=fy(:,1)';%.*cos(2*pi*6000*t);
fy_m(:,2)=fy(:,2)';%.*cos(2*pi*6000*t);
% [X_f_m,F]=centeredFFT(fx_m,fs);
% [Y_f_m,F]=centeredFFT(fy_m,fs);
% 
% plot(F,abs(X_f_m));
% hold on;
% plot(F,abs(Y_f_m),'r');

s=fx_m+fy_m;

wavwrite(s(:,1),fs,8,'merged_bal_can8.wav');
