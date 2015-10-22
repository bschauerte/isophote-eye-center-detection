function [k,D,c,acc,Lx,Ly,Lxx,Lxy,Lyy]=isophote_eye_center_detector(I)
    % ISOPHOTE_EYE_CENTER_DETECTOR implements an eye-center detector based
    %   on "Accurate Eye Center Location and Tracking Using Isophote 
    %   Curvature" by R. Valenti and Theo Gevers
    %
    % This implementation relies on imderivgauss3 and performs the main
    % calculations (accumulator & co.) in Matlab. If you want to try some
    % minor code changes, this is where you can simply hack and test it.
    % It's also a good implementation to see and understand what's being 
    % calculated.
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
    
    % convert to RGB if necessary
    if size(I,3) > 1
        I=rgb2gray(I);
    end
    I=im2double(I);
    
    % calculate 1st/2nd derivatives/gradients
    %[Lx,Ly]=gradient(I);
    %[Lxx,Lxy]=gradient(Lx);
    %[~,Lyy]=gradient(Ly);
    %[Lx,Ly]=imderiv(I);
    %[Lxx,Lxy]=imderiv(Lx);
    %[Lyx,Lyy]=imderiv(Ly);
    %[Lx,Ly]=imderivgauss(I);
    %[Lxx,Lxy]=imderivgauss(Lx);
    %[Lyx,Lyy]=imderivgauss(Ly);
    %[Lx,Ly,Lxx,Lxy,Lyy]=imderiv2(I);
    %[Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss2(I,1);
    [Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss3(I,1);
    
    % Some helper functions
    Lx2 = Lx .^ 2;
    Ly2 = Ly .^ 2;
    
    % calculate the isophote curvature
    k = - ( ((Ly2 .* Lxx) - (2 .* Lx .* Lxy .* Ly) + (Lx2 .* Lyy)) ./ ((Lx2 + Ly2) .^ (3/2)) );
    
    % calculate the displacement vectors
    Dx = - (Lx .* (Lx2 + Ly2)) ./ ((Ly2 .* Lxx) - (2 .* Lx .* Lxy .* Ly) + (Lx2 .* Lyy));
    Dy = - (Ly .* (Lx2 + Ly2)) ./ ((Ly2 .* Lxx) - (2 .* Lx .* Lxy .* Ly) + (Lx2 .* Lyy));
    D = zeros([size(I) 2]);
    D(:,:,1) = Dx;
    D(:,:,2) = Dy;
    
    % the corresponding radius (defined by the displacement vectors)
    %R = sqrt(sum(D .^ 2,3));
    %R = R(:);
    %R = R(find(not(isnan(R)) & R > 2 & find(not(isinf(abs(R))))));
    %R = sort(R)
    %figure; plot(R(1:end-4)');
    
    % calculate the curvedness
    Lxx2 = Lxx .^ 2;
    Lxy2 = Lxy .^ 2;
    Lyy2 = Lyy .^ 2;
    c = sqrt( Lxx2 + 2*Lxy2 + Lyy2 );
    
    % accumulator (simple implementation)
    acc = zeros(size(I));
    % round Dx/Dy to get valid indices, i.e. "ganze Zahlen"
    Dxi=round(Dx);
    Dyi=round(Dy);
    for x=1:size(I,2)
        for y=1:size(I,1)
            %if abs(c(y,x)) > 4
            %if not(isnan(c(y,x))) && not(isinf(abs(c(y,x))))
            if not(isnan(c(y,x))) && not(isinf(abs(c(y,x)))) && k(y,x) < 0 %
                %   ignore votes with positive curvature, because this is
                %   specifically interesting to detect the iris (because "it can
                %   be assumed that the sclera is brighter than the cornea and the
                %   iris)
                
                % easy
                %indx = min(max(1,x + Dxi(y,x)),size(I,2));%D(y,x,1)),size(I,2));
                %indy = min(max(1,y + Dyi(y,x)),size(I,1));%D(y,x,2)),size(I,1));
                % better
                indx = x + Dxi(y,x);
                indy = y + Dyi(y,x);
                if indx < 1 || indy < 1 || indx > size(I,2) || indy > size(I,1)
                    break
                end
                
                %acc(indy,indx) = acc(indy,indx) + 1;
                acc(indy,indx) = acc(indy,indx) + c(y,x);
            end
        end
    end