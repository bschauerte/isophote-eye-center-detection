function [lx,ly,rx,ry]=read_bioid_eye(filename)
  % Open the file
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

  fid = fopen(filename,'r');
  
  tline = fgets(fid);
  while ischar(tline)
      if tline(1) == '#' || length(tline) < 2
        tline = fgets(fid);
        continue;
      end
      
      %[lx,ly,rx,ry] = sscanf(tline,'%d %d %d %d');
      eyes = sscanf(tline,'%d %d %d %d');
      lx = eyes(1);
      ly = eyes(2);
      rx = eyes(3);
      ry = eyes(4);
      
      tline = fgets(fid);
  end
  
  % Close the file
  fclose(fid);