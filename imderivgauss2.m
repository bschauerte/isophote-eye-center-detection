function [Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss2(I,sigma)
  
  % parameters
  if nargin < 2
    sigma = 1;
  end

  % determine necessary filter support (for Gaussian)
  Wx = floor((5/2)*sigma); % @todo: is the support high enough?
  if Wx < 1
     Wx = 1;
  end
  x = [-Wx:Wx];

  % create the coefficients, i.e. evaluate 1D Gaussian filter (and its
  % derivatives, i.e. 1st=gp and 2nd=gpp).
  %g = exp(-(x.^2)/(2*sigma^2)); % gaussian
  %gp = -(x/sigma).*exp(-(x.^2)/(2*sigma^2)); % 1st derivative
  %gpp = x.^2./(sigma^3.*exp(x.^2./(2*sigma^2))) - 1./(sigma.*exp(x.^2./(2*sigma^2))); % 2nd derivative
  
  % new forumalas for 1st and 2nd derivatives (the ones above were taken 
  % from steerGauss and seem to  be false)
  %g = exp(-(x.^2)/(2*sigma^2)); % gaussian
  %gp = -(x/(sigma^2)).*exp(-(x.^2)/(2*sigma^2)); % 1st derivative
  %gpp = x.^2./((sigma^4)*exp(x.^2/(2*sigma^2))) - 1./((sigma^2).*exp((x.^2)/(2*sigma^2))); % 2nd derivative
  
  % new forumalas for 1st and 2nd derivatives (the ones above were taken 
  % from steerGauss and seem to  be false)
  g = 1 / (sqrt(2*pi*sigma)) * exp(-(x.^2)/(2*sigma^2)); % gaussian
  gp = 1 / (sqrt(2*pi*sigma)) * -(x/(sigma^2)).*exp(-(x.^2)/(2*sigma^2)); % 1st derivative
  gpp = 1 / (sqrt(2*pi*sigma)) * (x.^2./((sigma^4)*exp(x.^2/(2*sigma^2))) - 1./((sigma^2).*exp((x.^2)/(2*sigma^2)))); % 2nd derivative
  
  % 2-D gauss: g(x,y) = 1 / sqrt(2*PI*sigma) * exp(-1/2 * (x^2 / sigma^2 + y^2 / sigma^2))
  %   diff(diff(exp(-1/2 * (x^2 / sigma^2 + y^2 / sigma^2)),'x'),'y') = (x*y)/(sigma^4*exp(x^2/(2*sigma^2) + y^2/(2*sigma^2)))
  %fxy = @(x,y) (x*y)/(sigma^4*exp(x^2/(2*sigma^2) + y^2/(2*sigma^2)))
  
  % calculate derivatives
  hx = gp;        Lx =  conv2pad(I,hx);%conv2(I,hx,'same');
  hy = gp';       Ly =  conv2pad(I,hy);%conv2(I,hy,'same');
  hxx = gpp;      Lxx = conv2pad(I,hxx);%conv2(I,hxx,'same');
  hyy = gpp';     Lyy = conv2pad(I,hyy);%conv2(I,hyy,'same');
  hxy = gp' * gp; Lxy = conv2pad(I,hxy);%conv2(I,hxy,'same');
  %hx = gp;        Lx =  conv2pad(conv2(I,g,'same'),hx);%conv2(I,hx,'same');
  %hy = gp';       Ly =  conv2pad(conv2(I,g','same'),hy);%conv2(I,hy,'same');
  %hxx = gpp;      Lxx = conv2pad(conv2(I,g,'same'),hxx);%conv2(I,hxx,'same');
  %hyy = gpp';     Lyy = conv2pad(conv2(I,g','same'),hyy);%conv2(I,hyy,'same');
  %hxy = gp' * gp; Lxy = conv2pad(conv2(I,g' * g,'same'),hxy);%conv2(I,hxy,'same');
  