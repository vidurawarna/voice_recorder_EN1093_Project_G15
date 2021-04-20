close all;
%Define number of samples to take
fs = 8000;
f = 400; %Hz
%Define signal
t = 0:1/fs:1-1/fs;
signal1 = sin(2*pi*f*t);
signal2 = sin(pi*f*t);
%Plot to illustrate that it is a sine wave
%plot(t, signal);
%title('Time-Domain signal');
%Take fourier transform
fftSignal1 = fft(signal1);
fftSignal2 = fft(signal2);
%apply fftshift to put it in the form we are used to (see documentation)
%fftSignal1 = fftshift(fftSignal1);
%fftSignal2 = fftshift(fftSignal2);
%Next, calculate the frequency axis, which is defined by the sampling rate
f = (fs/2)*linspace(-1,1,fs);
%Since the signal is complex, we need to plot the magnitude to get it to
%look right, so we use abs (absolute value)
figure;
%plot(t,signal);
plot(f, abs(fftSignal1));
hold on
plot(f, abs(fftSignal2));
title('magnitude FFT of sine');
xlabel('Frequency (Hz)');
ylabel('magnitude');