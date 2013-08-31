%
% test the corrfilter1d implementation -> the result should be a zero matrix
%

% filtering along the 1st dimension
imfilter([1 2 3 4 5 6; 2 3 4 5 6 7],[1 2 3]',2) - corrfilter1d([1 2 3 4 5 6; 2 3 4 5 6 7],[1 2 3]',2,1)
% filtering along the 2nd dimension
imfilter([1 2 3 4 5 6; 2 3 4 5 6 7],[1 2 3],2) - corrfilter1d([1 2 3 4 5 6; 2 3 4 5 6 7],[1 2 3]',2,2)
% filtering along the 3rd dimension
A=rand(5,5,5); F=rand(1,1,3); imfilter(A,F,2) - corrfilter1d(A,F,2,3)

%
% test the corrfilter1d speed
%
% filter 1 line
filter=rand(1,13);
data=rand(1,1000);
tic, imfilter(data,rand,1); toc
tic, corrfilter1d(data,rand,1,2); toc

% filter 50 lines in parallel
filter=rand(1,13);
data=rand(50,1000); 
tic, imfilter(data,rand,1); toc
tic, corrfilter1d(data,rand,1,2); toc

% filter 50x50 vectors in parallel along the 3rd dimension
filter=rand(1,1,13);
data=rand(50,50,50); 
tic, imfilter(data,rand,1); toc
tic, corrfilter1d(data,rand,1,3); toc

% filter 50x50 vectors in parallel along the 3rd dimension and non-constant padding
filter=rand(1,1,13);
data=rand(32,32,32); 
tic, imfilter(data,rand,'symmetric'); toc
tic, corrfilter1d(data,rand,'symmetric',3); toc