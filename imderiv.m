function [A,B]=imderiv(I)
  A=[zeros(size(I,1),1) diff(I,1,2)];
  B=[zeros(1,size(I,2)); diff(I,1,1)];