function map=contributing_pixel_map(x,y,ind,radius)
    % CONTRIBUTING_PIXEL_MAP calculates which pixels 'voted' for the position
    %   (x,y); radius is the maximum (i.e. error) distances between the voted
    %   position and the given/selected position x,y.
    %
    % @author B. Schauerte
    % @date   2011

    distmic=ind;
    distmic(:,:,1) = distmic(:,:,1) - x;
    distmic(:,:,2) = distmic(:,:,2) - y;
    distmic = distmic .^ 2;
    distmic = sqrt(sum(distmic,3));
    map = zeros(size(ind,1),size(ind,2));
    map(distmic <= radius) = 1;