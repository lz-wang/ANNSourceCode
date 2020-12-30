function save_triple_cell (faa,c,type)

    fid = fopen (faa, 'wb');
    
    fwrite (fid, size(c,1), 'double');
  
	for ai=1:size(c,1)
		a = c{ai};
		fwrite (fid, size(a,1), 'double');
		for i=1:size(a,1)
			fwrite (fid, size(a,2), 'double');
			fwrite (fid, a(i,:), type);
		end
	end
    fclose(fid);
end