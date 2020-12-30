function [centers_table, idx_table, R1, R2] = train_bopq_np(X, M)
% The non-pamametric solution of Globally Bilinear Optimized Product Quantization
% Input:
%	X is the training data matrix, and each row is an observation
%	M is the number of subspace
% Output:
%	centers_table is the codebook, and each cell of it is a sub-codebook
%	idx_table is the encoded X, and each number of it is a code index
% 	R1 is the left projection matrix
%	R2 is the right projection matrix	

    k = 256; % fixed number of centers per subspaces 
    dim = size(X, 2);
    d = dim / M;
    max_iter = 10; 
    centers_table = cell(M, 1);
    idx_table = zeros(size(X, 1), M, 'uint16');
    X_tensor = TensorFV(X);
    [d1, d2, n] = size(X_tensor);
    R1 = eye(d1);
    R2 = eye(d2);
    distortion = 0;
    Xsubs = cell(M, 1);
    for m = 1:M
       Xsubs{m} = X(:, (1:d) + (m-1)*d);
    end
    Y = cell(1, M);
    parfor m = 1:M
        Xsub = Xsubs{m};
        [~, centers] = litekmeans(double(Xsub), k);  
        centers_table{m} = centers;
        dist = sqdist(centers', Xsub');
        [dist, idx] = min(dist);
        idx_table(:,m) = idx(:);
        Y{m} = single(centers(idx(:),:));
        distortion = distortion + mean(dist);
    end
    Y = cell2mat(Y);
    fprintf('Initial distortion: %e \n', distortion);
    for iter = 1:max_iter
        Y_tensor = TensorFV(Y);
        % fix R1 and rotate R2
        X = cell(n, 1);
        Y = cell(n, 1);
        parfor i = 1:n
            X{i} = R1' * X_tensor(:, :, i);
            Y{i} = Y_tensor(:, :, i);
        end
        X = cell2mat(X);
        Y = cell2mat(Y);
        [UB, ~, UA] = svds(double(Y'*X), d2);
        R2 = UA * UB';
        % fix R2 and rotate R1
		X = cell(1, n);
        Y = cell(1, n);
        parfor i = 1:n
            X{i} = X_tensor(:, :, i) * R2;
            Y{i} = Y_tensor(:, :, i);
        end
        X = cell2mat(X);
        Y = cell2mat(Y);
        [UB,~,UA] = svds(double(Y*X'), d1);
        R1 = UA * UB';
        % Update the codebook
        Xrot = cell(n, 1);
        parfor i = 1:n
            xt = R1'*X_tensor(:, :, i)*R2;
            Xrot{i} = xt(:)';
        end
        Xrot = cell2mat(Xrot);
        distortion = 0;
        Xsubs = cell(M,1);
        for m = 1:M
            Xsubs{m} = Xrot(:, (1:d) + (m-1)*d);
        end
        Y = cell(1, M);
        parfor m = 1:M
            Xsub = Xsubs{m};
            [~, centers] = litekmeans(double(Xsub), k, 'start', centers_table{m});
            centers_table{m} = centers;
            dist = sqdist(centers', Xsub');
            [dist, idx] = min(dist);
            idx_table(:,m) = idx(:);
            Y{m} = centers(idx(:),:);
            distortion = distortion + mean(dist);
        end
        Y = cell2mat(Y);
        fprintf('Iteration %d, distortion = %e \n', iter, distortion);
    end
end






