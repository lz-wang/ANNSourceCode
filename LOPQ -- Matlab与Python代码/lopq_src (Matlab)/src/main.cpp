#include <get_arguments.hpp>
#include <lopq.hpp>

using namespace std;

//! function for evaluating sift1m
std::vector<double> evaluate_sift1m_recall_1_10000(const std::vector< std::vector<unsigned int> >& res, const std::string& ground_truth_file = "data/sift1m_gt_1nn.bin");


int main(int argc , char* argv[])
{

	size_t V, M, T, knn;
	string folder, dataset, query_str;
	bool debug, eval_sift1m, single_query = false;

	//! get command line arguments
	get_arguments(argc, argv, V, M, folder, dataset, T, knn, debug, query_str, eval_sift1m);

	lopq_index lopq(M, V);

	if (debug) {
		cout << " - lopq: Dataset: " << dataset << endl;
		cout << " - lopq: Folder in use: " << folder << endl;
		cout << " - lopq: T = " << T << endl;
	}

	//! load index and data
	lopq.load(folder, dataset, debug);

	//! check if single query
	if (query_str.size()>0)
		single_query = true;
	//! for the evaluation Recall@{1,10,100,1K,10K} is returned so knn should be 10K
	if (eval_sift1m)
		knn = 10000;


	if (debug)
		cout << " - lopq: Memory per compressed vector is "<< M*8 << " bits or " << M << " bytes" << endl;

	//! queries containers
	std::vector< std::vector< unsigned int > > query_mi_lbl_1, query_mi_lbl_2;
	std::vector< std::vector< float > > query, query_mi_lbl_1_d, query_mi_lbl_2_d;
	size_t w = V;
	std::vector< std::vector<float> > query_proj, tmpq, query1, query2;
	std::vector< std::vector<unsigned int> > qlbl;
	std::vector< std::vector< int > > qlbl_pos, qlbl_pos1, qlbl_pos2;
	std::vector<float> query0;
	size_t noq = 0;
	size_t D2 = lopq.dim()/2;

	if (single_query) {
		query_mi_lbl_1.resize(1); query_mi_lbl_2.resize(1); query_mi_lbl_1_d.resize(1); query_mi_lbl_2_d.resize(1);
		load_array_python<unsigned int>( query_str +".lbl.1.bin", V, query_mi_lbl_1[0]);
		load_array_python<unsigned int>( query_str +".lbl.2.bin", V, query_mi_lbl_2[0]);
		load_array_python<float>( query_str +".lbl.d.1.bin", V, query_mi_lbl_1_d[0]);
		load_array_python<float>( query_str +".lbl.d.2.bin", V, query_mi_lbl_2_d[0]);
		noq = 1;
	} else {
		if (debug)
			cout <<" - lopq: loading query files from files like: " << string(folder +"/" + dataset + "_query.mi.voc_" +to_string(V)+ ".1.bin") << endl;
		load_double_array<unsigned int>( folder +"/" + dataset + "_query.mi.voc_" +to_string(V)+ ".1.bin", query_mi_lbl_1);
		load_double_array<unsigned int>( folder +"/" + dataset + "_query.mi.voc_" +to_string(V)+ ".2.bin", query_mi_lbl_2);
		load_double_array<float>( folder +"/" + dataset + "_query.mi.voc_" +to_string(V)+ ".1.d.bin", query_mi_lbl_1_d);
		load_double_array<float>( folder +"/" + dataset + "_query.mi.voc_" +to_string(V)+ ".2.d.bin", query_mi_lbl_2_d);
		noq = query_mi_lbl_1.size();
		if (debug)
			cout << " - lopq: loaded for " << noq << " query vectors."<< endl;
	}

	//! container for resulting ids
	std::vector< std::vector< unsigned int > > result(noq, std::vector<unsigned int>(knn));

	for(size_t q=0; q < noq; q++) {
		if (debug)
			cout << "   - q: " << q << "/"  << noq << ", start..." <<  endl;
		std::vector< int > qqlbl_pos1(V, -1);
		std::vector< int > qqlbl_pos2(V, -1);
		if (single_query) {
			load_double_array_python<float>( query_str +".proj.1.bin", V, D2, query1);
			load_double_array_python<float>( query_str +".proj.2.bin", V, D2, query2);
		} else {
			load_double_array_transposed<float>(folder + "/queries_lazy/" + dataset + "_query." + to_string(q+1) + ".1.bin", query1);
			load_double_array_transposed<float>(folder + "/queries_lazy/" + dataset + "_query." + to_string(q+1) + ".2.bin", query2);
		}
		const std::vector<unsigned int>& qlbl1q = query_mi_lbl_1[q];
		const std::vector<unsigned int>& qlbl2q = query_mi_lbl_2[q];
		for (size_t k =0; k < w;k++) {
			qqlbl_pos1[ qlbl1q[k]-1 ] = k;
			qqlbl_pos2[ qlbl2q[k]-1 ] = k;
		}
		lopq.search(w,
				query1, query2,	qqlbl_pos1, qqlbl_pos2,
				query_mi_lbl_1[q], query_mi_lbl_2[q],
				query_mi_lbl_1_d[q], query_mi_lbl_2_d[q],
				T, knn, debug);

		result[q] = lopq.result_ids;

		//print(lopq.result_ids);
		//print(lopq.result_dist);

	}

	if (eval_sift1m) {
		vector<double> recall = evaluate_sift1m_recall_1_10000(result);
		cout << " - lopq: SIFT1M - Recall at {1,10,100,1K,10K}: " << endl;
		print(recall);
	} else {
		string result_txt =  dataset + "." + to_string(M) + "." + to_string(V) + ".T" + to_string(T) +".k" + to_string(knn)+".txt";
		double_array2txt(result,result_txt);
		if (debug)
			cout << " - lopq: Output saved as txt at " << result_txt << endl;
	}



	return 0;
}

std::vector<double> evaluate_sift1m_recall_1_10000(const std::vector< std::vector<unsigned int> >& res, const std::string& ground_truth_file)
{
	// load_groundtruth
	std::vector< unsigned int > nn;
	load_array<unsigned int>(ground_truth_file, nn);

	unsigned int nq = res.size();
	double nqd = double(nq);
	unsigned int res_sz = res[0].size();
	std::vector<double> recall(5, 0.0);

	for (unsigned int q=0; q < nq; q++) {
		const std::vector<unsigned int>& r = res[q];
		unsigned int this_nn = nn[q];
		for (unsigned int k=0; k < res_sz; k++){
			if (r[k] == this_nn) {
				if (k < 1) recall[0]++;
				if (k < 10) recall[1]++;
				if (k < 100) recall[2]++;
				if (k < 1000) recall[3]++;
				if (k < 10000) recall[4]++;
				break;
			}
		}
	}

	for (size_t i = 0; i<5; i++)
		recall[i] /= nqd;

	return recall;
}
