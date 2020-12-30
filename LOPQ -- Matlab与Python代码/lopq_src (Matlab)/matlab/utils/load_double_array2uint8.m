function a = load_double_array2uint8(faa,type, row)

fid = fopen (faa, 'rb');
sz = fread (fid,1 , 'double');
sz2 = fread (fid, 1, 'double');

if (nargin < 3)
    
    a = uint8(zeros(sz,sz2));

    a(1,:)=fread (fid, sz2, type);
    for i=2:sz
         fread (fid, 1, 'double');
         a(i,:)= fread (fid, sz2, type);
     end
    
    
else
type
sizeof(type)
    fseek (fid, (row-1)*sz2*sizeof(type), 'cof');
    fseek (fid, (row-1)*sizeof('double'), 'cof');
	a = fread (fid, sz2, type)';
end
fclose(fid);