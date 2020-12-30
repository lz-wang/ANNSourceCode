#ifndef __LOPQ_HPP__
#define __LOPQ_HPP__

#include <valarray>
#include <map>

//----------------------------------------------------------------------
//! definitions of hard-coded parameters
#define Ks 256
#define MAX_CODE_SIZE 32
#define SUBQ_FOLDER "/subquantizers/"

//----------------------------------------------------------------------
//! code container
struct CODE {
	unsigned char data[MAX_CODE_SIZE];
};
#include <io.hpp>

//----------------------------------------------------------------------
class lopq_index
{
public:
	//! constructor
	lopq_index(size_t _M, size_t _V) : M(_M), V(_V)
		{ V2 = 2*_V; M2 = M/2; };

	//! load function
	void load(const std::string& folder, const std::string& dataset, bool debug = false);

	//! search function
	void search(size_t w, const std::vector< std::vector<float> >& query1,
			const std::vector< std::vector<float> >& query2,
			const std::vector<int>& qlbl_pos1,
			const std::vector<int>& qlbl_pos2,
			const std::vector<unsigned int>& lbl_1, const std::vector<unsigned int>& lbl_2,
			const std::vector<float>& lbl_1_d, const std::vector<float>& lbl_2_d,
			size_t rerank = 100000, size_t k = 10000, bool debug = false);

	//! search results
	std::vector<unsigned int> result_ids;
	std::vector<float> result_dist;
	size_t res_row_max, res_col_max;

	//1 return dimension
	size_t dim() { return Ds * M; }

private:

	//! value for M - Bytes per vector
	size_t M, M2;

	//! size of multi-index vocabularies
	size_t V, V2;

	//! number of points indexed
	size_t N;

	//! number of dimensions per bucket
	size_t Ds;

	//! main LOPQ files folder
	std::string folder;

	//! vocabularies
	std::vector< std::vector<float> > voc1;
	std::vector< std::vector<float> > voc2;
	//! rotations for all cells
	std::vector< std::vector< std::vector<float> > > LOPQ_R;
	std::vector< std::vector<float> > LOPQ_mu;
	//! subquantizers
	std::vector< std::vector< std::vector< std::valarray<float> > > > subquantizers;
//	std::vector< std::vector< float* > > subquantizers;

	//! multi-index
	std::vector< CODE > data_codes;
	std::vector< unsigned int > mi_idx;
	std::vector< unsigned int > mi_bs;
	std::vector< unsigned int > mi_be;

	//! get multi-index box from cell labels
	inline size_t get_box(unsigned int i, unsigned int j) { return (j + (i-1)*V - 1); }

	//! compute a lookup table - core
	void lookup_table_core(const std::valarray<float>& q, const std::vector<std::valarray<float> >& subq, std::vector<float>& dist)
	{
		size_t KKs =subq.size();
		dist.resize(KKs);
		for (size_t k = 0; k < KKs; k++)
			dist[k] = euclidean_dist_unsqrt(q, subq[k]);
	};

	//! compute a lookup table
	inline void lookup_table(const std::vector<float>& q, size_t sq, std::vector< std::vector<float> >& codes)
	{
		//std::thread t1();
		//! initialize output
		codes.resize(M2);//, std::vector<float>(Ks) );
		const std::vector<std::vector< std::valarray<float> > >& subq = subquantizers[sq];
		std::valarray<float> res(Ds);
		for (size_t m = 0; m < M2; m++) {
			size_t start_dim = m*Ds;
			for (size_t i=start_dim;i<start_dim+Ds;i++)
				res[i-start_dim] = q[i];
			lookup_table_core(res, subq[m], codes[m]);
		}
	}
};

