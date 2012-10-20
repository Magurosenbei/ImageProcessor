clear all; clc;

K_low_to_high = 1.42;
K_high_to_low = 2;
k_low = 2;%:1.42;k_high = 2;
nD = 16; % Disparity range
nAlpha = 11; % Fusion weight range
w = 2;% nAlpha = 1;

%% Transformation of Low, High Exposed image
img_low = double(rgb2gray(imread('../aloe_exp0_view1.png')));
img_high = double(rgb2gray(imread('../aloe_exp2_view5.png')));

max_pix_val_low = max( max ( img_low ) );
max_pix_val_high = max ( max ( img_high ) );

max_x_val = ( max_pix_val_low / 0.2 ) .^ (1/2);
k_high = log( 255 / 3.2 ) / log( max_x_val);
Y = 0 : 255 ;
%x_low = ( y / 0.2 ) .^ (1/k_low);
%x_high = ( y / 3.2 ) .^ (1/k_high);
%X = 0.2* (( Y / 3.2 ) .^ (1/K_low_to_high)).^K_high_to_low;
%XX = 3.2*(( Y / 0.2 ) .^ (1/K_high_to_low)).^K_low_to_high;
%XX = XX(XX < max(X));
%x_low = x_low( x_low <max(x_high));
transformed_low_to_high = nan(size(img_low,1),size(img_low,2));
k_high = 1.6;
for i = 1 : 256
        transformed_low_to_high(img_low==i-1) = 3.2*(( (i-1) / 0.2).^(1/2)).^k_high;
end
subplot(1,2,1),imshow(uint8(round(transformed_low_to_high)));
subplot(1,2,2),imshow(uint8(img_high));

%% rewritten code 
imgFilePathL = 'TestImages\low.jpg'; % left File
imgFilePathR = 'TestImages\high.jpg'; % left File

