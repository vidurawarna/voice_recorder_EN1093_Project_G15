[Signal,Fs] = audioread('Sound.wav');
t = [0:1/Fs:(length(Signal)-1)/Fs];

subplot(4, 1, 1);
plot(t, Signal)%time domain plot of original signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Original Signal Time Spectrum");

subplot(4, 1, 2);
Show_Frequency(Signal, length(Signal), Fs, "Original Signal Frequency Spectrum");%frequency domain plot of original signal

a = input("Enter Scaling Coefficient: ");
at = linspace(0, (length(Signal)-1)/(a*Fs), length(Signal));
fft_coef = abs(fft(Signal)/length(Signal))/abs(a);
f_a = linspace(-length(Signal)/2,length(Signal)/2-1,length(Signal))*Fs*abs(a)/length(Signal);

subplot(4, 1, 3);
plot(at, Signal);%time domain plot of scaled signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Scaled Signal Time Spectrum");

subplot(4, 1, 4);
plot(f_a, fftshift(fft_coef));%frequency domain plot of scaled signal
xlabel("Frequency (Hz)");
ylabel("Amplitude");
title("Scaled Signal Frequency Spectrum");

player = audioplayer(Signal,a*Fs);%play edited .wav file
play(player)
