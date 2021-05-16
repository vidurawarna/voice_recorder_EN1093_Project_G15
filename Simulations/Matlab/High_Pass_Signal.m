function [y] = High_Pass_Signal(Original_Signal, CutF, Fs)
    M = 801;
    
    CutF = CutF/Fs;
    
    H = zeros(M,1);
    
    for I = 1:M%finding filter coeffecients
        if (I-M/2)== 0
            H(I) = 2*pi*CutF;
        else
            H(I) = sin(2*pi*CutF*(I-M/2))/(I-M/2);
        end
        H(I) = H(I)*(0.54-0.46*cos(2*pi*I/M));
    end
    
    Sum = 0;%normalization
    for I = 1:M
        Sum = Sum + H(I);
    end
    
    for I = 1:M
        H(I) = H(I)/Sum;
    end
    
    for I = 1:M% convert low pass filter coefficients into high pass filter coefficients
        H(I) = -H(I);
    end
    H(400) = H(400) + 1;
    
    y = conv(Original_Signal,H,'same');%convolution
    
end