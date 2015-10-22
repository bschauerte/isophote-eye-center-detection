function eval_yaleb_isophote_eye_center_detector
  % Evaluate the isophote bases eye center detector on the YaleB data set
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
  
  if not(isunix)
    data_yaleb_path='C:\Users\Boris\Desktop\data\YaleB\';
  else
    data_yaleb_path='/home/bschauer/data/face/YaleB/';
  end

  neac=65; % there are 65 azimuth/elevation combinations for each subject and pose 0
  nsubjects=10;
  pose=0;
  data_yaleb_nsamples=neac * nsubjects;
  
  % parameters
  gsigma = 1;%0.70;%1; % the sigma (std. dev.) for the Gaussian derivatives
  ewx=40; ewy=30; % crop window size
  %ewx=64; ewy=48; % crop window size
  
  % results
  tndl = zeros(data_yaleb_nsamples,1); % table of normalized left eye center detection distances
  tndr = zeros(data_yaleb_nsamples,1); % table of normalized right eye center detection distances
  
  data_yaleb_sample = 0;
  for subject=1:nsubjects
    
    info=read_yale_info(subject,pose,data_yaleb_path); % contains all elevation/azimuth combinations for this subject/pose
    
    for i=1:size(info,1)
      data_yaleb_sample=data_yaleb_sample+1;
    
      assert(size(info,1) == neac);
      subject=info(i,1);
      pose=info(i,2);
      azimuth=info(i,3);
      elevation=info(i,4);
      
      % load image
      img=imread_yale(subject,pose,azimuth,elevation,data_yaleb_path);
      
      % read annotations
      c=info(i,:); % [subject pose azimuth elevation]
      [elx,ely,erx,ery,~,~]=read_yale_eye(subject,pose,azimuth,elevation,data_yaleb_path);
      el=[elx ely];
      er=[erx ery];
      
      % crop the eye regions
      hewx=floor(ewx/2); hewy=floor(ewy/2);
      calx = (elx-hewx+1); % crop anchor point for the left eye
      caly = (ely-hewy+1); % crop anchor point for the left eye
      carx = (erx-hewx+1); % crop anchor point for the right eye
      cary = (ery-hewy+1); % crop anchor point for the right eye
      calx = max(calx,1);
      caly = max(caly,1);
      carx = max(carx,1);
      cary = max(cary,1);
      img_left_eye=img(caly:(caly+ewy-1),calx:(calx+ewx-1)); % crop left eye
      img_right_eye=img(cary:(cary+ewy-1),carx:(carx+ewx-1)); % crop right eye
      
      % run isophote eye center detection
      [lecx,lecy]=run_isophote_eye_detector(img_left_eye,gsigma);
      [recx,recy]=run_isophote_eye_detector(img_right_eye,gsigma);
      % calculate the absolute position of the eye center detection in the
      % image
      lecx = lecx + (calx - 1);
      lecy = lecy + (caly - 1);
      recx = recx + (carx - 1);
      recy = recy + (cary - 1);
      lec=[lecx lecy];
      rec=[recx recy];

      % calculate evaluation measures
      ed = norm(lec - rec); % distance beween the eyes (for normalization)

      dl = norm(el - lec);
      dr = norm(er - rec);

      ndl=dl / ed; % normalize distance
      ndr=dr / ed; % normalize distance

      tndl(data_yaleb_sample) = ndl;
      tndr(data_yaleb_sample) = ndr;
    end
  end
  
  % plot accuracy
  es = [0:0.01:0.3];
  accl=zeros(2,numel(es));
  accr=zeros(2,numel(es));
  accb=zeros(2,numel(es));
  accw=zeros(2,numel(es));
  accl(1,:) = es;
  accr(1,:) = es;
  accb(1,:) = es;
  accw(1,:) = es;
  for i=1:numel(es)
    accl(2,i) = numel(find(tndl <= es(i))) / data_yaleb_nsamples;
    accr(2,i) = numel(find(tndr <= es(i))) / data_yaleb_nsamples;
    accb(2,i) = numel(find(min(tndr,tndl) <= es(i))) / data_yaleb_nsamples;
    accw(2,i) = numel(find(max(tndr,tndl) <= es(i))) / data_yaleb_nsamples;
  end
  
  %accb=max(accl,accr);   % best eye accuracy
  %accw=min(accl,accr);   % worst eye accuracy
  acca=(accl + accr) / 2; % average eye accuracy
  
  % plot the accuracy
  figure('name','accuracy');
  hold on;
  plot(accb(1,:),accb(2,:),'g')
  plot(accw(1,:),accw(2,:),'r')
  plot(acca(1,:),acca(2,:),'k')
  plot(accl(1,:),accl(2,:),'c')
  plot(accr(1,:),accr(2,:),'m')
  
  % short summary of evaluation results
  fprintf('%0.02f: b=%0.03f a=%0.03f w=%0.03f  l=%0.03f r=%0.03f\n',accb(1,6),accb(2,6),acca(2,6),accw(2,6),accl(2,6),accr(2,6))
  fprintf('%0.02f: b=%0.03f a=%0.03f w=%0.03f  l=%0.03f r=%0.03f\n',accb(1,11),accb(2,11),acca(2,11),accw(2,11),accl(2,11),accr(2,11))
  fprintf('%0.02f: b=%0.03f a=%0.03f w=%0.03f  l=%0.03f r=%0.03f\n',accb(1,16),accb(2,16),acca(2,16),accw(2,16),accl(2,16),accr(2,16))
  fprintf('%0.02f: b=%0.03f a=%0.03f w=%0.03f  l=%0.03f r=%0.03f\n',accb(1,26),accb(2,26),acca(2,26),accw(2,26),accl(2,26),accr(2,26))