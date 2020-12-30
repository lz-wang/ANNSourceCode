#include <string>
#include <iostream>
#include <string.h>
#include "arguments.hpp"

using namespace std;

//--------------------------------------------------------------------
void get_arguments(int argc , char* argv[], size_t& voc_size, size_t& M, 
				   std::string& folder, std::string& dataset_str,
				   size_t& rerank, size_t& nkret, bool& debug, std::string& query_str,
				   bool& eval_sift1m)
{
	//! constructor of the class
	arguments args(argc, argv);

	//! for each argument you give:
	//!   (long name, default value, short name (optional), help text (optional), mandatory argument flag (optional) )
	//fn = args.get_arg_string("data_fn","","f","binary file with descriptors", true);

	dataset_str = args.get_arg_string("dataset","sift1m","d","the dataset to use");
	voc_size = args.get_arg_size_t("voc_size",1024,"v","size of the MI vocabulary");

	folder = args.get_arg_string("folder","data/sift1m.bin.lopq/","f","the folder to use");

	M = args.get_arg_size_t("M",8,"M","number of subquantizers");
	rerank = args.get_arg_size_t("rr",10000,"T","use reranking on the top X vectors");
	nkret = args.get_arg_size_t("nkret",10000,"knn","return this many nearest neighbors");

	query_str = args.get_arg_string("query","","q","single query mode");

	eval_sift1m = args.get_arg_bool("eval_sift1m",false,"sift1m","flag to evaluate sift1M dataset queries - requires pre-processing through MATLAB");

	debug = args.get_arg_bool("debug",false,"debug","flag to be verbose for debugging");

	//! also allow -h and --help as flags to display help. This has to be the last argument
	args.get_help_argument();

	//! check whether the minumum number of arguments acceptable is acheved, otherwise display help and exit
	args.check_min_narguments(0);

	//! check whether all mandatory arguments are set, otherwise display help and exit
	args.check_mandatory_arguments();
}
