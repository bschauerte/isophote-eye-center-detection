function B=conv2pad(A,h)
    % 2-D convolution with padding.
    %
    % @author B. Schauerte
    % @date   2011
    %
    % Copyright (C) Boris Schauerte - All Rights Reserved
    % Unauthorized copying of this file, via any medium is strictly prohibited
    % Proprietary and confidential
    % Written by Boris Schauerte <schauerte@ieee.org>, 2011

    assert(mod(3,2) ~= 0);
    hsz=size(h);
    hhsz=floor(hsz/2);
    Ap=padarray(A,hhsz,'symmetric');
    B=conv2(Ap,h,'valid');