%% Initializations
addpath('mdaisy-v1.0\'); addpath('GCmex1.9\');
l = nD * nAlpha;

imgL = imread(imgFilePathL); imgL_gray = rgb2gray(imgL);
[imgDirectoryPathL,imgFileNameL,imgFileExtensionL] = fileparts(imgFilePathL);
imgR = imread(imgFilePathR); imgR_gray = rgb2gray(imgR);
[imgDirectoryPathR,imgFileNameR,imgFileExtensionR] = fileparts(imgFilePathR);

%% Extracting the daisy descriptor on each pixel
tstart = tic;
[m n c] = size(imgL);
fprintf(1,'Extracting descriptor vector on left image...');
iptsdL = compute_daisy(imgL_gray);
fprintf(1,'Done \nExtracting right descriptor vector...');
iptsdR = compute_daisy(imgR_gray);
fprintf(1,'Done\n');
telapsed = toc(tstart);
fprintf(1,'Time taken to extract Daisy Descriptors = %f secs\n',telapsed);

%% Data Cost of Energy function
fprintf(1,'Generating the data cost...\n');
tstart = tic;
D = zeros(m,n,l);diff1 = zeros(1,1,l);diff2 = zeros(1,1,l);diff3 = zeros(1,1,l);diff4 = zeros(1,1,l);
D_diff1 = zeros(m,n,l); D_diff2 = zeros(m,n,l); D_diff3 = zeros(m,n,l);
[am bm cm] = meshgrid(1:m,1:n,1:l);
for i = 1 : m
    for j = 1 : n
        for k = 1 : l
            d = ceil(k/nAlpha) - 1;
            a = (rem(k-1,nAlpha))/(nAlpha - 1);            
            if (j-d>0 && i-w >= 1 && i+w <= m && j-d+w <= n && j-d-w >= 1 && j-w >= 1 && j+w <= n)
                featureL = iptsdL.descs(((i-1)*n)+j,:); featureR = iptsdR.descs(((i-1)*n)+j-d,:);
                %diff1(1,1,k) = sum(abs(featureL - featureR));
                diff1(1,1,k) = sqrt( sum( (featureL - featureR) .* (featureL - featureR) ) );
                diff2(1,1,k) = exp(-1*sum(sum((1+(double(imgL_gray(i-w:i+w,j-w:j+w))/255)).*(1+(double(imgR_gray(i-w:i+w,j-d-w:j-d+w))/255))))/(norm((1+double(imgL_gray(i-w:i+w,j-w:j+w))/255))*norm((1+double(imgR_gray(i-w:i+w,j-d-w:j-d+w))/255))));
                imgF = (a*double(imgL_gray(i-w:i+w,j-w:j+w))/255) + ((1-a)*double(imgR_gray(i-w:i+w,j-d-w:j-d+w))/255);
                %diff2(1,1,k) = exp( -((sum(sum(imgF.*log(1+imgF))))/(((2*w+1)^2)*log(2))) - (std2(imgF))^2 );
                %diff2(1,1,k) = exp( -(sum(sum(imgF.*log(1+imgF))))/(((2*w+1)^2)*log(2)) );
                %imgF(imgF==0)=1;
                %diff3(1,1,k) = exp(( -(sum(sum(imgF.*log(1+imgF))))/(((2*w+1)^2)*log(2)) ));
                %diff3(1,1,k) = exp( -(sum(sum(imgF.*log(imgF)))) ); %R
                %diff4(1,1,k) = exp((- (std2(imgF))^2)); %R
            else
                diff1(1,1,k) = 6; diff2(1,1,k) = 1; %diff3(1,1,k) = 1; diff4(1,1,k) = 1;%R
            end
        end        
        diff1 = (diff1 - min(diff1))/(max(diff1) - min(diff1));diff1(isnan(diff1))=1;
        diff2 = (diff2 - min(diff2))/(max(diff2) - min(diff2));diff2(isnan(diff2))=1;
        diff3 = (diff3 - min(diff3))/(max(diff3) - min(diff3));diff3(isnan(diff3))=1;
        diff4 = (diff4 - min(diff4))/(max(diff4) - min(diff4));diff4(isnan(diff4))=1;
        %D_diff1(i,j,:) = diff1(1,1,:); D_diff2(i,j,:) = diff2(1,1,:); D_diff3(i,j,:) = diff3(1,1,:);
        %D(i,j,:) = 1*(diff1(1,1,:) + diff2(1,1,:));% + 1*(diff3(1,1,:) + diff4(1,1,:));
        D(i,j,:) = (diff1(1,1,:) + diff2(1,1,:)) % + (diff3(1,1,:) + diff4(1,1,:));%R
    end
end
telapsed = toc(tstart);
fprintf(1,'Time taken to calculate data cost = %f secs\n',telapsed);

%% Generating smoothness cost
fprintf(1,'Generating the smoothness cost...\n');
tstart = tic;
S = zeros(l,l);
for k1 = 1 : l
    for k2 = 1 : l
        d1 = (ceil(k1/nAlpha) - 1)/(nD-1);
        a1 = (rem(k1-1,nAlpha))/(nAlpha - 1);% if a1 < 0, a1 = 1; end
        d2 = (ceil(k2/nAlpha) - 1)/(nD-1);
        a2 = (rem(k2-1,nAlpha))/(nAlpha - 1);% if a2 < 0, a2 = 1; end        
        S(k1,k2) = abs(d1-d2) + abs(a1-a2); % min(2* (abs(d1-d2) + abs(a1-a2)), 2); %2*(min((dp1-dp2)^2,(alpha1-alpha2)^2));        
    end
end
telapsed = toc(tstart);
fprintf(1,'Time taken to generate Smoothness Cost = %f secs\n',telapsed);

%% Optimizing engergy
fprintf(1,'Performing optimization...\n');
tstart = tic;
gch = GraphCut('open',D,S);
% [gch labels] = GraphCut('get', gch);
[gch se de] = GraphCut('energy', gch);
fprintf(1,'Energy before Optimization = %f\n',de+se);
%[gch labels] = GraphCut('expand', gch);
[gch labels] = GraphCut('swap', gch);
[gch se de] = GraphCut('energy', gch);
fprintf(1,'Energy after Optimization = %f\n',de+se);
[gch] = GraphCut('close', gch);
telapsed = toc(tstart);
fprintf(1,'Time taken to optimize = %f secs\n',telapsed);
labels = uint8(labels);

%% Displaying assigned labels
imgHDR = zeros(size(imgL));
alphaLabels = zeros(m,n);dpLabels = zeros(m,n);
imgHDR_gray = zeros(m,n);
for i = 1 : m
    for j = 1 : n
        % extract dp and alpha values for pixel (i,j) dp -> 0,1,2,...,nD-1; alpha -> 0,...,1
        dp = floor(labels(i,j)/nAlpha);
        alph = rem(labels(i,j),nAlpha);
        alph = double(double(alph)/double(nAlpha-1));        
        alphaLabels(i,j) = double(alph);
        dpLabels(i,j) = uint8(dp);
        if (i-dp >= 1 && i-dp <= m)
            imgHDR_gray(i,j) = round( (alph*double(imgL_gray(i,j))) + ((1-alph)*double(imgR_gray(i-dp,j))) );
            imgHDR(i,j,1) = round( (alph*double(imgL(i,j,1))) + ((1-alph)*double(imgR(i-dp,j,1))) );            
            imgHDR(i,j,2) = round( (alph*double(imgL(i,j,2))) + ((1-alph)*double(imgR(i-dp,j,2))) );
            imgHDR(i,j,3) = round( (alph*double(imgL(i,j,3))) + ((1-alph)*double(imgR(i-dp,j,3))) );
        else
            imgHDR_gray(i,j) = imgL_gray(i,j);
            imgHDR(i,j,1) = imgL(i,j,1);
            imgHDR(i,j,2) = imgL(i,j,2);
            imgHDR(i,j,3) = imgL(i,j,3);
        end        
    end
end
imview(uint8(imgHDR));
imwrite(uint8(imgHDR),'hdr.jpg','jpeg');

%% self mailing
cmap=colormap(gray(nD));
%imwrite(labels,cmap,'depthMap.jpg','jpeg');
setpref('Internet','E_mail','raghudeep.g@gmail.com');
setpref('Internet','SMTP_Server','smtp.gmail.com');
setpref('Internet','SMTP_Username','unome.88@gmail.com');
setpref('Internet','SMTP_Password','lyk@noida');
props = java.lang.System.getProperties;
props.setProperty('mail.smtp.auth','true');
props.setProperty('mail.smtp.socketFactory.class','javax.net.ssl.SSLSocketFactory');
props.setProperty('mail.smtp.socketFactory.port','465');
sendmail('raghudeep.g@gmail.com','HDR Image Result','','hdr.jpg');