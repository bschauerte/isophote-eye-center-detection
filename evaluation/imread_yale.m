function I=imread_yale(subject,pose,azimuth,elevation,DB_PATH)
  if nargin < 5
    DB_PATH='/home/bschauer/data/face/YaleB';
  end
  
  if not(isnan(elevation)) && not(isnan(azimuth))
    filename=sprintf('%s/images/yaleB%02d_P%02dA%+04dE%+03d.pgm',DB_PATH,subject,pose,azimuth,elevation);
  else
    filename=sprintf('%s/images/yaleB%02d_P%02d_Ambient.pgm',DB_PATH,subject,pose);
  end
  I=imread(filename);