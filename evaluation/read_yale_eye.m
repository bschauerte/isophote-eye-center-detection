function [lx,ly,rx,ry,mx,my]=read_yale_eye(subject,pose,azimuth,elevation,DB_PATH)
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
