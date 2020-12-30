function idx = get_opq_ids(M, D, E)
	
	dd=D:-1:1;
	if ~issorted(E)
		if ~issorted(fliplr(E))
			if (sum(sort(E,'descend') - E)>0)
				disp('non sorted eigvalues!');
				idx = 0;
				return;
			end
		end
		disp('reversley sorted!');
		dd = 1:D;
		E=E+1;
	end
	
	Ds = D / M;
	idx = zeros(M,Ds);
		
	k = ones(M,1);
	eigprod = zeros(M,1);
		
	for dk=1:D
		d=dd(dk);
		e = log2(abs(E(d)));
		
		ind = find(k <= Ds);
		tmp_eigprod = eigprod(ind);
		[mp, p] = min(tmp_eigprod);
		
		pp =  ind(p);
		if (mp == 0), eigprod( pp ) = e; 
		else eigprod( pp ) = eigprod( pp )+e;
		end
		idx( pp, k( pp ) ) = d;
		k(pp) = k(pp) + 1;
			
	end
	idx = idx';
	idx = idx(:)';
	
		