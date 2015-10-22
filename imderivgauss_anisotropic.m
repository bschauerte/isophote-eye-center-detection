function [Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss_anisotropic(I,sigma)
    % ISOPHOTE_CALCULATION2 Calculate the isophotes and their curvature,
    %   curvedness, target displacement coordinates/pixels/indices, 
    %   displacement vectors and radius
    %
    %   This implementation uses anisotropic Gauss filtering instead of
    %   Matlab's built-in filter operation.
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
    
    % parameters
    if nargin < 2
        sigma = 1;
    end
    
    phi=0;
    Lx  =  anigauss(I,sigma,sigma,phi,0,1);
    Ly  = -anigauss(I,sigma,sigma,phi,1,0);
    Lxx =  anigauss(I,sigma,sigma,phi,0,2);
    Lyy =  anigauss(I,sigma,sigma,phi,2,0);
    Lxy = -anigauss(I,sigma,sigma,phi,1,1);
    %Lx  = anigauss(I,sigma,sigma,phi,0,1);
    %Ly  = anigauss(I,sigma,sigma,phi,1,0);
    %Lxx = anigauss(I,sigma,sigma,phi,0,2);
    %Lyy = anigauss(I,sigma,sigma,phi,2,0);
    %Lxy = anigauss(I,sigma,sigma,phi,1,1);
    %Lx  = anigauss(I,sigma,sigma,phi,1,0);
    %Ly  = anigauss(I,sigma,sigma,phi,0,1);
    %Lxx = anigauss(I,sigma,sigma,phi,2,0);
    %Lyy = anigauss(I,sigma,sigma,phi,0,2);
    %Lxy = anigauss(I,sigma,sigma,phi,1,1);
    hx  = 0;
    hy  = 0;
    hxx = 0;
    hyy = 0;
    hxy = 0;
    