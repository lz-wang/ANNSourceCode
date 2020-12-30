% ---------------------------
% some more params here

Ks = 256;

% ---------------------------

Vstr = num2str(V);

lopq_folder = [data_fn '.lopq/'];
python_folder = lopq_folder;


[s,mess,messid] = mkdir(lopq_folder);
[s,mess,messid] = mkdir([lopq_folder '/subquantizers/']);
[s,mess,messid] = mkdir([lopq_folder '/queries_lazy/']);

if (export_python)
	[s,mess,messid] = mkdir(python_folder);
	[s,mess,messid] = mkdir([python_folder '/R/']);
	[s,mess,messid] = mkdir([python_folder '/sq/']);
end

addpath('yael_v371/matlab');
addpath('utils');

M2 = M/2;