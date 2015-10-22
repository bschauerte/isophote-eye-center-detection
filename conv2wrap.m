function C=conv2wrap(A,B,shape)
    % Wrapper around conv2 that integrates the padding/padded mode ('pad')
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