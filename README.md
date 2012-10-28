MyPersonalC
=============

This is a repository to share some old experimental projects in C.



- det(multithr): This is a small program I developed in 2007 (after attending the operative system course) in order to evaluate the increase of performance due to the multithreading. I begun writing an algorithm that computed the determinant of a square matrix using its definition ( the algorithm has a complexity of O(n!),just a way to give a lot of operations to do to the CPU). Then I tried to separe the computation in two parallel streams opening two different threads to assign the tasks to different CPU cores. I could see the results by evaluating the execution time. You will find the code with italian name for variables and functions... bad things... I was not so experienced and I had no interest in this aspect.

- clserdetbrows: This is another experiment dealing with the posix APIs. It uses the same algorithm used before but implements a web server that manages the requests send via HTTP by a browser and parsing the arriving data in HTML. After running the program it waits for connections requests by a browser (like firefox or chrome) and the starts a thread for each client. Then it sends an HTML page asking to fill a matrix with integer numbers and waits for the response.When it arrives, the new opened thread makes a parse of the HTTP message gathering data and giving it to the previous algorithm.
It was a good way to make some practice with posix under linux using threads and sockets (to access the TCP/IP stack).
I think it works well but, as before, don't expect code refinement because the program was not intended to be delivered in the future.

- filt_modulation: This program performs an AM demodulation. I tried to put into practice some theory of signals and, at the same time, I wanted to try to develope some kind of compressing audio codec...
Using MATLAB i generated a .wav file created by merging two .wav: one of the two was multiplied by a cos(2*pi*f0*t), where f0=10kHz and t is the base time computed by datarate. Then, the two PCM signals were summed togheter and written into a wav file.
The C program performs a real time digital demodulation by multipling for a cos(2*pi*f*t) where f can be decided by the user a runtime and then passes through a low pass filter at 6 kHz of cutoff. This operation rebuilds the two streams in a real time way using a file whoose dimension is equal to just one .wav stream (of course with loss of quality due to the filter).

- genetics: just a program which implements the computation of a minimum of a function using the genetics algorithms.

- I also include the directory "ST". It contains some firmware I developed during my working time there and it is the same you can download on the ST website (in particular: http://www.st.com/internet/evalboard/product/253896.jsp ) so there aren't secret violations because it is public (even if copyright protected). As you will see, this mode of programming is more professional than the experimental projects I described above. The code is more clean and provided with comments in doxygen style to automatically generate docs.
The project also contained the SDK application which make use of the FreeRTOS real time operative system ( http://www.freertos.org/ )  but this program is not available as source code.

I'm actually working on the iNemo M1 platform ( http://www.st.com/internet/analog/product/253162.jsp ) and I'm currently developing firmware. It will become available in Q1 2013 on the website. If you want, you can refer to this old firmware which is similar: http://www.st.com/internet/evalboard/product/250367.jsp (download the GUI and firmware zip file under Design support>SW Functions).
