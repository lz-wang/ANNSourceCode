function save_double_array (faa,a,type)

    fid = fopen (faa, 'wb');
    
    fwrite (fid, size(a,1), 'double');
  
    for i=1:size(a,1)
        fwrite (fid, size(a,2), 'double');
%         for j=1:size(a,2)
            fwrite (fid, a(i,:), type);
%         end
    end
    
    fclose(fid);
end