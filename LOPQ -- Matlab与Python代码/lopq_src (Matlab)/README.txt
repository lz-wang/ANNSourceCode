-------------------------------------------------------------------
A demo implementation for Locally Optimized Product Quantization
by Yannis Kalantidis and Yannis Avrithis
as presented in CVPR 2014


-------------------------------------------------------------------
This package contains:
-------------------------------------------------------------------
- Matlab code for training local rotations & subquantizers, assigning and building multi-index.
- Matlab code for batch query preprocessing (needed eg to evaluate SIFT1M using the current implementation)
- Python code for query preprocessing
- non-optimized C++ code for ANN search using Multi-LOPQ
- the SIFT1M dataset from Inria TexMex ( http://corpus-texmex.irisa.fr/ ) in the proper format to train and evaluate it with the current code.

The code in this package is provided for demonstration purposes.

Keep in mind that for this approach to work properly, it requires to have enough data per multi-index vocabulary cell to learn the proper rotations. It is therefore suited for larger datasets, e.g. SIFT1B.

-------------------------------------------------------------------
Requirements:
-------------------------------------------------------------------
 - yael library (for linux 64, yael precompiled matlab binaries are included in this package, in any other case check the README at matlab/yael_v371/ for compilation instructions)
 - python numpy package (for python query processing interface)

-------------------------------------------------------------------
Compilation & Input files / binary format:
-------------------------------------------------------------------
 - To use the MATLAB training, your data should be in a specific binary format. To get your data in the proper format, load your data in matlab columnwise (i.e. in a DxN matrix) and save them using the functions found in [lopq_src]/matlab/utils/ with the MATLAB command:
		> save_double_array('[data_file]', data, 'single');

 - To compile the c++ code:
	$ make -B
 - you may need to recompile the yael library (type > Make in MATLAB)

-------------------------------------------------------------------
Usage:
-------------------------------------------------------------------

--------------------
Demo: Evaluate Multi-LOPQ on SIFT1M dataset:
	
	The inctructions in this subsection describe a way to make sure that the code compiles and runs in your machine and that it works for nearest neighbor search. To train and run Multi-LOPQ for any other dataset, see instructions below.
	
	- run lopq_main.m in MATLAB keeping the default parameter settings. When complete, should display: " - lopq: Training, indexing and assigning ended successfully"
	- run
	$ ./lopq -sift1m -debug

	- the output after 10K queries should be something like:
 	- lopq: SIFT1M - Recall at {1,10,100,1K,10K}:
		[0.5184 0.9271 0.9772 0.9774 0.9774]


--------------------
Training LOPQ, assigning and indexing (MATLAB):

To use Multi-LOPQ for your own/any dataset, you may use the instructions provided here to train the vocabularies and subquantizers, as well as create then multi-index files. Use the instruction below to query with a new point.
	
	- Convert your dataset to the required format (see instructions above) 
	- Change the parameters on the top section of "lopq_main.m" to fit your dataset. Some hints:
			-> set preprocess_queries_for_sift1m_evaluation = 0 if you dont have predefined queries, otherwise set it to 1 and also set the query_data_file parameter to point at the proper file with the query data.
			-> give a name to your dataset, you will use this as a parameter to load the proper files when you run the search binary
			-> set the vocabulary size according to the dataset size. For LOPQ o work properly, each of the cells must have enough data to train a projection and subquantizers. E.g. for a 1 Billion dataset, setting V around 4-16K is the usual choice, for a dataset of just 10-100 thousand points, a vocabulary of 64-256 should be enough.
			  
	- run lopq_main.m
	- When complete, should display: " - lopq: Training, indexing and assigning ended successfully"


--------------------
Searching with a new query (requires python/numpy):

	- change the dataset name and folder accordingly on the top part of process_query.py

	- With your query vector values in text form, run first the preprocessing step (temporary files will be created), e.g.:
	$ cat data/a_sift_vector.txt  | python process_query.py

	- run the binary with the proper parameters, e.g.:
	$ ./lopq -M 8 -v 1024 -f data/sift1m.bin.lopq/ -d sift1m -q tmp_query -T 10000 -knn 10

	- the output is a .txt file with the sorted nearest neighbor ids
	
	- for explanation of the parameters of the lopq binary, run:
	$ ./lopq -h

NOTE: The python script and the c++ binary with the [-q] argument, as provided here, both work with a single query as input, but are easily extended to multiple.

