data = load_double_array(data_fn, 'single')';
D = size(data,2);
N = size(data,1);

C1 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.1.bin'],'single'); 
C2 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.2.bin'],'single'); 
box = load_array([lopq_folder dataset '.mi.voc_'  Vstr '.bin'],'uint32');
LOPQ = load_triple_cell([lopq_folder 'mi_LOPQ.bin'], 'single');
LOPQmu = load_double_array([lopq_folder 'mi_mu.bin'], 'single');

ncentroids = 2*V;
D2 = D/2;
M2 = M/2;

Ds = D2 / M2;
vocsz = V;
vs2 = 2*vocsz;

data1 = data(:, 1:D2);
data2 = data(:, D2+1:D);
labels = double( box - 1 );
labels1 = uint32( floor( labels / vocsz) + 1 );
labels2 = uint32( mod( labels, vocsz) + 1);
labels=[];
data1 = data1 - C1(labels1,:) - LOPQmu(labels1,:);
data2 = data2 - C2(labels2,:) - LOPQmu(labels2+vocsz,:);	

for c=1:V
	idx1 = find(labels1==c);
	cdata1 = data1(idx1,:)*LOPQ{c};
	idx2 = find (labels2==c);
	cdata2 = data2(idx2,:)*LOPQ{c+vocsz};
	
	centroids1 = cell(M2,1);
	centroids2 = cell(M2,1);
	disp(['c=' num2str(c) ', data: ' num2str(size(cdata1,1)) ' - '  num2str(size(cdata2,1))]);
	for m=1:M2
		start_dim = (m-1)*Ds+1;
		%disp([ num2str(c) ' : subquantizer ' num2str(m) ', dim [' num2str(start_dim) '-' num2str((start_dim+Ds-1)) ']' ]);
		slice1 = cdata1(:, start_dim:(start_dim+Ds-1))';
		slice2 = cdata2(:, start_dim:(start_dim+Ds-1))';
		if (size( slice1, 2 ) >= Ks)
			C = yael_kmeans(slice1, Ks, 'niter', niter_local, 'verbose', 0);
			centroids1{m} = C';
		else
		 	centroids1{m} = slice1';
		end
		if (size( slice2, 2 ) >= Ks)
			C = yael_kmeans(slice2, Ks, 'niter', niter_local, 'verbose', 0);
			centroids2{m} = C';
		else
		 	centroids2{m} = slice2';
		end
		
	end
	save_triple_cell([lopq_folder '/subquantizers/subquantizers.1.c' num2str(c) '.' Vstr '.' num2str(M) '.' num2str(Ks) '.adc.bin'] ,centroids1, 'single');
	save_triple_cell([lopq_folder '/subquantizers/subquantizers.2.c' num2str(c) '.' Vstr '.' num2str(M) '.' num2str(Ks) '.adc.bin'] ,centroids2, 'single');
end

if (export_python)
	for c=1:V
		centroids1 = load_triple_cell([lopq_folder '/subquantizers/subquantizers.1.c' num2str(c) '.' Vstr '.' num2str(M) '.' num2str(Ks) '.adc.bin'] , 'single') ;
		centroids2 = load_triple_cell([lopq_folder '/subquantizers/subquantizers.2.c' num2str(c) '.' Vstr '.' num2str(M) '.' num2str(Ks) '.adc.bin'] , 'single');
		cent1 =[]; cent2=[];
		for m=1:M2
			cent1 = [cent1 centroids1{m}];
			cent2 = [cent2 centroids2{m}];
		end
		dlmwrite([python_folder '/sq/lopq.c' num2str(c) '.sq.txt'], cent1, 'delimiter', ' ', 'precision', 10);
		dlmwrite([python_folder '/sq/lopq.c' num2str(c+V) '.sq.txt'], cent2, 'delimiter', ' ', 'precision', 10);
	end
end