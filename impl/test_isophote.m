function test_isophote
  imgin=rgb2gray(imread('../images/test_pattern.png'));    % simple test pattern
  %imgin=rgb2gray(imread('../images/big_eye_iris.jpg'));    % simple test pattern
  imgsz=size(imgin);
  sigma=single(1);
  [Lx Ly Lxx Lxy Lyy]=gauss_filter(imgin,sigma,sigma);
  
  figure('name','image'); imshow(imgin);
  %figure('name','Lx'); imshow(mat2gray(Lx)); 
  %figure('name','Ly'); imshow(mat2gray(Ly));
  %figure('name','Lxx'); imshow(mat2gray(Lxx));
  %figure('name','Lxy'); imshow(mat2gray(Lxy));
  %figure('name','Lyy'); imshow(mat2gray(Lyy));

  [k c dx dy acc]=isophote(Lx,Ly,Lxx,Lxy,Lyy);
  %min(dx(:))
  %max(dx(:))
  %min(dy(:))
  %max(dy(:))
  
  figure('name','k'); imshow(mat2gray(k));
  figure('name','c'); imshow(mat2gray(c));
  figure('name','acc'); imshow(mat2gray(acc));
  
  %[mk mc mD]=isophote_calculation(imgin,sigma);
  [mk mc mD macc mind]=isophote_eye_center_detector3(imgin,sigma);
  figure('name','mk');   imshow(mat2gray(mk));
  figure('name','mc');   imshow(mat2gray(mc));
  figure('name','macc'); imshow(mat2gray(macc));
  
  vdx=dx(10:imgsz(1)-10,10:imgsz(2)-10); % cut out a part of the image that is valid, i.e. ignore the corrupt borders
  vdy=dy(10:imgsz(1)-10,10:imgsz(2)-10); % cut out a part of the image that is valid, i.e. ignore the corrupt borders
  mdx=mD(:,:,1);
  mdy=mD(:,:,2);
  min(mdx(:))
  max(mdx(:))
  min(mdy(:))
  max(mdy(:))
  vmdx=mdx(10:imgsz(1)-10,10:imgsz(2)-10);
  vmdy=mdy(10:imgsz(1)-10,10:imgsz(2)-10);
  sum(sqrt((vdx(:) - vmdx(:)).^2)) / ((imgsz(1)-20) * (imgsz(1)-20))
  sum(sqrt((vdy(:) - vmdy(:)).^2)) / ((imgsz(1)-20) * (imgsz(1)-20))
  
  vk=k(10:imgsz(1)-10,10:imgsz(2)-10);
  vmk=mk(10:imgsz(1)-10,10:imgsz(2)-10);
  sum(abs(sign(vk(:)) - sign(vmk(:))))
  
  % accumulator (simple implementation)
  test_acc = zeros(size(imgin));
  for x=1:size(imgin,2)
    for y=1:size(imgin,1)
      if not(isnan(c(y,x))) && not(isinf(c(y,x))) && k(y,x) < 0
        %   ignore votes with positive curvature, because this is 
        %   specifically interesting to detect the iris (because "it can 
        %   be assumed that the sclera is brighter than the cornea and the
        %   iris)
        
        % better
        indx = mind(y,x,1);
        indy = mind(y,x,2);
        if indx < 1 || indy < 1 || indx > size(imgin,2) || indy > size(imgin,1)
          break
        end
        
        test_acc(indy,indx) = test_acc(indy,indx) + c(y,x);
      end
    end
  end
  figure('name','macc'); imshow(mat2gray(test_acc));