% test the corrfilter1d implementation -> the result should be a zero matrix
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