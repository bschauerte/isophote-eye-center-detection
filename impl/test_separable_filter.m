function test_separable_filter
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