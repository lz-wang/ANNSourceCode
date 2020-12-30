%% ---------------------------
%% definitions & general parameters

% data file (saved with save_double_array - one datapoint per column)
data_fn = '../data/sift1m.bin';

% a name for the dataset (used as a prefix for saving files)
dataset = 'sift1m';


% codebook size
V = 1024;

% number of PQ buckets (ie number of Bytes per compressed vector in memory)
M = 8;

% also save txt (needed for query processing with python)
export_python = 1;

% some more params and outpout folders in here:
set_params;

% k-means iterations
niterat = 10;
% run k-means this number of times and keep best model
kmeansntimes = 1; 

% k-means iterations for subquantizers
niter_local = 20;

% for SIFT1M evaluation:
preprocess_queries_for_sift1m_evaluation = 1;
query_data_file = '../data/sift1m_query.bin';


%% ---------------------------
	
% create multi-index vocabularies
lopq_create_multi_index;

% assign cells to dataand create the multi-index
lopq_assign_cells;

% get local rotations per cell
lopq_get_cov;

% create the local subquantizers
lopq_create_local_subquantizers;

% get codes for data
lopq_get_codes;

% pre-process queries (eg for SIFT1M)
if (preprocess_queries_for_sift1m_evaluation)
	lopq_get_query_lbl;
end

disp(' - lopq: Training, indexing and assigning ended sucessfully');
%% ---------------------------
