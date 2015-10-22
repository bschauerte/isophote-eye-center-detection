% build.m
%   build the isophote-based eye center detection implementation for use with Matlab
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

mex CXXFLAGS="\$CXXFLAGS -Wall" -c gauss_filter.cpp
mex CXXFLAGS="\$CXXFLAGS -Wall" -c separable_filter.cpp
debug_build=true;
if debug_build
  mex CXXFLAGS="\$CXXFLAGS -Wall" -g -D__MEX gauss_filter.cpp separable_filter.o
  mex CXXFLAGS="\$CXXFLAGS -Wall" -g -D__MEX separable_filter.cpp
  mex CXXFLAGS="\$CXXFLAGS -Wall" -g -D__MEX isophote.cpp
else
  mex CXXFLAGS="\$CXXFLAGS -Wall" -D__MEX gauss_filter.cpp separable_filter.o
  mex CXXFLAGS="\$CXXFLAGS -Wall" -D__MEX separable_filter.cpp
  mex CXXFLAGS="\$CXXFLAGS -Wall" -D__MEX isophote.cpp
end
