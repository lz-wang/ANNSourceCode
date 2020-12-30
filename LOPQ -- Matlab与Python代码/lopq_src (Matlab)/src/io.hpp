#ifndef __LOPQ_IO_HPP__
#define __LOPQ_IO_HPP__

#include <iostream>
#include <string>
#include <vector>

//---------------------------------------------------------
//loads array from binary file
template < class T >
void load_array(const std::string& bin_filename, std::vector<T>& data)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	if (!(file.is_open())) {
		std::cerr << "load_array: Error: cant open binary file: " << bin_filename << std::endl;
		data.resize(0);
		return;
	}

	double elements; //no of arrays
	file.read(reinterpret_cast<char *>(&elements), sizeof(elements));

	if (elements<1){
		std::cerr << "load_array: Error: binary file: " << bin_filename << " contains "
			<< elements << " elements!" << std::endl;
		data.resize(0);
		return;
	}

	data.resize((size_t)elements);

	T *buf = new T[(size_t)elements];
	file.read(reinterpret_cast<char *>(buf), (std::streamsize)elements * sizeof(T));
	for (size_t i=0; i < (size_t)elements; i++)
		data[i] = buf[i];

	delete buf;

	file.close();
}
//---------------------------------------------------------
//loads array from binary file
template < class T >
void load_array_python(const std::string& bin_filename, size_t sz, std::vector<T>& data)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	if (!(file.is_open())) {
		std::cerr << "load_array: Error: cant open binary file: " << bin_filename << std::endl;
		data.resize(0);
		return;
	}

	data.resize(sz);

	T *buf = new T[sz];
	file.read(reinterpret_cast<char *>(buf), (std::streamsize)sz * sizeof(T));
	for (size_t i=0; i < sz; i++)
		data[i] = buf[i];

	delete buf;

	file.close();
}
//---------------------------------------------------------
//loads array of arrays of equal size from binary file
template<class T>
void load_double_array(const std::string& bin_filename, std::vector< std::vector<T> >& data)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	if (!(file.is_open())) {
		std::cerr << "load_double_array: Error: cant open binary file: " << bin_filename << std::endl;
		data.resize(0);
		return;
	}

	double no_of_arrays; //no of arrays
	file.read(reinterpret_cast<char *>(&no_of_arrays), sizeof(no_of_arrays));

	if (no_of_arrays<1){
		std::cerr << "load_double_array: Error: binary file: " << bin_filename << " contains "
			<< no_of_arrays << " elements!" << std::endl;
		data.resize(0);
		return;
	}

	data.resize((size_t)no_of_arrays);
	double dim; //size of each array

	for (size_t m=0; m < (size_t)no_of_arrays; m++) {
		file.read(reinterpret_cast<char *>(&dim),sizeof(dim));
		std::vector<T>& dat = data[m];
		dat.resize((size_t)dim);

		T *buf = new T[(size_t)dim];
		file.read(reinterpret_cast<char *>(buf), (std::streamsize)dim * sizeof(T));
		for (size_t i=0; i < dim; i++)
			dat[i] = buf[i];

		delete buf;
	}
	file.close();
}
//---------------------------------------------------------
//loads array of arrays of equal size from binary file
template<class T>
void load_double_array_python(const std::string& bin_filename, size_t rows, size_t cols, std::vector< std::vector<T> >& data)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	if (!(file.is_open())) {
		std::cerr << "load_double_array: Error: cant open binary file: " << bin_filename << std::endl;
		data.resize(0);
		return;
	}

	data.resize(rows, std::vector<T>(cols));

	for (size_t m=0; m < rows; m++) {
		T *buf = new T[(size_t)cols];
		std::vector<T>& dat = data[m];
		file.read(reinterpret_cast<char *>(buf), (std::streamsize)cols * sizeof(T));
		for (size_t i=0; i < cols; i++)
			dat[i] = buf[i];

		delete buf;
	}
	file.close();
}
//---------------------------------------------------------
//loads array of arrays of equal size from binary stream
template<class T>
void load_double_array_transposed(const std::string& bin_filename, std::vector< std::vector<T> >& data, bool print_stuff = false)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	if (!(file.is_open())) {
		std::cerr << "load_double_array: Error: cant open binary file: " << bin_filename << std::endl;
		return;
	}

	double dimensions; //no of arrays
	file.read(reinterpret_cast<char *>(&dimensions), sizeof(dimensions));

	double no_of_arrays; //size of each array
	file.read(reinterpret_cast<char *>(&no_of_arrays),sizeof(no_of_arrays));

	if (print_stuff)
		std::cout << "loading...number of arrays: " << no_of_arrays << std::endl << "array dimensions: " << dimensions <<std::endl;

	data.resize((size_t)no_of_arrays, std::vector<T>(size_t(dimensions)) );
	if (print_stuff)
		std::cout << "resized...number of arrays: " << no_of_arrays << std::endl << "array dimensions: " << dimensions <<std::endl;

	for (size_t m=0; m < dimensions; m++) {

		T *buf = new T[(size_t)no_of_arrays];
		file.read(reinterpret_cast<char *>(buf), (std::streamsize)no_of_arrays * sizeof(T));

		for (size_t i=0; i < no_of_arrays; i++)
			data[i][m] = buf[i];

		delete buf;

		//! read again the num of arrays
		file.read(reinterpret_cast<char *>(&no_of_arrays),sizeof(no_of_arrays));
	}
	file.close();
	if (print_stuff)
		std::cout << "load_double_array: data loaded ok!" << std::endl;
}
//---------------------------------------------------------
//! loads a double array to pointer
//! IT MALLOCs INSIDE!!!
template<class T>
T* load_double_array_to_pointer(const std::string bin_filename)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	T* data = NULL;

	if (!(file.is_open())) {
		std::cerr << "load_double_array_flann: Error: cant open binary file: " << bin_filename << std::endl;
		return data;
	}

	//! load rows and colums to initalize pointer
	double rows;
	file.read(reinterpret_cast<char *>(&rows), sizeof(rows));
	double cols; //size of each array
	file.read(reinterpret_cast<char *>(&cols),sizeof(cols));

	data =  (T*)malloc(rows*cols*sizeof(T));

	T* ptr = data;
	for (size_t m=0; m < rows; m++) {
		if (m>0)
			file.read(reinterpret_cast<char *>(&cols),sizeof(cols));
		for (size_t i=0; i < cols; i++) {
			file.read(reinterpret_cast<char *>(ptr),sizeof(T));
			ptr++;
		}
	}

	return data;
}
//---------------------------------------------------------
//! loads triple array to vector of pointer
//! IT MALLOCs INSIDE!!!
template<class T>
void load_triple_array_to_pointer(const std::string bin_filename, std::vector< T* >& v)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	double no_of_arrays; //no of arrays
	file.read(reinterpret_cast<char *>(&no_of_arrays), sizeof(no_of_arrays));

	v.resize(size_t(no_of_arrays));

	for (size_t vv=0; vv < v.size(); vv++) {
		T* data = NULL;

		if (!(file.is_open())) {
			std::cerr << "load_double_array_flann: Error: cant open binary file: " << bin_filename << std::endl;
			return data;
		}

		//! load rows and colums to initalize pointer
		double rows;
		file.read(reinterpret_cast<char *>(&rows), sizeof(rows));
		double cols; //size of each array
		file.read(reinterpret_cast<char *>(&cols),sizeof(cols));

		data =  (T*)malloc(rows*cols*sizeof(T));

		T* ptr = data;
		for (size_t m=0; m < rows; m++) {
			if (m>0)
				file.read(reinterpret_cast<char *>(&cols),sizeof(cols));
			for (size_t i=0; i < cols; i++) {
				file.read(reinterpret_cast<char *>(ptr),sizeof(T));
				ptr++;
			}
		}
		v[vv] = data;
	}
}
//---------------------------------------------------------
//loads array of arrays of equal size from binary file
template<class T>
void load_triple_array(const std::string &bin_filename, std::vector< std::vector< std::vector<T> > >& data)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	if (!(file.is_open())) {
		std::cerr << "load_double_array: Error: cant open binary file: " << bin_filename << std::endl;
		return;
	}
	double no_of_arrays; //no of arrays
	file.read(reinterpret_cast<char *>(&no_of_arrays), sizeof(no_of_arrays));

	double dim; //size of each array
	data.resize((size_t)no_of_arrays);

	for (size_t m=0; m < no_of_arrays; m++) {
		file.read(reinterpret_cast<char *>(&dim),sizeof(dim));

		std::vector<std::vector<T> > &t1(data[m]);
		t1.resize((size_t)dim);
		double dim2; //size of each array

		for (size_t k=0; k < dim; k++) {
			file.read(reinterpret_cast<char *>(&dim2),sizeof(dim2));
			std::vector<T> &t2(t1[k]);
			t2.resize((size_t)dim2);

			T *buf = new T[(size_t)dim2];
			file.read(reinterpret_cast<char *>(buf), (std::streamsize)dim2 * sizeof(T));
			for (size_t i=0; i < dim2; i++)
				t2[i] = buf[i];
			delete buf;
		}
	}
	file.close();
}
//loads array of arrays of equal size from binary file
template<class T>
void load_triple_array(const std::string &bin_filename, std::vector< std::vector< std::valarray<T> > >& data)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	if (!(file.is_open())) {
		std::cerr << "load_double_array: Error: cant open binary file: " << bin_filename << std::endl;
		return;
	}
	double no_of_arrays; //no of arrays
	file.read(reinterpret_cast<char *>(&no_of_arrays), sizeof(no_of_arrays));

	double dim; //size of each array
	data.resize((size_t)no_of_arrays);

	for (size_t m=0; m < no_of_arrays; m++) {
		file.read(reinterpret_cast<char *>(&dim),sizeof(dim));

		std::vector<std::valarray<T> > &t1(data[m]);
		t1.resize((size_t)dim);
		double dim2; //size of each array

		for (size_t k=0; k < dim; k++) {
			file.read(reinterpret_cast<char *>(&dim2),sizeof(dim2));
			std::valarray<T> &t2(t1[k]);
			t2.resize((size_t)dim2);

			T *buf = new T[(size_t)dim2];
			file.read(reinterpret_cast<char *>(buf), (std::streamsize)dim2 * sizeof(T));
			for (size_t i=0; i < dim2; i++)
				t2[i] = buf[i];
			delete buf;
		}
	}
	file.close();
}

