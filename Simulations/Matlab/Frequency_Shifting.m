[Signal,Fs] = audioread('sound.wav');
t = [0:1/Fs:(length(y)-1)/Fs];

subplot(4, 1, 1);
plot(t, Signal);%time domain plot of original signal
subplot(4, 1, 1);
show_fre(Signal, length(Signal), Fs, "Original Signal Frequency Spectrum");%frequency domain plot of original signal

frequency_shift = input("Enter required frequency shift: ");
w0 = 2*pi*frequency_shift;
t = t';
shifted_signal = exp(1i*w0*t).*Signal;

subplot(4, 1, 1);
plot(t, abs(shifted_signal));%time domain plot of shifted signal
subplot(4, 1, 1);
show_fre(shifted_signal, length(shifted_signal), Fs, "Shifted Signal Frequency Spectrum");%frequency domain plot of shifted signal

