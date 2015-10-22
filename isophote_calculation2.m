function [k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=isophote_calculation2(I,sigma,conv_shape)
    % ISOPHOTE_CALCULATION2 Calculate the isophotes and their curvature,
    %   curvedness, target displacement coordinates/pixels/indices, 
    %   displacement vectors and radius
    %
    %   This implementation allows to specify the convolution shape 
    %   parameter (i.e., padding, same, etc.). See 'conv2wrap' and 'conv2'.
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

    % convert to gray level, if necessary
    if size(I,3) > 1
        I=rgb2gray(I);
    end
    I=im2double(I);

    % calculate 1st/2nd derivatives/gradients
    [Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss3(I,sigma,conv_shape);

    % Some helper functions
    Lx2 = Lx .^ 2;
    Ly2 = Ly .^ 2;

    % this is a shared term in the curvature and curvedness
    T1 = (2 .* Lx .* Lxy .* Ly) - (Lx2 .* Lyy) - (Ly2 .* Lxx) + eps; % "+ eps" to avoid div by 0

    % calculate the isophote curvature
    k = T1 ./ (((Lx2 + Ly2) .^ (3/2)) + eps); % "+ eps" to avoid div by 0

    % calculate the displacement vectors
    D = zeros([size(Lx) 2]);
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
    [X,Y]=meshgrid(1:size(Lx,2),1:size(Lx,1));
    XY=cat(3,X,Y);
    ind=Dind + XY; % the indices of the target accumulator cells
