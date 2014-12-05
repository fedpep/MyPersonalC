MyPersonalC
=============

This is a repository to share some old experimental projects in C.



- det(multithr): This is a small program I developed in 2007 (after attending the operative system course) in order to evaluate the increase of performance due to the multithreading. I've begun writing an algorithm to compute the determinant of a square matrix using its definition ( the algorithm has a complexity of O(n!),just a way to give a lot of operations to do to the CPU). Then I tried to separe the computation in two parallel streams opening two different threads to assign the tasks to different CPU cores. I could see the results by evaluating the execution time. You will find the code with italian name for variables and functions... bad things... I was not so experienced and I had no interest in this aspect.

- clserdetbrows: This is another experiment dealing with the posix APIs. It uses the same algorithm used before but implements a web server that manages the requests sent via HTTP by a browser and parses the arriving HTML formatted data. After running the program it waits for connections requests by a browser (like firefox or chrome) and then starts one thread for each client. Then the client (web browser) sends an HTML page asking to fill a matrix with integer numbers and waits for the response. When it arrives, the new opened thread makes a parsing of the HTTP message, gathering data and giving it to the previous algorithm.
It was a good way to practicize with posix under linux using threads and sockets (to access the TCP/IP stack).
I think it works well but, as before, don't expect code refinement because the program was not intended to be delivered in the future.

- filt_modulation: This program performs an AM demodulation. I tried to put into practice some theory of signals and, at the same time, I wanted to try to develope some kind of compressing audio codec...
Using MATLAB i generated a .wav file created by merging two .wav: one of the two was multiplied by a cos(2*pi*f0*t), where f0=10kHz and t is the base time computed by datarate. Then, the two PCM signals were summed togheter and written into a wav file.
The C program performs a real time digital demodulation by multipling for a cos(2*pi*f*t) where f can be decided by the user a runtime and then passes through a low pass filter at 6 kHz of cutoff. This operation rebuilds the two streams in a real time way using a file whoose dimension is equal to just one .wav stream (of course with loss of quality due to the filter).

- genetics: just a program which implements the computation of a minimum of a function using the genetics algorithms.

