function [y] = Band_Pass_Signal(Original_Signal, CutF1, CutF2, Fs)
    M = 801;
    % Low Pass Filter Start
    CutF1 = CutF1/Fs;
    
    H1 = zeros(M,1);
    
    for I = 1:M%finding filter coeffecients
        if (I-M/2)== 0
            H1(I) = 2*pi*CutF1;
        else
            H1(I) = sin(2*pi*CutF1*(I-M/2))/(I-M/2);
        end
        H1(I) = H1(I)*(0.54-0.46*cos(2*pi*I/M));
    end
    
    Sum = 0;%normalization
    for I = 1:M
        Sum = Sum + H1(I);
    end
    
    for I = 1:M
        H1(I) = H1(I)/Sum;
    end
    % Low Pass Filter End
    
    %High Pass Filter Start
    CutF2 = CutF2/Fs;
    
    H2 = zeros(M,1);
    
    for I = 1:M%finding filter coeffecients
        if (I-M/2)== 0
            H2(I) = 2*pi*CutF2;
        else
            H2(I) = sin(2*pi*CutF2*(I-M/2))/(I-M/2);
        end
        H2(I) = H2(I)*(0.54-0.46*cos(2*pi*I/M));
    end
    
    Sum = 0;%normalization
    for I = 1:M
        Sum = Sum + H2(I);
    end
    
    for I = 1:M
        H2(I) = H2(I)/Sum;
    end
    for I = 1:M% convert low pass filter coefficients into high pass filter coefficients
        H2(I) = -H2(I);
    end
    H2(400) = H2(400) + 1;
    %High Pass Filter End
    
    %Band Stop Filter 
    H = zeros(M,1);
    
    for I = 1:M
        H(I) = H1(I) + H2(I);
    end
    
    %Band Pass Filter
    for I = 1:M
        H(I) = -H(I);    
    end
    H(400) = H(400) + 1;
    
    y = conv(Original_Signal,H,'same');%convolution

    fileID = fopen('Band_Pass_Coeff.txt','w');
    Content = fprintf(fileID, '%f\n' , H);
    fclose(fileID);
    
end