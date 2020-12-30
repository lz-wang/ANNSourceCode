function save_array (faa,a,type)

    fid = fopen (faa, 'wb');
    
    fwrite (fid, length(a), 'double');
  
%    for i=1:length(a)
            fwrite (fid, a(:), type);
 %   end
    
    fclose(fid);
end