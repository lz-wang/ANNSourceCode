function [centers_table, idx_table, R1, R2] = train_bopq_p(X, M)
% The pamametric solution of Globally Bilinear Optimized Product Quantization
% Input:
%	X is the training data matrix, and each row is an observation
%	M is the number of subspace
% Output:
%	centers_table is the codebook, and each cell of it is a sub-codebook
%	idx_table is the encoded X, and each number of it is a code index
% 	R1 is the left projection matrix
%	R2 is the right projection matrix
    k = 256; % Codebook size is fixed to 256
    [n, dim] = size(X);
    d = dim/M;
    X_tensor = TensorFV(X);
    % Optimize R2
    X = cell(n, 1);
    parfor i = 1:n
        X{i} = X_tensor(:, :, i);
    end
    X = cell2mat(X);
    R2 = eigenvalue_allocation(X, M);
    % Optimize R1
    X = cell(1, n);
    parfor i = 1:n
        X{i} = X_tensor(:, :, i);
    end
    X = cell2mat(X);
    R1 = eigenvalue_allocation(X', M);
    Xrot = cell(n, 1);
    parfor i = 1:n
        xt = R1'*X_tensor(:, :, i)*R2;
        Xrot{i} = xt(:)';
    end
    Xrot = cell2mat(Xrot);
    Xsubs = cell(M,1);
    for m = 1:M
        Xsubs{m} = Xrot(:, (1:d) + (m-1)*d);
    end
    centers_table = cell(M, 1);
    parfor m = 1:M
        Xsub = Xsubs{m};
        [~, centers] = litekmeans(double(Xsub), k);
        centers_table{m} = centers;
        dist = sqdist(centers', Xsub');
        [~, idx] = min(dist);
        idx_table(:,m) = idx(:);
    end
end

