
centroids = cell(2*V,1);
disp(' - LOPQ: Getting Codes...');
for c=1:V
	centroids{c} = load_triple_cell([lopq_folder '/subquantizers/subquantizers.1.c' num2str(c) '.' Vstr '.' num2str(M) '.' num2str(Ks) '.adc.bin'] , 'single') ;
	centroids{c+V} = load_triple_cell([lopq_folder '/subquantizers/subquantizers.2.c' num2str(c) '.' Vstr '.' num2str(M) '.' num2str(Ks) '.adc.bin'] , 'single');
end

codes = zeros(M, N, 'uint16');	
data1 = data(:, 1:D2);
data2 = data(:, D2+1:D);
labels = double( box - 1 );
lbl1 = uint32( floor( labels / V) + 1 );
lbl2 = uint32( mod( labels, V) + 1);
clear labels;
data1 = data1 - C1(lbl1,:);
data2 = data2 - C2(lbl2,:);
lbl2 = V + lbl2;
data1 = data1 - LOPQmu(lbl1,:);
data2 = data2 - LOPQmu(lbl2,:);

Ds = D2 / M2;
for c=1:V
	
	iind1 = find(lbl1 == c );
	iind2 = find(lbl2 == (c+V) );
		
	if (length(iind1)>0) 
		data_m1 = single(  data1(iind1,:) * LOPQ{c} );
		for m=1:M2
			start_dim = (m-1)*Ds+1;
			cent1 = centroids{c}{m}; cent1 = cent1';
			data_m = data_m1(:, start_dim:(start_dim+Ds-1))';
			[cod, distz] = yael_nn(cent1, data_m, 1, 2);
			codes(m,iind1) = cod - 1;	
		end
	end
	if (length(iind2)>0)
		data_m2 = single( data2(iind2,:) * LOPQ{c+V} );			
		for m=1:M2
			start_dim = (m-1)*Ds+1;
			cent2 = centroids{c+V}{m}; cent2 = cent2';
			data_m = data_m2(:, start_dim:(start_dim+Ds-1))' ;
			[cod, distz] = yael_nn(cent2, data_m, 1, 2);
			codes(M2+m,iind2) = cod - 1;
		end
	end
end

save_double_array([lopq_folder '/' dataset '_codes.' Vstr '.' num2str(M) '.' num2str(Ks) '.us.adc.ms.bin'] ,codes, 'uint16');
