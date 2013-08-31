% build.m
%   build the isophote-based eye center detection implementation for use with Matlab
%
% author: B. Schauerte
% date: 2011

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
