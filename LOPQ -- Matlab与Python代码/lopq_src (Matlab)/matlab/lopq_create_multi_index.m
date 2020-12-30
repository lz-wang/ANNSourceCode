
% create codebooks

learn_set = load_double_array(data_fn, 'single');

D = size(learn_set,1)
N = size(learn_set,2)

ntrain = N;

disp([' - LOPQ: create multi-index: creating multi-index codebooks from ' num2str(size(learn_set,2)) ' vectors of dimension ' num2str(D)]);
disp([' - LOPQ: create multi-index: clustering...']);

C1 = yael_kmeans(learn_set(1:D/2, :), V, 'niter', niterat, 'redo', kmeansntimes);
save_double_array( [ lopq_folder 'mi_voc_'  Vstr '.1.bin'],C1','float'); 
dlmwrite([python_folder 'voc1.txt'], C1', 'delimiter', ' ', 'precision', 8);

C2 = yael_kmeans(learn_set((D/2+1):D, :), V, 'niter', niterat, 'redo', kmeansntimes);
save_double_array( [ lopq_folder 'mi_voc_'  Vstr '.2.bin'],C2','float'); 
dlmwrite([python_folder 'voc2.txt'], C2', 'delimiter', ' ', 'precision', 8);

clear learn_set;