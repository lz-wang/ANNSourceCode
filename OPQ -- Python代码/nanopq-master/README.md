# nanopq

[![Build Status](https://travis-ci.org/matsui528/nanopq.svg?branch=master)](https://travis-ci.org/matsui528/nanopq)
[![Documentation Status](https://readthedocs.org/projects/nanopq/badge/?version=latest)](https://nanopq.readthedocs.io/en/latest/?badge=latest)
[![PyPI version](https://badge.fury.io/py/nanopq.svg)](https://badge.fury.io/py/nanopq)
[![Downloads](https://pepy.tech/badge/nanopq)](https://pepy.tech/project/nanopq)

Nano Product Quantization (nanopq): a vanilla implementation of Product Quantization (PQ) and Optimized Product Quantization (OPQ) written in pure python without any third party dependencies.


## Installing
You can install the package via pip. This library works with Python 3.5+ on linux.
```
pip install nanopq
```

## [Documentation](https://nanopq.readthedocs.io/en/latest/index.html)
- [Tutorial](https://nanopq.readthedocs.io/en/latest/source/tutorial.html)
- [API](https://nanopq.readthedocs.io/en/latest/source/api.html)

## Example

```python
import nanopq
import numpy as np

N, Nt, D = 10000, 2000, 128
X = np.random.random((N, D)).astype(np.float32)  # 10,000 128-dim vectors to be indexed
Xt = np.random.random((Nt, D)).astype(np.float32)  # 2,000 128-dim vectors for training
query = np.random.random((D,)).astype(np.float32)  # a 128-dim query vector

# Instantiate with M=8 sub-spaces
pq = nanopq.PQ(M=8)

# Train codewords
pq.fit(Xt)

# Encode to PQ-codes
X_code = pq.encode(X)  # (10000, 8) with dtype=np.uint8

# Results: create a distance table online, and compute Asymmetric Distance to each PQ-code 
dists = pq.dtable(query).adist(X_code)  # (10000, ) 
```

## Author
- [Yusuke Matsui](http://yusukematsui.me)


## Reference
- [H. Jegou, M. Douze, and C. Schmid, "Product Quantization for Nearest Neighbor Search", IEEE TPAMI 2011](https://ieeexplore.ieee.org/document/5432202/) (the original paper of PQ)
- [T. Ge, K. He, Q. Ke, and J. Sun, "Optimized Product Quantization", IEEE TPAMI 2014](https://ieeexplore.ieee.org/document/6678503/) (the original paper of OPQ)
- [Y. Matsui, Y. Uchida, H. Jegou, and S. Satoh, "A Survey of Product Quantization", ITE MTA 2018](https://www.jstage.jst.go.jp/article/mta/6/1/6_2/_pdf/) (a survey paper of PQ) 
- [PQ in faiss](https://github.com/facebookresearch/faiss/wiki/Faiss-building-blocks:-clustering,-PCA,-quantization#pq-encoding--decoding) (Faiss contains an optimized implementation of PQ. [See the difference to ours here](https://nanopq.readthedocs.io/en/latest/source/tutorial.html#difference-from-pq-in-faiss))
- [Rayuela.jl](https://github.com/una-dinosauria/Rayuela.jl) (Julia implementation of several encoding algorithms including PQ and OPQ)
- [PQk-means](https://github.com/DwangoMediaVillage/pqkmeans) (clustering on PQ-codes. The implementation of nanopq is compatible to [that of PQk-means](https://github.com/DwangoMediaVillage/pqkmeans/blob/master/tutorial/1_pqkmeans.ipynb))
- [Rii](https://github.com/matsui528/rii) (IVFPQ-based ANN algorithm using nanopq)