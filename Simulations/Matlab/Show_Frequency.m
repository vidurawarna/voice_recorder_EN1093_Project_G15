function [] = Show_Frequency(signal, l, Fs, Title)
    f = (-l/2:l/2-1)*(Fs/l);
    fft_vals = fftshift(fft(signal)/l);
    
    plot(f, abs(fft_vals));
    grid();
    xlabel("Frequency (Hz)");
    ylabel("Amplitude");
    title(Title);
end
