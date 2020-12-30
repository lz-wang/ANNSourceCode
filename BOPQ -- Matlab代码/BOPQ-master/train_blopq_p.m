function [coarse_centers, fine_centers, mu, R1, R2] = train_blopq_p(X, M)
% The pamametric solution of Locally Bilinear Optimized Product Quantization
% Input:
%	X is the training data matrix, and each row is an observation
%	M is the number of subspace
% Output:
%	coarse_centers is used to allocate the data sample to its corresponding coarse cluster before rotation
%	fine_centers is the codebook, and each cell of it is a sub-codebook
%	mu is a set of centroids of the residuals
% 	R1 is the left projection matrix
%	R2 is the right projection matrix   
k = 256; % fixed number of centers per subspaces
dim = size(X, 2);
d = dim / M;
fine_centers = cell(M, 1);
V = 8; % number of coarse clusters
[lbl, coarse_centers] = litekmeans(double(X), V);
coarse_centers = single(coarse_centers);
X_proj = cell(V, 1);
R1 = cell(V, 1);
R2 = cell(V, 1);
mu = cell(V, 1);
for v = 1:V
    cluster = X((lbl == v), :);
    Xc_tensor = TensorFV(cluster);
    n = size(cluster, 1);
    Xc = cell(n, 1);
    parfor i = 1:n
        Xc{i} = Xc_tensor(:, :, i);
    end
    Xc = cell2mat(Xc);
    R2{v} = eigenvalue_allocation(Xc, M);
    Xc = cell(1, n);
    parfor i = 1:n
        Xc{i} = Xc_tensor(:, :, i);
    end
    Xc = cell2mat(Xc);
    R1{v} = eigenvalue_allocation(Xc', M);
    residual = single(bsxfun(@minus, cluster, coarse_centers(v,:)));
    mu{v} = single(mean(residual, 1));
    Xc_tensor = TensorFV(bsxfun(@minus, residual, mu{v}));
    Xrot = cell(n, 1);
    parfor i = 1:n
        xt = R1{v}'*Xc_tensor(:, :, i)*R2{v};
        Xrot{i} = xt(:)';
    end
    X_proj{v} = cell2mat(Xrot);
end
X_proj = cell2mat(X_proj);
Xsubs = cell(M, 1);
for m = 1:M
    Xsubs{m} = X_proj(:, (1:d) + (m-1)*d);
end
parfor m = 1:M
    Xsub = Xsubs{m};
    [~, centers] = litekmeans(double(Xsub), k);
    fine_centers{m} = single(centers);
end

end

