function [A,B]=imderivgauss(I)
  A=steerGauss(I,180);
  B=steerGauss(I,90);