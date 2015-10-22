function [k,c,D,acc,ind,Lx,Ly,Lxx,Lxy,Lyy]=isophote_eye_center_detector2(I,sigma)
    % ISOPHOTE_EYE_CENTER_DETECTOR2 implements an eye-center detector based
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
    
    imgsz=size(I);
    
    % convert to RGB if necessary
    if size(I,3) > 1
        I=rgb2gray(I);
    end
    I=im2double(I);
    %I=im2uint8(I);
    
    % calculate 1st/2nd derivatives/gradients
    %[Lx,Ly,Lxx,Lxy,Lyy,hx,hy,hxx,hxy,hyy]=imderivgauss3(I,sigma);
    [Lx,Ly,Lxx,Lxy,Lyy]=imderivgauss3(I,sigma);
    
    % Some helper functions
    Lx2 = Lx .^ 2;
    Ly2 = Ly .^ 2;
    
    % this is a shared term in the curvature and curvedness
    T1 = (2 .* Lx .* Lxy .* Ly) - (Lx2 .* Lyy) - (Ly2 .* Lxx) + eps; % "+ eps" to avoid div by 0
    
    % calculate the isophote curvature
    k = T1 ./ ((Lx2 + Ly2) .^ (3/2));% + eps); % "+ eps" to avoid div by 0
    
    % calculate the displacement vectors
    D = zeros([size(I) 2]);
    D(:,:,1) = (Lx .* (Lx2 + Ly2)) ./ T1;
    D(:,:,2) = (Ly .* (Lx2 + Ly2)) ./ T1;
    
    % calculate the curvedness
    Lxx2 = Lxx .^ 2;
    Lxy2 = Lxy .^ 2;
    Lyy2 = Lyy .^ 2;
    c = sqrt( Lxx2 + 2*Lxy2 + Lyy2 );
    
    % the corresponding radius (defined by the displacement vectors)
    R = sqrt(sum(D .^ 2,3));
    
    % accumulator (simple implementation)
    acc = zeros(size(I));
    Dind=round(D); % round D to get valid indices, i.e. "ganze Zahlen"
    [X,Y]=meshgrid(1:size(I,2),1:size(I,1));
    XY=cat(3,X,Y);
    ind=Dind + XY; % the indices of the target accumulator cells
    
    for x=1:size(I,2)
        for y=1:size(I,1)
            if not(isnan(c(y,x))) && not(isinf(c(y,x))) && k(y,x) < 0 && R(y,x) < 0.15*norm(imgsz) && R(y,x) > 2 % %0.125*norm(imgsz) && R(y,x) > 2 %
                %   ignore votes with positive curvature, because this is
                %   specifically interesting to detect the iris (because "it can
                %   be assumed that the sclera is brighter than the cornea and the
                %   iris)
                
                % better
                indx = ind(y,x,1);
                indy = ind(y,x,2);
                if indx < 1 || indy < 1 || indx > size(I,2) || indy > size(I,1)
                    break
                end
                
                acc(indy,indx) = acc(indy,indx) + c(y,x);
            end
        end
    end