function [k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy]=isophote_calculation_anisotropic(I,sigma)
    % ISOPHOTE_CALCULATION Calculate the isophotes and their curvature,
    %   curvedness, target displacement coordinates/pixels/indices, 
    %   displacement vectors and radius
    %
    % @author B. Schauerte
    % @date 2011
    %
    % Copyright (C) Boris Schauerte - All Rights Reserved
    % Unauthorized copying of this file, via any medium is strictly prohibited
    % Proprietary and confidential
    % Written by Boris Schauerte <schauerte@ieee.org>, 2011

  % convert from RGB to Gray if necessary
  if size(I,3) > 1
    I=rgb2gray(I);
  end
  I=im2double(I);
  
  % calculate 1st/2nd derivatives/gradients
  [Lx,Ly,Lxx,Lxy,Lyy]=imderivgauss_anisotropic(I,sigma);
  
  % Some helper functions
  Lx2 = Lx .^ 2;
  Ly2 = Ly .^ 2;
  
  % this is a shared term in the curvature and curvedness
  T1 = (2 .* Lx .* Lxy .* Ly) - (Lx2 .* Lyy) - (Ly2 .* Lxx) + eps; % "+ eps" to avoid div by 0
  
  % calculate the isophote curvature
  k = T1 ./ (((Lx2 + Ly2) .^ (3/2)) + eps); % "+ eps" to avoid div by 0
  
  % calculate the displacement vectors
  D = zeros([size(I) 2]);
  D(:,:,1) = (Lx .* (Lx2 + Ly2)) ./ T1;
  D(:,:,2) = (Ly .* (Lx2 + Ly2)) ./ T1;
  
  % calculate the curvedness
  Lxx2 = Lxx .^ 2;
  Lxy2 = Lxy .^ 2;
  Lyy2 = Lyy .^ 2;
  c = sqrt( Lxx2 + 2*Lxy2 + Lyy2 );
  
  % the corresponding radius (defined by the displacement vectors)
  R = sqrt(sum(D .^ 2,3));
  
  % create the target indices/coordinates resulting from the displacement vectors
  Dind=round(D); % round D to get valid indices, i.e. "ganze Zahlen"
  [X,Y]=meshgrid(1:size(I,2),1:size(I,1));
  XY=cat(3,X,Y);
  ind=Dind + XY; % the indices of the target accumulator cells
