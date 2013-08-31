function test_separable_filter
  %imgin=rgb2gray(imread('../images/test_pattern.png'));    % simple test pattern
  %imgin=rgb2gray(imread('../images/big_eye_iris.jpg'));    % simple test pattern
  imgin=uint8(rand(100,20)*200);
  rowfilter=ones(1,9) / 9;
  colfilter=ones(1,9) / 9;
  imgout=separable_filter(imgin,rowfilter,colfilter);
  
  figure; imshow(imgin);
  %figure; imshow(mat2gray(imgout));
  figure; imshow(uint8(imgout));
  
  imgout