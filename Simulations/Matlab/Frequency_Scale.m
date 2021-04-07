[Signal,Fs] = audioread('sound.wav');
t = [0:1/Fs:(length(y)-1)/Fs];

subplot(4, 1, 1);
plot(t, Signal)%time domain plot of original signal
subplot(4, 1, 2);
show_fre(Signal, length(Signal), Fs, "Original Signal Frequency Spectrum");%frequency domain plot of original signal

a = input("Enter Scaling Coefficient: ");
at = linspace(0, (length(Signal)-1)/(a*Fs), length(Signal));
fft_coef = abs(fft(Signal))/abs(a);
f_a = linspace(-length(Signal)/(2*a),(length(Signal)-1)/(2*a),length(Signal));

subplot(4, 1, 3);
plot(at, Signal);%time domain plot of scaled signal
subplot(4, 1, 4);
plot(f/a, fftshift(fft_coef));%frequency domain plot of scaled signal

