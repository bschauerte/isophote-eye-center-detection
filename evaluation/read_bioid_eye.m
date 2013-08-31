function [lx,ly,rx,ry]=read_bioid_eye(filename)
  % Open the file
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