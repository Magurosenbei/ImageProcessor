% Code to perform exposure fusion of teo images.
clear all; close all; clc;
%% Parameters and input
leftImg = 'exp0_view5.png';%'cloth2/exp0_view5.png';%'dataset/lv/lv1.jpg';%aloe/exp0_view1.jpg';%'cave2/s7708_02.0.jpg'; %strcat(testDir,'/exp0_view1.png');
rightImg = 'exp2_view5.png';%'cloth2/exp2_view5.png';%'dataset/lv/lv2.jpg';%aloe/exp2_view1.jpg';%'cave2/s7708_04.0.jpg'; %strcat(testDir,'/exp2_view1.png');
addpath('GCmex1.9/');
I1 = imread(leftImg); imgL = double(rgb2gray(I1))/255;
I2 = imread(rightImg); imgR = double(rgb2gray(I2))/255;
w = 4; nAlpha = 101;

%% Computing the D matrix. Size of D -> m x n x nAlpha
[m n] = size(imgL);
fprintf(1,'Generating the data cost...\n');
tstart = tic;
D = zeros(m,n,nAlpha);
for i = 1 : m
    for j = 1 : n
        for k = 1 : nAlpha
            a = (k-1)/(nAlpha - 1);
            if (i-w >= 1 && i+w <= m && j+w <= n && j-w >= 1)
                imgF = (a*imgL(i-w:i+w,j-w:j+w)) + ((1-a)*imgR(i-w:i+w,j-w:j+w));
                D(i,j,k) = exp( (sum(sum(imgF.*log(imgF))))/( (2*w+1)*0.5*log(2) ) );				                
            else
                D(i,j,k) = 1; 
            end
        end
        D(i,j,:) = mat2gray(D(i,j,:));
    end
end
telapsed = toc(tstart);
fprintf(1,'Time taken to calculate data cost = %f secs\n',telapsed);


%% Computing the S matrix. Size of S -> nAlpha x nAlpha
fprintf(1,'Generating the smoothness cost...\n');
tstart = tic;
[X Y] = meshgrid(1:nAlpha,1:nAlpha);
S = abs( (X - 1)/(nAlpha - 1) - (Y - 1)/(nAlpha - 1));%S(S>0.3)=2;
telapsed = toc(tstart);
fprintf(1,'Time taken to generate Smoothness Cost = %f secs\n',telapsed);

%% GraphCut.
fprintf(1,'Performing optimization...\n');
tstart = tic;
gch = GraphCut('open',D,S);
[gch se de] = GraphCut('energy', gch);
fprintf(1,'Energy before Optimization = %f\n',de+se);
[gch labels] = GraphCut('swap', gch);
[gch se de] = GraphCut('energy', gch);
fprintf(1,'Energy after Optimization = %f\n',de+se);
[gch] = GraphCut('close', gch);
telapsed = toc(tstart);
fprintf(1,'Time taken to optimize = %f secs\n',telapsed);

%% fusion of I1, I2 to create I.
wMap = (double(labels))/(nAlpha-1);
hdr = imgL.*wMap + imgR.*(1-wMap);
I = zeros(m,n,3);
I(:,:,1) = double(I1(:,:,1)).*wMap + double(I2(:,:,1)).*(1-wMap);
I(:,:,2) = double(I1(:,:,2)).*wMap + double(I2(:,:,2)).*(1-wMap);
I(:,:,3) = double(I1(:,:,3)).*wMap + double(I2(:,:,3)).*(1-wMap);
figure, imshow(uint8(round(I))),title('Fused Color Img');
imwrite(uint8(round(I)),'I.png','png');
p1 = imhist(imgL)./numel(imgL); p2 = imhist(imgR)./numel(imgR); p3 = imhist(hdr)./numel(hdr);
p1(p1==0) = 1; p2(p2==0) = 1;p3(p3==0) = 1;
avgInfVal1 = sum(-p1.*log(p1)); avgInfVal2 = sum(-p2.*log(p2)); avgInfVal3 = sum(-p3.*log(p3));
fprintf(1,'Information val left->%f right->%f fused->%f\n',avgInfVal1,avgInfVal2,avgInfVal3);
