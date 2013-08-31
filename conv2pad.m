function B=conv2pad(A,h)
    % 2-D convolution with padding.
    %
    % @author B. Schauerte
    % @date   2011

    assert(mod(3,2) ~= 0);
    hsz=size(h);
    hhsz=floor(hsz/2);
    Ap=padarray(A,hhsz,'symmetric');
    B=conv2(Ap,h,'valid');
