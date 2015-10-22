function info=read_yale_info(subject,pose,DB_PATH)
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
    DB_PATH='/home/bschauer/data/face/YaleB';
  end

  % parse the information file
  filename=[DB_PATH '/images/yaleB' sprintf('%02d',subject) '_P' sprintf('%02d',pose) '.info'];
  fid=fopen(filename,'r');
  tline = fgets(fid);
  nline = 1;
  info=zeros(65,4);
  while ischar(tline)
      if nline == 1
        info(nline,:) = [subject pose NaN NaN]; % Ambient
      else
        info(nline,:) = sscanf(tline,'yaleB%d_P%dA%dE%d.pgm')';
      end
      nline = nline + 1;
      tline = fgets(fid);
  end
  fclose(fid);