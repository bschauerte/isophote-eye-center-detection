function C=conv2wrap(A,B,shape)
  if nargin < 3
    shape = 'full';
  end
  
  switch lower(shape)
    case {'pad'}
      C=conv2pad(A,B);
    case {'validsame'}
      C=zeros(size(A));
      C1=conv2(A,B,'valid');
      sz=size(C);
      dsz=size(C)-size(C1);
      hdsz=dsz / 2;
      C(hdsz(1):(sz(1)-hdsz(1)-1),hdsz(2):(sz(2)-hdsz(2)-1))=C1;
    otherwise
      C=conv2(A,B,shape);
  end