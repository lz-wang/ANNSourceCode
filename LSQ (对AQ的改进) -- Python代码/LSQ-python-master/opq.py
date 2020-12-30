import numpy as np
from sklearn.cluster import MiniBatchKMeans, KMeans
from utils import *
from sklearn.metrics import mean_squared_error
import multiprocessing as mp
from scipy.cluster.vq import kmeans2


def quantize_pq(B, C):
    M, _, d = C.shape
    N, _ = B.shape
    X = np.zeros([N, d*M])
    for i in range(M):
        X[:, d*i:d*(i+1)] = C[i, B[:, i]]
    return X

def kmean_pool(tup):
    k, it, init, X, logging = tup
    # result = MiniBatchKMeans(n_clusters=k, max_iter=it, init=init, n_init=1).fit(X)
    # kmeans = KMeans(n_clusters=k, init=init, n_init=1, max_iter=it, n_jobs=1).fit(X)
    # return kmeans2(X, init, it, minit='matrix')
    return kmeans2(X, k, it, minit='points')
    
    # dist = sqdist(X, kmeans.cluster_centers_)
    # idx = dist.argmin(axis=1).reshape(-1)
    # if type(logging) == int:
    #     print(f'subspace {logging}')
    # return result.cluster_centers_, result.labels_ 

def train_opq_np(X, pool, M, k=256, niter=100):
    """Product Quantization, ported to python
    using X to get M codebooks, each has
    k codewords
    Parameters
    ----------
    X : ndarray
        the input, [N, D]
    M : int
        the codebook num
    R_init : ndarray
        the inital orthogonal matrix, [D, D]
    centers_table_init : ndarray
        the inital centers table by PQ, [k, D]
    k : int, optional
        actually k*, codeword num for each book (the default is 256)
    iter_in : int, optional
        the iter of pq, because using pre-computed centers table, this can be small (the default is 100)
    iter_out : int, optional
        the iter of R computation(the default is 100)

    Returns
    -------
    centers_table : list
        a [k, D] matrix, the codebooks, the slice d = D // M of 2nd dim is combined
    B : a [N, M] matrix
        the index of nearest center for each split in X
    R_opq_np : ndarray
        the R, [D, D]
    distortion : double
        the total mean square distance
    """
    D = X.shape[1]
    d = D // M

    B = np.zeros((X.shape[0], M), dtype=np.int32)

    C = np.random.randn(M, k, d)

    R = np.eye(D)

    print(f"OPQ-NP training on {X.shape} datas")

    for i in range(niter):
        Xproj = X.dot(R)
        # NOTE: the start and end in matlab is different

        res = pool.map(kmean_pool, [(k, 20, C[m], Xproj[:, m*d:(m+1)*d], m) for m in range(M)])

        '''
        for m in range(M):
            print("subspace #: %d" % m)
            Xsub = Xproj[:, m*d:(m+1)*d]
            # kmeans = KMeans(n_clusters=k, init=C[m*k:(m+1)*k, m*d:(m+1)*d], n_init=1, max_iter=100, n_jobs=-1).fit(Xsub)
            # kmeans = MiniBatchKMeans(n_clusters=k, max_iter=1, init=C[m], n_init=1).fit(Xsub)

            # [k, d]
            centers = kmeans.cluster_centers_

            C[m] = centers

            # [N, k] =   [N, d] [k, d]
            dist = sqdist(Xsub, centers)

            # [N, ]
            idx = dist.argmin(axis=1).reshape(-1)

            B[:, m] = idx
        '''
        for m in range(M):
            C[m] = res[m][0]
            B[:, m] = res[m][1]

        quantized = quantize_pq(B, C)
        distortion = np.mean(np.sum(np.square(Xproj - quantized), 1))
        print("[%3d]: e %4e" % (i+1, distortion))
        R_opq_np = R

        U, _, Vh = np.linalg.svd(X.T.dot(quantized), full_matrices=False)
        R = U.dot(Vh)

    return C, B, R_opq_np, distortion