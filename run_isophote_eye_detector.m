function [ecx,ecy,k,c,D,acc,sacc,ind,Lx,Ly,Lxx,Lxy,Lyy]=run_isophote_eye_detector(img,gsigma)
  if nargin < 2
    gsigma = 1;
  end
  
  % pre-processing
  if ~isfloat(img)
    img=im2double(img);
    img = (img - min(img(:))) / (max(img(:)) - min(img(:)));
  end
  
  % run isophote eye detector
  [k,c,D,acc,ind,Lx,Ly,Lxx,Lxy,Lyy]=isophote_eye_center_detector3(img,gsigma);
  %if isempty(which('spectral_saliency')), addpath(genpath('../saliency/')); end
  %acc=acc+imresize(spectral_saliency(imresize(img,[NaN 32])),size(img));
  %acc=acc+spectral_saliency(img);
  %k=0; c=0; D=0;
  
  % post-process
  h = fspecial('gaussian',11,3);
  %h = fspecial('gaussian',9,9);
  %h = fspecial('average',9);
  %h = fspecial('gaussian',3,3);
  sacc = conv2(acc,h,'same');
  %sacc = conv2pad(acc,h);
  
  % select MIC (Maximum Isocenter)
  [tmp,i] = max(sacc);
  [~,j] = max(tmp);
  ymic = i(j);
  xmic = j;
  
  % cluster -> gradient ascend to other true center (e.g. use mean shift)
  % ...
  
  % verify eye center location (e.g. with SIFT)
  % ...
  
  % set output eye center x and y coordinates
  ecx = xmic;
  ecy = ymic;