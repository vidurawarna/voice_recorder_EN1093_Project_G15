function [Y] = High_Pass_Signal_Convol(Original_Signal)
    H = [-0.000000000000000000250712482816235203279
     0.00027579742923587301662441406691073098 
     0.000629655163845921220580614807715846837
     0.00111215901386648645214616504262039598 
     0.00175181194539987669309133444528470136 
     0.002540381272953769805494905753562306927
     0.003421745808272406378508501134660946263
     0.004286277446373946281577005379403999541
     0.004972209878053889149651656254036424798
     0.005274639522224172996645030764284456382
     0.004961862795504023808390581251615003566
     0.003797792372811997340525946853517780255
     0.001568332907814113134567501894878205349
    -0.001891054398631433503680243646272174374
    -0.006669719280471254028996863638667491614
    -0.012761236551122604604424637386728136335
    -0.020050604628718261129849409485359501559
    -0.028311249198766697682261295199168671388
    -0.037212868250374529777158016941029927693
    -0.046340009289287852067840134395737550221
    -0.055220091816489506875154091858348692767
    -0.063358513725292317508852590890455758199
    -0.070277635465248483592759498606028500944
    -0.075555922538444217351205622890120139346
    -0.078863410069501926891177845391212031245
     0.919884455264330158641428170085418969393
    -0.078863410069501926891177845391212031245
    -0.075555922538444217351205622890120139346
    -0.070277635465248483592759498606028500944
    -0.063358513725292317508852590890455758199
    -0.055220091816489506875154091858348692767
    -0.046340009289287852067840134395737550221
    -0.037212868250374529777158016941029927693
    -0.028311249198766697682261295199168671388
    -0.020050604628718261129849409485359501559
    -0.012761236551122604604424637386728136335
    -0.006669719280471254028996863638667491614
    -0.001891054398631433503680243646272174374
     0.001568332907814113134567501894878205349
     0.003797792372811997340525946853517780255
     0.004961862795504023808390581251615003566
     0.005274639522224172996645030764284456382
     0.004972209878053889149651656254036424798
     0.004286277446373946281577005379403999541
     0.003421745808272406378508501134660946263
     0.002540381272953769805494905753562306927
     0.00175181194539987669309133444528470136 
     0.00111215901386648645214616504262039598 
     0.000629655163845921220580614807715846837
     0.00027579742923587301662441406691073098 
    -0.000000000000000000250712482816235203279];
    
    %Convolution Algorithm
    X = Original_Signal;
    m = size(X);
    m = m(1,1);
    n = size(H);
    n = n(1,1);
    Y = zeros(m,1);
    for i = n+1:m
        Y(i)=0;
        for j = 1:n
            Y(i) = Y(i) + X(i-j)*H(j);
        end
    end
end