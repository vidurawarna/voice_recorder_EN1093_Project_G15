[Signal,Fs] = audioread('sound.wav');
t = [0:1/Fs:(length(Signal)-1)/Fs];

subplot(4, 1, 1);
plot(t, Signal)%time domain plot of original signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Original Signal Time Spectrum");

subplot(4, 1, 2);
show_fre(Signal, length(Signal), Fs, "Original Signal Frequency Spectrum");%frequency domain plot of original signal

Typ_Fre = input("Type of filter(High_Pass(H)/Band_Pass(B)/Low_Pass(L)): ",'s');
Freq_range = input("Input frequency or frequency range which need Enchancing(Hz): ");
Enchance_coeff = input("Input Enchancing Coefficient: ");

%enchancing components of the signal
if Typ_Fre == 'H'
    Signal_high = High_Pass_Signal(Signal, Freq_range, Fs);
    Signal_high = Signal_high.*Enchance_coeff;
    Signal_low = Low_Pass_Signal(Signal, Freq_range, Fs);
    New_Signal = Signal_high + Signal_low;
elseif Typ_Fre == 'B'
    Signal_high = High_Pass_Signal(Signal, Freq_range(2), Fs);
    Signal_band = Band_Pass_Signal(Signal, Freq_range(1), Freq_range(1), Fs);
    Signal_band = Signal_band.*Enchance_coeff;
    Signal_low = Low_Pass_Signal(Signal, Freq_range(1), Fs);
    New_Signal = Signal_high + Signal_band + Signal_low;
else
    Signal_high = High_Pass_Signal(Signal, Freq_range, Fs);
    Signal_low = Low_Pass_Signal(Signal, Freq_range, Fs);
    Signal_low = Signal_low.*Enchance_coeff;
    New_Signal = Signal_high + Signal_low;
end

subplot(4, 1, 3);
plot(t, New_Signal)%time domain plot of enchanced signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Enchanced Signal Time Spectrum");

subplot(4, 1, 4);
show_fre(New_Signal, length(New_Signal), Fs, "Enchanced Signal Frequency Spectrum");%frequency domain plot of enchanced signal

player = audioplayer(New_Signal,Fs);%play edited .wav file
play(player)