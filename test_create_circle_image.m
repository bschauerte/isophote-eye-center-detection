function [image]=test_create_circle_image(imsize,pos,r,bgval,fgval)
    % Create a simple image that depicts a white/black circle on a
    % black/white background. Useful to debug/understand the isophote
    % calculation.
    %
    % @author B. Schauerte
    % @date   2011
    %
    % Copyright (C) Boris Schauerte - All Rights Reserved
    % Unauthorized copying of this file, via any medium is strictly prohibited
    % Proprietary and confidential
    % Written by Boris Schauerte <schauerte@ieee.org>, 2011
    
    if nargin < 1, imsize = [100 100]; end
    if nargin < 2, pos = imsize/2; end
    if nargin < 3, r = sqrt(sum(imsize.^2))/3; end
    if nargin < 4
        fgval = 0;%1;
        bgval = 1;%0;
    end

    X = ones(imsize(1),1)*(1:imsize(2));
    Y = (1:imsize(1))'*ones(1,imsize(2));
    Z = (X - pos(2)).^2 + (Y - pos(1)).^2;
    image = ones(imsize) * bgval;
    image(Z <= r^2) = fgval;
    image=image';

    if nargout < 1
        figure;
        imshow(image);
    end