//----------------------------------------------------------------------
//! load index and required data
void lopq_index::load(const std::string& folder, const std::string& dataset, bool debug)
{
	using namespace std;

	//! load codes
	string codes_fn = folder + "/" + dataset + "_codes."+to_string(V)+"."+to_string(M)+"."+to_string(Ks)+".us.adc.ms.bin";
	std::vector<size_t> sizes = load_double_array_sizes(codes_fn);
	N = sizes[1];
	load_codes<unsigned short>(M, codes_fn, data_codes, debug);
	//cout << "lala" <<endl;
	//! load multi-index
	string mi_idx_fn = folder +"/" + dataset + ".mi.voc_" +to_string(V)+ ".idx.bin";
	string mi_bs_fn = folder +"/" + dataset + ".mi.voc_" +to_string(V)+ ".bs.bin";
	string mi_be_fn = folder +"/" + dataset + ".mi.voc_" +to_string(V)+ ".be.bin";
	load_array<unsigned int>(mi_idx_fn, mi_idx);
	load_array<unsigned int>(mi_bs_fn, mi_bs);
	load_array<unsigned int>(mi_be_fn, mi_be);
	decreasebyone(mi_idx);
	decreasebyone(mi_bs);
	decreasebyone(mi_be);
	if (debug) {
		cout << " - lopq_index::load: Loading Multi-Index data for " << N << " vectors (MI voc size: " << V << ")" << endl;
		cout << " - lopq_index::load: Loading Multi-Index indices from file: " << mi_idx_fn << endl;
		cout << " - lopq_index::load: Loading Multi-Index codes: " << endl;
	}

	//! load subquantizers
	subquantizers.resize(V2);
	string subq_folder = folder + SUBQ_FOLDER;
	string extension = "." +  to_string(V) + "." + to_string(M) + "." + to_string(Ks) + ".adc.bin";
	for (size_t i = 0 ; i < V ; i++) {
		load_triple_array<float>(subq_folder + "/subquantizers.1.c" + to_string(i+1) + extension, subquantizers[i]);
		load_triple_array<float>(subq_folder + "/subquantizers.2.c" + to_string(i+1) + extension, subquantizers[i+V]);
	}
	Ds = subquantizers[0][0][0].size();

	if (debug)
		cout << " - lopq_index::load: Ds = " << Ds << endl;

	//! load vocabularies
	if(debug)
		cout << " - lopq_index::load: Loading Multi-Index vocs: " << endl;
	load_double_array<float>(folder + "/mi_voc_" + to_string(V) + ".1.bin", voc1);
	load_double_array<float>(folder + "/mi_voc_" + to_string(V) + ".2.bin", voc2);

	//! load rotations
	load_triple_array<float>(folder + "mi_LOPQ.bin", LOPQ_R);
	load_double_array<float>(folder + "mi_mu.bin", LOPQ_mu);

}
//----------------------------------------------------------------------
//! search via LOPQ multi-index
void lopq_index::search(size_t w, const std::vector< std::vector<float> >& query1,
	const std::vector< std::vector<float> >& query2,
	const std::vector<int>& qlbl_pos1,
	const std::vector<int>& qlbl_pos2,
	const std::vector<unsigned int>& lbl_1, const std::vector<unsigned int>& lbl_2,
	const std::vector<float>& lbl_1_d, const std::vector<float>& lbl_2_d,
	size_t rerank, size_t k, bool debug)
{
	using namespace std;


	if (N == 0) {
		cerr << " - lopq::search: Error! Index is empty!" << endl;
		return;
	}
	std::map< float, unsigned int > res;
	std::map< float, unsigned int >::iterator res_it = res.begin();

	CODE* codesj;

	std::multimap<float, std::pair<size_t, size_t> > prq;

	std::vector<bool> lookup1(V,true);
	std::vector<bool> lookup2(V,true);
	std::vector< std::vector< std::vector<float> > > lt1(V);
	std::vector< std::vector< std::vector<float> > > lt2(V);

	std::vector<bool> traversed(V*V, false);
	prq.insert( std::pair<float, std::pair<size_t, size_t> > ( lbl_1_d[0] + lbl_2_d[0],  std::pair<size_t, size_t>(0, 0) ) );

	size_t c=0, row=0, col=0;
	res_row_max=0, res_col_max=0;

	while (c < rerank) {
		if ( prq.size() == 0 ) {
			cerr << " - lopq: Error: priority queue is empty : " << prq.size() <<endl;
			break;
		}
		row = (*(prq.begin())).second.first;
		col = (*(prq.begin())).second.second;
		prq.erase( prq.begin() );

		if( row > res_row_max) res_row_max = row;
		if( col > res_col_max) res_col_max = col;
		if ( (row >=w ) || (col>=w) ) continue;
		size_t box = get_box(lbl_1[row], lbl_2[col]);

		if ( traversed[ box ] ) continue;
		traversed[ box ] = true;

		size_t bstart = mi_bs[box];
		if (bstart < N) {
			size_t bend = mi_be[box];

			size_t lbl1 = lbl_1[row]-1;
			size_t lbl2 = lbl_2[col]-1;
		//	tlt.tic();
			if (lookup1[lbl1]) {
				lookup_table(query1[ qlbl_pos1[lbl1] ], lbl1, lt1[lbl1]);
				//lookup_table_thread(query1[ qlbl_pos1[lbl1] ], lbl1, lt1[lbl1]);
				lookup1[lbl1]=false;
			}
			if (lookup2[lbl2]) {
				lookup_table(query2[ qlbl_pos2[lbl2] ], lbl2+V, lt2[lbl2]);
				//lookup_table_thread(query2[ qlbl_pos2[lbl2] ], lbl2+V, lt2[lbl2]);
				lookup2[lbl2]=false;
			}

			const std::vector<std::vector<float> >& dists1 = lt1[lbl1];
			const std::vector<std::vector<float> >& dists2 = lt2[lbl2];

			for (size_t i=bstart; i <= bend; i++) {
				size_t id = mi_idx[i];
				codesj = &data_codes[id];

				float dist = 0;
				for (size_t m = 0; m < M2; m++)
					dist += dists1[m][codesj->data[m]] + dists2[m][codesj->data[M2+m]];

				res.insert( std::pair<float, unsigned int>(dist, id) );
				c++;
			} // for each point in cell
		}
		//! get next box - MultiSequence
		if ( (col < V-1) && ( (row==0) ) )
			prq.insert( std::pair<size_t, std::pair<size_t, size_t> > (( lbl_1_d[row] + lbl_2_d[col+1] ),  std::pair<size_t, size_t>(row, col+1) ) );
		if ( (col < V-1) && (row>0) && ( traversed[ get_box(lbl_1[row-1], lbl_2[col+1]) ] ) )
			prq.insert( std::pair<size_t, std::pair<size_t, size_t> > (( lbl_1_d[row] + lbl_2_d[col+1] ),  std::pair<size_t, size_t>(row, col+1) ) );
		if ( (row < V-1) && ( (col==0) ) )
			prq.insert( std::pair<size_t, std::pair<size_t, size_t> > (( lbl_1_d[row+1] + lbl_2_d[col] ),  std::pair<size_t, size_t>(row+1, col) ) );
		if ( (row < V-1) && (col>0) && ( traversed[ get_box(lbl_1[row+1], lbl_2[col-1]) ] ) )
			prq.insert( std::pair<size_t, std::pair<size_t, size_t> > (( lbl_1_d[row+1] + lbl_2_d[col] ),  std::pair<size_t, size_t>(row+1, col) ) );

	} // while not enough points have been checked

	result_ids.resize(k, (unsigned int)(-1) );
	result_dist.resize(k, -1.0);
	size_t cmin = std::min(k, c);
	res_it = res.begin();
	for (size_t i = 0 ; i < cmin; i++, res_it++) {
		result_ids[i] = (*res_it).second;
		result_dist[i] = (*res_it).first;
	}
}

#endif //__LOPQ_HPP__
