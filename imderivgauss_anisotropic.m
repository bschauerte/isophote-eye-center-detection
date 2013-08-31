function [Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss_anisotropic(I,sigma)
  if isempty(which('anigauss')), addpath(genpath('../libs/anisotropic_gauss/')); end

  % parameters
  if nargin < 2
    sigma = 1;
  end
  
  phi=0;
  Lx  =  anigauss(I,sigma,sigma,phi,0,1);
  Ly  = -anigauss(I,sigma,sigma,phi,1,0);
  Lxx =  anigauss(I,sigma,sigma,phi,0,2);
  Lyy =  anigauss(I,sigma,sigma,phi,2,0);
  Lxy = -anigauss(I,sigma,sigma,phi,1,1);
  %Lx  = anigauss(I,sigma,sigma,phi,0,1);
  %Ly  = anigauss(I,sigma,sigma,phi,1,0);
  %Lxx = anigauss(I,sigma,sigma,phi,0,2);
  %Lyy = anigauss(I,sigma,sigma,phi,2,0);
  %Lxy = anigauss(I,sigma,sigma,phi,1,1);
  %Lx  = anigauss(I,sigma,sigma,phi,1,0);
  %Ly  = anigauss(I,sigma,sigma,phi,0,1);
  %Lxx = anigauss(I,sigma,sigma,phi,2,0);
  %Lyy = anigauss(I,sigma,sigma,phi,0,2);
  %Lxy = anigauss(I,sigma,sigma,phi,1,1);
  hx  = 0;
  hy  = 0;
  hxx = 0;
  hyy = 0;
  hxy = 0;
  