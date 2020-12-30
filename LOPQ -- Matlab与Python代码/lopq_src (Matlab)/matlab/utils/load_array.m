function a = load_array (faa,type)

    fid = fopen (faa, 'rb');    
    sz = fread (fid,1 , 'double'); 
    a = zeros(sz,1, type);  
    a = fread (fid, sz, type); 
    fclose(fid); 
end

