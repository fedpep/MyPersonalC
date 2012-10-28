clear all
[data,fs]=wavread('kida1.wav');

Tmax=25;
dT=0.2;
Kmax=Tmax/dT;

dim=size(data,1);
dt=1/fs;

a=zeros(1,Kmax);

j=0;
for k=0:1:Kmax-1  
    j=ceil(dT*fs*k);
    for i=1:1:dim-j
        a(k+1)=a(k+1)+(data(i,1)+data(i,2))/2*(data(j+i,1)+data(j+i,2))/2*dt;
    end

end
t=0:dT:Tmax;
t(end)=[];
plot(t,a);