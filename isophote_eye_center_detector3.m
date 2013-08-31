% ISOPHOTE_EYE_CENTER DETECTOR Eye detector based on the 2008 CVPR paper 
%   "Accurate Eye Center Location and Tracking Using Isophote Curvature" by 
%   R. Valenti and Theo Gevers

function [k,c,D,acc,ind,Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=isophote_eye_center_detector3(I,sigma)
  imgsz=size(I);

  [k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy]=isophote_calculation_anisotropic(I,sigma);
  %[k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=isophote_calculation(I,sigma);
  
 
  % accumulator (simple implementation)
  %acc = zeros(size(I));
  %for x=1:size(I,2)
  %  for y=1:size(I,1)
  %    if not(isnan(c(y,x))) && not(isinf(c(y,x))) && k(y,x) < 0 && R(y,x) < 0.15*norm(imgsz) && R(y,x) > 2 % %0.125*norm(imgsz) && R(y,x) > 2 %
  %      %   ignore votes with positive curvature, because this is 
  %      %   specifically interesting to detect the iris (because "it can 
  %      %   be assumed that the sclera is brighter than the cornea and the
  %      %   iris)
  %      
  %      % better
  %      indx = ind(y,x,1);
  %      indy = ind(y,x,2);
  %      if indx < 1 || indy < 1 || indx > size(I,2) || indy > size(I,1)
  %        break
  %      end
  %      
  %      acc(indy,indx) = acc(indy,indx) + c(y,x);
  %    end
  %  end
  %end
  
  % use mex file to calculate the isophote information and accumulator
  if isempty(which('isophote')), addpath(genpath('./impl/')); end
  [k,c,dx,dy,acc]=isophote(Lx,Ly,Lxx,Lxy,Lyy);
  D=cat(3,dx,dy);
  