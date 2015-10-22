function [k,c,D,acc,ind,Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=isophote_eye_center_detector3(I,sigma)
    % ISOPHOTE_EYE_CENTER_DETECTOR3 implements an eye-center detector based
    %   on "Accurate Eye Center Location and Tracking Using Isophote 
    %   Curvature" by R. Valenti and Theo Gevers
    %
    % This implementation relies on anisotropic Gauss filtering and the
    % .mex implementation of the main isophote calculations.
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
    
    [k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy] = isophote_calculation_anisotropic(I,sigma);
    %[k,c,D,R,ind,Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=isophote_calculation(I,sigma);
    
    % use mex file to calculate the isophote information and accumulator
    [k,c,dx,dy,acc] = isophote(Lx,Ly,Lxx,Lxy,Lyy);
    D = cat(3,dx,dy);
  