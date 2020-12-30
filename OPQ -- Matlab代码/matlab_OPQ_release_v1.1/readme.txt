
***************************************************************************************
***************************************************************************************

Matlab demo code for "Optimized Product Quantization for Approximate Nearest Neighbor Search" (CVPR 2013)

by Kaiming He (kahe@microsoft.com), March 15, 2013

If you use/adapt our code, please appropriately cite our CVPR 2013 paper.

This code is for academic purpose only. Not for commercial/industrial activities.

***************************************************************************************
***************************************************************************************

Usage:
Run main.m to see the *distortion* of 5 methods: PQ-raw (on raw features), PQ-RO (random order), PQ-RR (random rotation), OPQ-P (our parametric), and OPQ-NP (our non-parametric with the best initialization).

Run main_ht.m to see comparisons with one more method: PQ-HT (balanced component variance using Householder transform).

Notice:
1. This demo is only for a quick understanding of the algorithms. The iterations numbers are very small so all algorithms have not yet converged. The datasets are very sparsely sampled.
2. This demo only shows the training stage of the algorithms. The testing/evaluation are the same as the original PQ paper, and not provided here.
3. The SIFT and GIST demo sets are randomly sampled from the sets in http://corpus-texmex.irisa.fr/. The MNIST demo set is from www.cs.toronto.edu/~norouzi/research/mlh/.

Any bug report or discussion is highly appreciated.
