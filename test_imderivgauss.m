% Test different image derivation/differentiation algorithms and compare.
% Isophotes can react sensitive to slight algorithm differences ...
%
% @author B. Schauerte
% @date   2011

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