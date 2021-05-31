[Signal,Fs] = audioread('Sound.wav');
t = [0:1/Fs:(length(Signal)-1)/Fs];

subplot(4, 1, 1);
plot(t, Signal);%time domain plot of original signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Original Signal Time Spectrum");

subplot(4, 1, 2);
Show_Frequency(Signal, length(Signal), Fs, "Original Signal Frequency Spectrum");%frequency domain plot of original signal

frequency_shift = 500;
w0 = 2*pi*frequency_shift;
t = t';
modulated_signal = cos(w0*t).*Signal;
shifted_signal = High_Pass_Signal_Convol(modulated_signal);


subplot(4, 1, 3);
plot(t, abs(shifted_signal));%time domain plot of shifted signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Shifted Signal Time Spectrum");

subplot(4, 1, 4);
Show_Frequency(shifted_signal, length(shifted_signal), Fs, "Shifted Signal Frequency Spectrum");%frequency domain plot of shifted signal

player = audioplayer(shifted_signal,Fs);%play edited .wav file
play(player)