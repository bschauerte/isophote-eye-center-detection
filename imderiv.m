function [A,B]=imderiv(I)
    % IMDERIV2 calculates the basic 1st order partial image derivatives in
    %   x- and y-direction.
    %
    % Simplistic implementation. 
    %
    % @author B. Schauerte
    % @date   2011
    %
    % Copyright (C) Boris Schauerte - All Rights Reserved
    % Unauthorized copying of this file, via any medium is strictly prohibited
    % Proprietary and confidential
    % Written by Boris Schauerte <schauerte@ieee.org>, 2011
    
    A=[zeros(size(I,1),1) diff(I,1,2)];
    B=[zeros(1,size(I,2)); diff(I,1,1)];