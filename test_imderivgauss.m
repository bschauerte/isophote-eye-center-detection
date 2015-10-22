% Test different image derivation/differentiation algorithms and compare.
% Isophotes can react sensitive to slight algorithm differences ...
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

I=im2double(rgb2gray(imread('/home/bschauer/private/wallpaper-hot/1310156288288.jpg')));
I=imresize(I,[NaN 64]);

sigma=1;

[Lx_o,Ly_o,Lxx_o,Lxy_o,Lyy_o,hx_o,hy_o,hxx_o,hxy_o,hyy_o]=imderivgauss3(I,sigma);

[Lx_a,Ly_a,Lxx_a,Lxy_a,Lyy_a,hx_a,hy_a,hxx_a,hxy_a,hyy_a]=imderivgauss_anisotropic(I,sigma);

figure('name','filter results');
subplot(5,2,1);  imshow(mat2gray(Lx_o));
subplot(5,2,2);  imshow(mat2gray(Lx_a));
subplot(5,2,3);  imshow(mat2gray(Ly_o));
subplot(5,2,4);  imshow(mat2gray(-Ly_a));
subplot(5,2,5);  imshow(mat2gray(Lxx_o));
subplot(5,2,6);  imshow(mat2gray(Lxx_a));
subplot(5,2,7);  imshow(mat2gray(Lyy_o));
subplot(5,2,8);  imshow(mat2gray(Lyy_a));
subplot(5,2,9);  imshow(mat2gray(Lxy_o));
subplot(5,2,10); imshow(mat2gray(-Lxy_a));