//----------------------------------------------------------------------
//loads array of arrays size from binary file
std::vector<size_t> load_double_array_sizes(const std::string& bin_filename, bool print_stuff = false)
{
	std::ifstream file(bin_filename.c_str(), std::ios::binary);

	double no_of_arrays; //no of arrays
	file.read(reinterpret_cast<char *>(&no_of_arrays), sizeof(no_of_arrays));
	if (print_stuff)
		std::cout << "file " << bin_filename << " contains " << no_of_arrays << " arrays! " << std::endl;
	double dimensions; //no of arrays
	file.read(reinterpret_cast<char *>(&dimensions), sizeof(dimensions));

	std::vector<size_t> sizes(2);
	sizes[0] = no_of_arrays;
	sizes[1] = dimensions;
	return sizes;
}
//----------------------------------------------------------------------
//! load codes to array
template <class US>
void load_codes(size_t M, const std::string& codes_fn, std::vector<CODE>& codes, bool debug = false, size_t chunksize = 10000000)
{
	using namespace std;

	if (debug)
		cout << "   - load_codes: " << codes_fn << endl;

	std::vector<size_t> sizes = load_double_array_sizes(codes_fn);
	size_t N = sizes[1];
	if (debug)
		cout << "   - load_codes: loading and inserting " << N << " codes of " << M << " Bytes in chunks" <<endl;

	if (N < chunksize) {
		if (debug)
			cout << "   - load_codes: N < chunksize (N=" << N <<") "<< std::endl;
		chunksize = N;//exit(17);
	}
	codes.resize(N);

	std::ifstream file(codes_fn.c_str(), std::ios::binary);

	CODE tmp_code;
	vector< US *> buf(M), tmpptr(M);
	for (size_t m=0; m < M; m++) {
		buf[m] = new US[chunksize];
		tmpptr[m] = buf[m];
	}

	for (size_t ch=0; ch < N; ch+=chunksize) {
		if (ch+chunksize >N)
			chunksize = N - ch;
		if (debug)
			if ( (ch % (10*chunksize) ) == 0 )
				cout << "   - load_codes: loading chunk [" << ch << " - " << ch+chunksize << "]" <<endl;
		file.seekg( 2*sizeof(double) + ch*sizeof(US), ios_base::beg);

		for (size_t m=0; m < M; m++) {
			tmpptr[m] = buf[m];
			file.read(reinterpret_cast<char *>(buf[m]), (std::streamsize)chunksize * sizeof(US));
			file.seekg( (N - chunksize)*sizeof(US) + sizeof(double)  , ios_base::cur);
		}

		for (size_t n=0; n < chunksize ; n++) {
			for (size_t m=0; m < M; m++) {
				tmp_code.data[m] =(*tmpptr[m]); tmpptr[m]++;
			}
			codes[ch + n] = tmp_code;
		}


	}
	for (size_t m=0; m < M; m++)
		delete buf[m];
	file.close();
}
//---------------------------------------------------------
//saves array into text file
template<class T>
void array2txt(const std::vector<T>& a, const std::string& bin_filename, bool print_stuff = false)
{
    std::ofstream fout(bin_filename.c_str(),std::ios::trunc);
    for(size_t i = 0; i < a.size(); i++)
        fout << a[i] << std::endl;
	fout.close();
}
//---------------------------------------------------------
//saves array into text file
inline
void array_float2txt(const std::vector<float>& a, const std::string& bin_filename)
{
    std::ofstream fout(bin_filename.c_str(),std::ios::trunc);
    for(size_t i = 0; i < a.size(); i++)
        fout << to_string(a[i]) << std::endl;
}
//---------------------------------------------------------
//saves array of array into text file
template<class T>
void double_array2txt(const std::vector<std::vector<T> >& a, const std::string& bin_filename, bool print_stuff = false)
{
    std::ofstream fout(bin_filename.c_str(),std::ios::trunc);
	for(size_t i = 0 ; i < a.size()-1 ; i++){
	    for(size_t j = 0 ; j < a[i].size()-1 ; j++)
			fout << a[i][j] << " ";
		fout << a[i][a[i].size()-1] << std::endl;
	}
    for(size_t j = 0 ; j < a[a.size()-1].size()-1 ; j++)
		fout << a[a.size()-1][j] << " ";
	fout << a[a.size()-1][a[a.size()-1].size()-1] << std::endl;
	fout.close();
}


#endif //__LOPQ_IO_HPP__
