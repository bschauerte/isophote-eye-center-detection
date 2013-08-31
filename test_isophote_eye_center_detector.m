function test_isophote_eye_center_detector(arg1,arg2)
    % Simple showcase of the isophote-based (eye-center) detector and all
    % of its information (k, sacc, ...)
    %
    % @author B. Schauerte
    % @date   2011

    %% add necessary libraries/implementations
    if isempty(which('isophote')), addpath(genpath('./impl/')); end

    %% what kind of input did we get?
    if nargin < 1, arg1='center'; end
    % create/load test image
    switch (arg1)
        case 'center'
            imgsz  = [400,400];
            radius = 160;
            img    = test_create_circle_image(imgsz,imgsz/2,radius); % simple circle
        case 'bioid'
            if not(isunix)
                data_bioid_path='C:\Users\Boris\Desktop\data\BioID';%'/cvhci/data/face/BioID'
            else
                data_bioid_path='/cvhci/data/face/BioID';
            end
            if nargin < 2
                data_bioid_snum=44;
            else
                data_bioid_snum=arg2;
            end
            img = uint8(readpgm([data_bioid_path '/images/BioID_' sprintf('%04d',data_bioid_snum) '.pgm']));

            [elx,ely,erx,ery] = read_bioid_eye([data_bioid_path '/metadata/BioID_' sprintf('%04d',data_bioid_snum) '.eye']);
            figure('name','BioID image'); imshow(img); hold on; plot(elx,ely,'gx','MarkerSize',10,'LineWidth',1); plot(erx,ery,'gx','MarkerSize',10,'LineWidth',1); hold off;
            ewx=40; ewy=30; % cropped eye window size
            %ewx=64; ewy=48; % cropped eye window size
            hewx=floor(ewx/2); hewy=floor(ewy/2);
            img=img((ely-hewy+1):(ely+hewy),(elx-hewx+1):(elx+hewx)); % crop left eye
            %img=img((ery-hewy+1):(ery+hewy),(erx-hewx+1):(erx+hewx)); % crop right eye
        otherwise
            if ischar(arg1)
                img = rgb2gray(imread(arg1)); % load an image at the specified path
            else
                img = arg1;
            end
    end

    %% main
    % optional resizing
    img=imresize(img,[64 NaN]);
    %img=imresize(img,[40 NaN]);
    % optional Gaussian smoothing
    %img = imfilter(img,fspecial('gaussian',5,1.5)); % smoothing is already done IN the isophote_eye_detector, because we are calculating with the derivatives of Gaussians

    % little helpers
    imgsz=size(img);

    [xmic,ymic,k,c,D,acc,sacc,ind,Lx,Ly,Lxx,Lxy,Lyy]=run_isophote_eye_detector(img);

    % visualize pixels that contributed to the MIC
    eradi=[3 5 10 15];
    figure('name','contributing edges (selected center)');
    for i=1:numel(eradi)
        subplot(2,numel(eradi),i); imagesc(contributing_pixel_map(xmic,ymic,ind,eradi(i)) .* c);%imshow(contributing_pixel_map(xmic,ymic,ind,eradi(i)) .* c);
        subplot(2,numel(eradi),i+numel(eradi)); imshow(contributing_pixel_map(xmic,ymic,ind,eradi(i)));
    end
    figure('name','contributing edges (true center)');
    xreal=44;
    yreal=31;
    for i=1:numel(eradi)
        subplot(2,numel(eradi),i); imagesc(contributing_pixel_map(xreal,yreal,ind,eradi(i)) .* c);%imshow(contributing_pixel_map(xreal,yreal,ind,eradi(i)) .* c);
        subplot(2,numel(eradi),i+numel(eradi)); imshow(contributing_pixel_map(xreal,yreal,ind,eradi(i)));
    end

    % displacement vectors
    %D1=D(:,:,1);
    %D2=D(:,:,2);
    %min(D1(:))
    %min(D2(:))
    %max(D1(:))
    %max(D2(:))
    %figure('name','D1'); imshow(mat2gray(abs(D1)));
    %figure('name','D2'); imshow(mat2gray(abs(D2)));

    figure('name','img');
    %hold all;
    % display the image
    imshow(img); hold on;
    % display ground truth
    %plot(elx,ely,'cx','MarkerSize',10);
    %plot(erx,ery,'cx','MarkerSize',10);
    % display detections
    plot(xmic,ymic,'r*','MarkerSize',10,'LineWidth',1);
    hold off;
    figure('name','c'); imagesc(mat2gray(c)); colorbar; %imshow(mat2gray(c));
    figure('name','k'); imagesc(mat2gray(k)); colorbar; %imshow(mat2gray(k));
    figure('name','abs(k)'); imshow(mat2gray(abs(k)));
    %figure('name','--k--'); imshow(mat2gray(k .* edge(img,'canny'))); % visualize the curvature according to the presentation method used in the CVPR paper
    figure('name','contour(c)'); imcontour(c); colorbar; %contourf(c); axis image;
    figure('name','contour(k)'); imcontour(k); colorbar; %contourf(k); axis image;
    figure('name','contour(abs(k))'); imcontour(abs(k)); colorbar; %contourf(k); axis image;

    % display filters
    %figure('name','Lx'); imshow(mat2gray(Lx));
    %figure('name','Ly'); imshow(mat2gray(Ly));
    %figure('name','Lxx'); imshow(mat2gray(Lxx));
    %figure('name','Lyy'); imshow(mat2gray(Lyy));
    %figure('name','Lxy'); imshow(mat2gray(Lxy));

    figure('name','acc'); imagesc(acc); colorbar; %imshow(mat2gray(acc));
    figure('name','sacc'); imagesc(sacc); colorbar; hold on; plot(xmic,ymic,'r*','MarkerSize',10); hold off;%imshow(mat2gray(sacc)); hold on; plot(xmic,ymic,'r*','MarkerSize',10); hold off;

    function plot_vector_field(D)
        sz=size(D);
        sz=sz(1:2);
        [X,Y]=meshgrid(1:sz(1),1:sz(2));
        D(isnan(D)) = 0;
        %quiver(X,Y,D(:,:,1),D(:,:,2));
        quiver(Y,X,D(:,:,1)',D(:,:,2)');
        axis equal;
    end
end