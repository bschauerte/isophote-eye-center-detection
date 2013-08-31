function [A,B]=imderivgauss(I)
    % IMDERIVGAUSS calculates the basic 1st order partial image derivatives
    %   in x- and y-direction. It uses Gaussian smoothing to provide better
    %   derivatives.
    %
    % This implementation relies on the 'steerGauss' filter/library.
    %
    % @author B. Schauerte
    % @date   2011
    %
    % Copyright (C) Boris Schauerte - All Rights Reserved
    % Unauthorized copying of this file, via any medium is strictly prohibited
    % Proprietary and confidential
    % Written by Boris Schauerte <schauerte@ieee.org>, 2011

    A=steerGauss(I,180);
    B=steerGauss(I,90);