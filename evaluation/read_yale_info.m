function info=read_yale_info(subject,pose,DB_PATH)
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