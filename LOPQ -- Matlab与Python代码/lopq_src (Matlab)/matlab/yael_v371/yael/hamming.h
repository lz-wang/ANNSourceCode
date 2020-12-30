/*
Copyright © INRIA 2010-2011. 
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

This software is a computer program whose purpose is to provide 
efficient tools for basic yet computationally demanding tasks, 
such as find k-nearest neighbors using exhaustive search 
and kmeans clustering. 

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/


/* Hamming distances. The binary vector length should be a power of 8 */
#ifndef __hamming_h
#define __hamming_h

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef long long int64;


/* matching elements (those returned) */
typedef struct hammatch_s {
  int qid;        /* query id */
  int bid;        /* base id */
  uint16 score;   /* Hamming distance */
} hammatch_t;


/* Define individual Hamming distance for various sizes.
   The generic one is slow while optimization is available for specific sizes */

uint16 hamming_generic (const uint8 *bs1, const uint8 * bs2, int ncodes);


/* Define prototype if no SSE4.2 available, otherwise use the specific processor instructions */
#ifndef __SSE4_2__
uint16 hamming_32 (const uint32 * bs1, const uint32 * bs2);
uint16 hamming_64 (const uint64 * bs1, const uint64 * bs2);

#else  /* Use SSE 4.2 */
#include <nmmintrin.h>
#ifndef hamming_32
#define hamming_32(pa,pb) _mm_popcnt_u32((*((uint32 *) (pa)) ^ *((uint32 *) (pb))))
#endif
#ifndef hamming_64
#define hamming_64(pa,pb) _mm_popcnt_u64((*((uint64 *) (pa)) ^ *((uint64 *) (pb))))
#endif
#endif

#ifndef hamming_128
#define hamming_128(a,b)  (hamming_64(a,b)+hamming_64(((uint64 *) (a)) + 1, ((uint64 *) (b)) + 1))
#endif

#ifndef BITVECSIZE
#warning "# BITVECSIZE UNDEFINED. SET TO 128 BY DEFAULT." 
#define BITVECSIZE 128
#elif BITVECSIZE%8 != 0
#error "Only power of 8 are possible for BITVECSIZE"
#endif

#define BITVECBYTE (BITVECSIZE/8)

/* Define the Hamming distance by selecting the most appropriate function,
   using the generic version as a backup */
#if BITVECSIZE==32
#define hamming(a,b)  hamming_32((uint32*) (a), (uint32*) (b))

#elif BITVECSIZE==64
#define hamming(a,b)  hamming_64((uint64*) (a), (uint64*) (b))

#elif BITVECSIZE==128
#define hamming(a,b)  hamming_128(a,b)

#else
#define hamming(a,b) hamming_generic((uint8*) (a), (uint8*) (b), BITVECBYTE);
#endif



/* Compute a set of Hamming distances between na and nb binary vectors */
void compute_hamming (uint16 * dis, const uint8 * a, const uint8 * b, int na, int nb);
void compute_hamming_32 (uint16 * dis, const uint8 * a, const uint8 * b, int na, int nb);
void compute_hamming_64 (uint16 * dis, const uint8 * a, const uint8 * b, int na, int nb);
void compute_hamming_128 (uint16 * dis, const uint8 * a, const uint8 * b, int na, int nb);

/* The same but with a generic function */
void compute_hamming_generic (uint16 * dis, const uint8 * a, const uint8 * b, 
                              int na, int nb, int ncodes);

/* Threaded versions, when OpenMP is available */
#ifdef _OPENMP
void compute_hamming_thread (uint16 * dis, const uint8 * a, const uint8 * b, int na, int nb);
#endif /* _OPENMP */

/* Compute hamming distance and report those below a given threshold in a structure array */
void match_hamming_thres (const uint8 * qbs, const uint8 * dbs, int nb, int ht,
                          size_t bufsize, hammatch_t ** hmptr, size_t * nptr);

void match_hamming_thres_generic (const uint8 * qbs, const uint8 * dbs, 
                                  int nb, int ht, size_t bufsize, 
                                  hammatch_t ** hmptr, size_t * nptr, 
                                  size_t ncodes);

/* Compute all cross-distances between two sets of binary vectors 
   crossmatch_he2 is same as crossmatch_he, but includes 
   - twice the matches: match (i,j,h) also gives the match (j,i,h)
   - self-matches of the form (i,i,0)
*/
void crossmatch_he (const uint8 * dbs, long n, int ht,
                    long bufsize, hammatch_t ** hmptr, size_t * nptr);

void crossmatch_he2 (const uint8 * dbs, long n, int ht,
                    long bufsize, hammatch_t ** hmptr, size_t * nptr);

/* alternative variant with pre-allocated external memory.
   return number of elements for safety check. 
   Typical usage is to first invoke crossmatch_he_count, allocate memory,
   and then invoke crossmatch_he_prealloc */
void crossmatch_he_count (const uint8 * dbs, int n, int ht, size_t * nptr);
void crossmatch_he_count2 (const uint8 * dbs, int n, int ht, size_t * nptr);

int crossmatch_he_prealloc (const uint8 * dbs, long n, int ht,  
                            int * idx, uint16 * hams);
int crossmatch_he_prealloc2 (const uint8 * dbs, long n, int ht,  
                             int * idx, uint16 * hams);


#endif /* __hamming_h */
