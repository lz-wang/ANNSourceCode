disp(' - LOPQ: getting all labels for all queries...');
	
qdata = load_double_array(query_data_file, 'single')';
noq = size(qdata,2)

query_mi_1 = zeros(noq, V);
query_mi_2 = zeros(noq, V);
query_mi_1_d = zeros(noq, V);
query_mi_2_d = zeros(noq, V);

C1 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.1.bin'],'single')'; 
C2 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.2.bin'],'single')'; 

for q=1:noq	
	query = qdata(:,q);
	[query_mi_1(q,:), query_mi_1_d(q,:)] = yael_nn(C1, query(1:D/2), V, 2);
	[query_mi_2(q,:), query_mi_2_d(q,:)] = yael_nn(C2, query((D/2+1):D), V, 2);
end	
% disp([' mean time for projection ' num2str(mean(timse)) ', saving...']);
disp(['saving query proj']);
save_double_array([lopq_folder dataset '_query.mi.voc_'  num2str(V) '.1.bin'],query_mi_1,'uint32');
save_double_array([lopq_folder dataset '_query.mi.voc_'  num2str(V) '.2.bin'],query_mi_2,'uint32');
save_double_array([lopq_folder dataset '_query.mi.voc_'  num2str(V) '.1.d.bin'],query_mi_1_d,'single');
save_double_array([lopq_folder dataset '_query.mi.voc_'  num2str(V) '.2.d.bin'],query_mi_2_d,'single');

C1=C1';
C2=C2';
w = V;
qdata1 = qdata(1:D2,:)';
qdata2 = qdata(D2+1:D,:)';
disp([ ' - saving projections per query (this might take a while...)']);
for q=1:noq
	%tic;
	%q
	lbl1 = query_mi_1(q,1:w);
	lbl2 = query_mi_2(q,1:w);
	lbl2ind = lbl2+V;
	query1 = repmat( qdata1(q,:), [w 1] ) - C1(lbl1,:);
	query2 = repmat( qdata2(q,:), [w 1] ) - C2(lbl2,:);
	query1 = query1  - LOPQmu(lbl1,:) ;
	query2 = query2  - LOPQmu(lbl2ind,:);
	for i=1:w
		query1(i,:) = query1(i,:)* LOPQ{lbl1(i)};
		query2(i,:) = query2(i,:)* LOPQ{lbl2ind(i)};
	end
	
	save_double_array([lopq_folder '/queries_lazy/' dataset '_query.' num2str(q) '.1.bin'], query1', 'single');
	save_double_array([lopq_folder '/queries_lazy/' dataset '_query.' num2str(q) '.2.bin'], query2', 'single');
	%toc;
end

