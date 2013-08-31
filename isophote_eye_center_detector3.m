function [k,c,D,acc,ind,Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=isophote_eye_center_detector3(I,sigma)
    % ISOPHOTE_EYE_CENTER_DETECTOR3 implements an eye-center detector based
    %   on "Accurate Eye Center Location and Tracking Using Isophote 
    %   Curvature" by R. Valenti and Theo Gevers
    %
    % This implementation relies on anisotropic Gauss filtering and the
    % .mex implementation of the main isophote calculations.
    %
    % @author B. Schauerte
    % @date   2011
    
    [k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy] = isophote_calculation_anisotropic(I,sigma);
    %[k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=isophote_calculation(I,sigma);
    
    % use mex file to calculate the isophote information and accumulator
    [k,c,dx,dy,acc] = isophote(Lx,Ly,Lxx,Lxy,Lyy);
    D = cat(3,dx,dy);
  