#ifndef __STD_UTILITIES_HPP__
#define __STD_UTILITIES_HPP__

#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <vector>
#include <valarray>
#include <string>
#include <set>
#define PI 3.14159265

//--------------------------------------------------------
//T to string
template<class T>
inline std::string to_string(const T& i)
{
		std::stringstream numm;
		std::string S;
		numm << i;
		numm >> S;
		return S;
}

//---------------------------------------------------------------------
template <class T>
inline void print(const std::vector<T> &a)
{
	using namespace std;
	//cout <<"[ ";
	for (size_t i=0; i < a.size(); i++)
		cout << a[i] << " ";
	//cout << "]" <<endl;
	cout << endl;
}
//---------------------------------------------------------------------
template <class T>
inline std::string to_string(const std::vector<T> &a)
{
	using namespace std;
	std::string s;
	for (size_t i=0; i < a.size()-1; i++)
		s += to_string(a[i]) + " ";
	s += a[a.size()-1];
	return s;
}
//---------------------------------------------------------------------
template <class T>
inline float euclidean_dist_unsqrt(const std::valarray<T> &a,const std::valarray<T> &b)
{
	float dist(0);
	size_t N = a.size();
	for(size_t i=0;i<N;++i) {
		T d = a[i]-b[i];
		dist+= d*d;
	}

return dist;
}
//---------------------------------------------------------------------
//! boolean function that returns true if element a is found in set s
template <class T> inline
void decreasebyone(std::vector<T>& v)
{
	for (size_t i=0; i < v.size(); i++)
		v[i]--;
}
//---------------------------------------------------------------------
//! returns true if x does exist in array a
template<class T>
bool in(const std::vector<T>& a, T x)
{
	for (size_t i = 0; i < a.size(); i++)
		if (a[i] == x)
			return true;
	return false;
}
//---------------------------------------------------------------------

#endif //__STD_UTILITIES_HPP__
