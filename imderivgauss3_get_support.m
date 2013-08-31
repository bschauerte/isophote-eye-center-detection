function [x,Wx]=imderivgauss3_get_support(sigma)
  % determine necessary filter support (for Gaussian)
  %Wx = floor((5/2)*sigma * 2); % Wx = floor((5/2)*sigma); % @todo: is the support high enough? How to calculate the support for higher order derivatives? 
  %Wx = ceil(sigma*3)*2+1; % taken from matlab LoG code (http://www.mathworks.com/help/toolbox/images/ref/edge.html)
  Wx = ceil(sigma*3); % taken from matlab LoG code (http://www.mathworks.com/help/toolbox/images/ref/edge.html)
    % Note that the selected width has a measurable influence on the performance
  if Wx < 1
     Wx = 1;
  end
  x = [-Wx:Wx];