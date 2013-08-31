function [lx,ly,rx,ry,mx,my]=read_yale_eye(subject,pose,azimuth,elevation,DB_PATH)
  if nargin < 5
    DB_PATH='/home/bschauer/data/face/YaleB';
  end

  % load the annotation file
  m=load([DB_PATH '/metadata/yaleB' sprintf('%02d',subject) '_P' sprintf('%02d',pose) '.crop']);

  assert(pose == 0); % otherwise there are no left eye/right eye/mouth coordinates
  left_eye_coords=m(1:65,:);
  right_eye_coords=m(66:130,:);
  mouth_coords=m(131:195,:);

  % parse the information file
  info=read_yale_info(subject,pose,DB_PATH);

  % read the correct element from the annotation file
  if not(isnan(azimuth)) && not(isnan(elevation))
    [~,ia]=intersect(info,[subject pose azimuth elevation],'rows');
  else
    ia=1;
  end
  lx=left_eye_coords(ia,1);
  ly=left_eye_coords(ia,2);
  rx=right_eye_coords(ia,1);
  ry=right_eye_coords(ia,2);
  mx=mouth_coords(ia,1);
  my=mouth_coords(ia,2);
