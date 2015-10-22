function [image]=test_create_circle_image(imsize,pos,r,bgval,fgval)
    % Create a simple image that depicts a white/black circle on a
    % black/white background. Useful to debug/understand the isophote
    % calculation.
    %
    % @author Boris Schauerte
    % @email  boris.schauerte@eyezag.com
    % @date   2011
    %
    % Copyright (C) 2011  Boris Schauerte
    % 
    % This program is free software: you can redistribute it and/or modify
    % it under the terms of the GNU General Public License as published by
    % the Free Software Foundation, either version 3 of the License, or
    % (at your option) any later version.
    %
    % This program is distributed in the hope that it will be useful,
    % but WITHOUT ANY WARRANTY; without even the implied warranty of
    % MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    % GNU General Public License for more details.
    %
    % You should have received a copy of the GNU General Public License
    % along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
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