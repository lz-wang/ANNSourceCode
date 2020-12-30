close all;
clear variables;
clc;

warning off;

 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% synthetic gaussian
%%% (OPQ-P and OPQ-NP should be similar)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
nTrain = 5000;
dim = 64;
sigma = exp(-0.1*(1:dim)');
Xtrain = randn(nTrain, dim)*diag(sigma);

fprintf('****** Demo: Gaussian ******\n');
demo(Xtrain);
title('distortion: synthetic Gaussian');
drawnow;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% 784d MNIST
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load mnist;
Xtrain = Xtrain / 255;

fprintf('****** Demo: MNIST ******\n');
demo(Xtrain);
title('distortion: MNIST');
drawnow;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% 128d SIFT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load sift;

fprintf('****** Demo: SIFT ******\n');
demo(Xtrain);
title('distortion: SIFT');
drawnow;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% 960d GIST
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load gist;
Xtrain = double(Xtrain);

fprintf('****** Demo: GIST ******\n');
demo(Xtrain);
title('distortion: GIST');
drawnow;
