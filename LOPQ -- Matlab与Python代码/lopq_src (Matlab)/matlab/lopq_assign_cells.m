
datat = load_double_array(data_fn, 'single');
D = size(datat,1);
N = size(datat,2);

C1 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.1.bin'],'single')'; 
C2 = load_double_array( [ lopq_folder 'mi_voc_'  Vstr '.2.bin'],'single')'; 

disp([' - LOPQ: Assigning cells for ' num2str(N) ' vectors...']);
box = zeros(N,1, 'uint32');
[tlbl1, ~] = yael_nn(C1, datat(1:D/2, :), 1, 2);
[tlbl2, ~] = yael_nn(C2, datat((D/2+1):D, :), 1, 2);
box = tlbl2 + (tlbl1-1)*V;
save_array([lopq_folder dataset '.mi.voc_'  Vstr '.bin'],box(:),'uint32');
[sbox idx] = sort(box);
save_array([lopq_folder dataset '.mi.voc_'  Vstr '.sbox.bin'],sbox,'uint32');
save_array([lopq_folder dataset '.mi.voc_'  Vstr '.idx.bin'],idx,'uint32');
nob = V*V;
box_start = zeros(nob,1);
box_end = zeros(nob,1);
val = sbox(1);
box_start( val )= 1;
for b=2:length(sbox)
	if (sbox(b) == val), continue; end
	box_end(val) = b - 1;
	val = sbox(b);
	box_start( val )= b;
end
box_end(val) = length(sbox);
disp([' sbox min max : ' num2str(min(sbox)) ' ' num2str(max(sbox)) ]);
disp([' box_start min max : ' num2str(min(box_start)) ' ' num2str(max(box_start)) ]);
disp([' box_end  min max : ' num2str(min(box_end)) ' ' num2str(max(box_end)) ]);
save_array([lopq_folder dataset '.mi.voc_' Vstr '.bs.bin'],box_start,'uint32');
save_array([lopq_folder dataset '.mi.voc_' Vstr '.be.bin'],box_end,'uint32');

clear datat;