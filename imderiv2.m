function [Lx,Ly,Lxx,Lxy,Lyy]=imderiv2(I)
    % IMDERIV2 calculates 1st and 2nd order image derivatives
    %
    % Simplest implementation, see http://en.wikipedia.org/wiki/Edge_detection#Differential_edge_detection
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
    
    hLx=[-0.5 0 0.5];
    %hLy=[0.5; 0; -0.5];
    hLy=[-0.5; 0; 0.5];
    hLxx=[1 -2 1];
    %hLxy=[-1/4 0 1/4; 0 0 0; 1/4 0 -1/4]; % i.e. hLy x hLx
    hLxy=-[-1/4 0 1/4; 0 0 0; 1/4 0 -1/4]; % i.e. hLy x hLx
    hLyy=[1; -2; 1];
    Lx=imfilter(I,hLx);
    Ly=imfilter(I,hLy);
    Lxx=imfilter(I,hLxx);
    Lxy=imfilter(I,hLxy);
    Lyy=imfilter(I,hLyy);
  