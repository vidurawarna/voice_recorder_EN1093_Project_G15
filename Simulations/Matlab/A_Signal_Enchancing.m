[Signal,Fs] = audioread('Sound.wav');
t = [0:1/Fs:(length(Signal)-1)/Fs];

subplot(4, 1, 1);
plot(t, Signal)%time domain plot of original signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Original Signal Time Spectrum");

subplot(4, 1, 2);
Show_Frequency(Signal, length(Signal), Fs, "Original Signal Frequency Spectrum");%frequency domain plot of original signal

Typ_Fre = input("Type of filter(High_Pass(H-500)/Band_Pass(B-300-500)/Low_Pass(L-300)): ",'s');

Enchance_coeff = input("Input Enchancing Coefficient: ");

%enchancing components of the signal
if Typ_Fre == 'H'
    Signal_high = High_Pass_Signal_Convol(Signal);
    Signal_high = Signal_high.*Enchance_coeff;
    Signal_low = Low_Pass_Signal_Convol(Signal);
    New_Signal = Signal_high + Signal_low;
elseif Typ_Fre == 'B'
    Signal_high = High_Pass_Signal_Convol(Signal);
    Signal_band = Band_Pass_Signal_Convol(Signal);
    Signal_band = Signal_band.*Enchance_coeff;
    Signal_low = Low_Pass_Signal_Convol(Signal);
    New_Signal = Signal_high + Signal_band + Signal_low;
else
    Signal_high = High_Pass_Signal_Convol(Signal);
    Signal_low = Low_Pass_Signal_Convol(Signal);
    Signal_low = Signal_low.*Enchance_coeff;
    New_Signal = Signal_high + Signal_low;
end

subplot(4, 1, 3);
plot(t, New_Signal)%time domain plot of enchanced signal
xlabel("Time (s)");
ylabel("Amplitude");
title("Enchanced Signal Time Spectrum");

subplot(4, 1, 4);
Show_Frequency(New_Signal, length(New_Signal), Fs, "Enchanced Signal Frequency Spectrum");%frequency domain plot of enchanced signal

player = audioplayer(New_Signal,Fs);%play edited .wav file
play(player)