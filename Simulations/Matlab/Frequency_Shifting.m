[Signal,Fs] = audioread('sound.wav');
t = [0:1/Fs:(length(Signal)-1)/Fs];

subplot(4, 1, 1);
plot(t, Signal);%time domain plot of original signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Original Signal Time Spectrum");

subplot(4, 1, 2);
show_fre(Signal, length(Signal), Fs, "Original Signal Frequency Spectrum");%frequency domain plot of original signal

frequency_shift = input("Enter required frequency shift(Hz): ");
w0 = 2*pi*frequency_shift;
t = t';
modulated_signal = cos(w0*t).*Signal;
shifted_signal = High_Pass_Signal(modulated_signal, frequency_shift, Fs);


subplot(4, 1, 3);
plot(t, abs(shifted_signal));%time domain plot of shifted signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Shifted Signal Time Spectrum");

subplot(4, 1, 4);
show_fre(shifted_signal, length(shifted_signal), Fs, "Shifted Signal Frequency Spectrum");%frequency domain plot of shifted signal

player = audioplayer(shifted_signal,Fs);%play edited .wav file
play(player)