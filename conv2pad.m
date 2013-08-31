function B=conv2pad(A,h)
  assert(mod(3,2) ~= 0);
  hsz=size(h);
  hhsz=floor(hsz/2);
  Ap=padarray(A,hhsz,'symmetric');
  B=conv2(Ap,h,'valid');
  