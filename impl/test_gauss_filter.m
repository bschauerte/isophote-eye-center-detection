function test_gauss_filter
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

  imgin=rgb2gray(imread('../images/test_pattern.png'));    % simple test pattern
  %imgin=rgb2gray(imread('../images/big_eye_iris.jpg'));    % simple test pattern
  imgsz=size(imgin);
  sigma=single(1);
  [Lx Ly Lxx Lxy Lyy]=gauss_filter(imgin,sigma,sigma);
  
  figure('name','image'); imshow(imgin);
  %figure; imshow(mat2gray(imgout));
  figure('name','Lx'); imshow(mat2gray(Lx)); 
  %min(Lx(:))
  %max(Lx(:))
  figure('name','Ly'); imshow(mat2gray(Ly));
  %min(Ly(:))
  %max(Ly(:))
  figure('name','Lxx'); imshow(mat2gray(Lxx));
  %min(Lxx(:))
  %max(Lxx(:))
  figure('name','Lxy'); imshow(mat2gray(Lxy));
  %min(Lxy(:))
  %max(Lxy(:))
  figure('name','Lyy'); imshow(mat2gray(Lyy));
  %min(Lyy(:))
  %max(Lyy(:))

  addpath(genpath('..'));
  [mLx,mLy,mLxx,mLxy,mLyy,mhx,mhy,mhxx,mhxy,mhyy]=imderivgauss3(imgin,sigma,'same');
  
  figure('name','C++ vs. MATLAB implementation');
  subplot(2,5,1); imshow(mat2gray(Lx)); 
  subplot(2,5,2); imshow(mat2gray(Ly)); 
  subplot(2,5,3); imshow(mat2gray(Lxx)); 
  subplot(2,5,4); imshow(mat2gray(Lxy)); 
  subplot(2,5,5); imshow(mat2gray(Lyy)); 
  subplot(2,5,6); imshow(mat2gray(mLx)); 
  subplot(2,5,7); imshow(mat2gray(mLy)); 
  subplot(2,5,8); imshow(mat2gray(mLxx)); 
  subplot(2,5,9); imshow(mat2gray(mLxy)); 
  subplot(2,5,10); imshow(mat2gray(mLyy));
  vLx=Lx(10:imgsz(1)-10,10:imgsz(2)-10); % cut out a part of the image which is valid, i.e. ignore the corrupt borders
  vmLx=mLx(10:imgsz(1)-10,10:imgsz(2)-10);
  min(vLx(:))
  max(vLx(:))
  vLxsz=size(vLx);
  sum(sqrt(abs(vLx(:).^2 - vmLx(:).^2))) % / (vLxsz(1)*vLxsz(2))
  
 
  
  
  