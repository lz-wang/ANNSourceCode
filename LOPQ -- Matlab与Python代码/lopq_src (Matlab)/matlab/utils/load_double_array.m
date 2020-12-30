function a = load_double_array (faa,type)

    fid = fopen (faa, 'rb');
    sz = fread (fid,1 , 'double');
    sz2 = fread (fid, 1, 'double');
    
    a = zeros(sz,sz2, type);

    a(1,:)=fread (fid, sz2, type);
    for i=2:sz
         fread (fid, 1, 'double');
         a(i,:)= fread (fid, sz2, type);
     end
    
    fclose(fid);
end