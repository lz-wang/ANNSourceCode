
data = load_double_array(data_fn, 'single')';
D = size(data,2);
N = size(data,1);

C1 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.1.bin'],'single'); 
C2 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.2.bin'],'single'); 
box = load_array([lopq_folder dataset '.mi.voc_'  Vstr '.bin'],'uint32');

ncentroids = 2*V;
D2 = D/2;
M2 = M/2;
	
disp(' - LOPQ: Getting cov');

% initialize LOPQ matrices and means
LOPQ = cell(ncentroids,1);
LOPQP = cell(ncentroids,1);
for i=1:ncentroids
	LOPQ{i} = zeros(D2,D2,'single');
	LOPQP{i}  = zeros(D2,D2,'single');
end
LOPQmu = zeros(ncentroids,D2,'single');
LOPQcount = zeros(ncentroids,1,'single');
LOPQE = zeros(ncentroids,D2,'single');
LOPQvar = zeros(ncentroids,1,'single');
	
labels = double( box - 1 );
lbl1 = uint32( floor( labels / V) + 1 );
lbl2 = uint32( mod( labels, V) + 1);
clear labels;

for j=1:N
	l1 = lbl1(j);
	l2 = lbl2(j);
	l2ind = l2 + V;
	res1 = data(j,1:D2) - C1(l1,:);
	res2 = data(j,D2+1:D) - C2(l2,:);
	LOPQmu(l1, :) = LOPQmu(l1, :) + res1;
	LOPQ{l1} =  LOPQ{l1} + res1'*res1;
	LOPQcount(l1) = LOPQcount(l1) + 1;
	LOPQmu(l2ind, :) = LOPQmu(l2ind, :) + res2;
	LOPQ{l2ind} =  LOPQ{l2ind} + res2'*res2;
	LOPQcount(l2ind) = LOPQcount(l2ind) + 1;
end

disp(' - LOPQ: Finalizing Rotations');
for i=1:ncentroids
	n = LOPQcount(i);
	disp(['cell ' num2str(i) ', points: ' num2str(n)]);
	LOPQmu(i, :) = LOPQmu(i, :) / n;
	mu = LOPQmu(i, :);
	A = LOPQ{i};
	A = (A + A')/(2*(n-1)) - mu'*mu;
	if (n > D2)
		[P, E] = eig(A);
	else
		disp('LESS POINTS THAN DIM!');
		P = ones(D2);
		E = eye(D2);
	end
	LOPQP{i} = P;
	LOPQE(i,:) = diag(E);
	LOPQvar(i) = det(E);
	idx = get_opq_ids(M2, D2, diag(E));
	LOPQ{i} = P(:, idx);		
end

save_triple_cell([lopq_folder 'mi_LOPQ.bin'], LOPQ, 'single');
save_triple_cell([lopq_folder 'mi_P.bin'], LOPQP, 'single');
save_double_array([lopq_folder 'mi_mu.bin'], LOPQmu, 'single');
save_double_array([lopq_folder 'mi_E.bin'], LOPQE, 'single');
save_array([lopq_folder 'mi_freq.bin'], LOPQcount, 'single');
save_array([lopq_folder 'mi_var.bin'], LOPQvar, 'single');

if (export_python)
	for i=1:ncentroids
		dlmwrite([python_folder '/R/lopq.c' num2str(i) '.R.txt'], LOPQ{i}, 'delimiter', ' ', 'precision', 10);
		dlmwrite([python_folder '/R/lopq.c' num2str(i) '.mu.txt'], LOPQmu(i,:), 'delimiter', ' ', 'precision', 10);
	end
end