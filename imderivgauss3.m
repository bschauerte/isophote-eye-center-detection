function [Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss3(I,sigma,conv_shape,show_kernels)
    % IMDERIVGAUSS2 calculates the 1st and 2nd order partial image 
    %   derivatives. It uses Gaussian smoothing to provide better 
    %   derivatives.
    %
    % This implementation relies on 'conv2pad' and specifically determined
    % image derivative filters (i.e., differentiation via convolution).
    %
    % This implementation allows to specify the convolution shape parameter
    % (see 'conv2' and 'conv2pad'/'conv2wrap').
    %
    % @author B. Schauerte
    % @date   2011  
    
    % parameters
    if nargin < 2
        sigma = 1;
    end
    
    if nargin < 3
        conv_shape = 'same';
    end
    
    if nargin < 4
        show_kernels = false;
    end
    
    % determine necessary filter support (for Gaussian)
    %   %Wx = floor((5/2)*sigma * 2); % Wx = floor((5/2)*sigma); % @todo: is the support high enough? How to calculate the support for higher order derivatives?
    %   %Wx = ceil(sigma*3)*2+1; % taken from matlab LoG code (http://www.mathworks.com/help/toolbox/images/ref/edge.html)
    %   Wx = ceil(sigma*3); % taken from matlab LoG code (http://www.mathworks.com/help/toolbox/images/ref/edge.html)
    %     % Note that the selected width has a measurable influence on the performance
    %   if Wx < 1
    %      Wx = 1;
    %   end
    %   x = [-Wx:Wx];
    [x,Wx]=imderivgauss3_get_support(sigma);
    
    % new forumalas for 1st and 2nd derivatives (the ones above were taken
    % from steerGauss and seem to  be false)
    %g   = 1 / (sqrt(2*pi*sigma)) * exp(-(x.^2)/(2*sigma^2)); % gaussian
    %gp  = 1 / (sqrt(2*pi*sigma)) * -(x/(sigma^2)).*exp(-(x.^2)/(2*sigma^2)); % 1st derivative
    %gpp = 1 / (sqrt(2*pi*sigma)) * (x.^2./((sigma^4)*exp(x.^2/(2*sigma^2))) - 1./((sigma^2).*exp((x.^2)/(2*sigma^2)))); % 2nd derivative
    g   = exp(-(x.^2)/(2*sigma^2)); % gaussian
    gp  = -(x/(sigma^2)).*exp(-(x.^2)/(2*sigma^2)); % 1st derivative
    gpp = (x.^2./((sigma^4)*exp(x.^2/(2*sigma^2))) - 1./((sigma^2).*exp((x.^2)/(2*sigma^2)))); % 2nd derivative
    
    % calculate derivatives
    %hx  = g' * gp;   Lx  = conv2pad(I,hx);
    %hy  = gp' * g;   Ly  = conv2pad(I,hy);
    %hxx = g' * gpp;  Lxx = conv2pad(I,hxx);
    %hyy = gpp' * g;  Lyy = conv2pad(I,hyy);
    %hxy = gp' * gp;  Lxy = conv2pad(I,hxy);
    %hx  = g' * gp;   Lx  = conv2(I,hx,'same');
    %hy  = gp' * g;   Ly  = conv2(I,hy,'same');
    %hxx = g' * gpp;  Lxx = conv2(I,hxx,'same');
    %hyy = gpp' * g;  Lyy = conv2(I,hyy,'same');
    %hxy = gp' * gp;  Lxy = conv2(I,hxy,'same');
    hx  = g' * gp;   Lx  = conv2wrap(I,hx,conv_shape);
    hy  = gp' * g;   Ly  = conv2wrap(I,hy,conv_shape);
    hxx = g' * gpp;  Lxx = conv2wrap(I,hxx,conv_shape);
    hyy = gpp' * g;  Lyy = conv2wrap(I,hyy,conv_shape);
    hxy = gp' * gp;  Lxy = conv2wrap(I,hxy,conv_shape);
    
    if show_kernels
        figure('name','filter kernels');
        subplot(3,2,1); imshow(mat2gray(hx)); title('hx');
        subplot(3,2,2); imshow(mat2gray(hy)); title('hy');
        subplot(3,2,3); imshow(mat2gray(hxx)); title('hxx');
        subplot(3,2,4); imshow(mat2gray(hyy)); title('hyy');
        subplot(3,2,5); imshow(mat2gray(hxy)); title('hxy');
    end
  