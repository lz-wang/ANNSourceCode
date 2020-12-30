function a = load_triple_cell (faa,type)

    fid = fopen (faa, 'rb');
    sz = fread (fid,1 , 'double');
	a = cell(sz,1);
    for i=1:sz
         sz2 = fread (fid, 1, 'double');
         for j=1:sz2
            sz3 = fread (fid, 1, 'double'); 
            a{i}(j,:)= single(fread (fid, sz3, type));
         end
    end
  
    fclose(fid);
end