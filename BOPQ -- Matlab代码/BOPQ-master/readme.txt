---------------------------------------------------------------------------------------

This toolset contains the Matlab demo code for "Bilinear Optimized Product Quantization for Scalable Visual Content Analysis" (IEEE Transactions on Image Processing).

by Litao Yu (l.yu4@uq.edu.au)

This demo code is only for a quick understanding of our proposed algorithms, which is for academic purpose only and not for commercial/industrial activities. Please kindly cite our paper if you use our code.

---------------------------------------------------------------------------------------

Usage:

Please first prepare the training data by a matrix with either single or double data type. Then call one of the three functions: 
	1. train_bopq_np, the non-parametric solution for global BOPQ;
	2. train_bopq_p, the parametric solution for global BOPQ;
	3. train_blopq_p, the solution for local BOPQ.


Notice:
The demo code only shows the training stage of the algorithms. The testing/evaluation are the same as the original PQ paper "Product Quantization", and not provided here. However, you can refer to http://people.rennes.inria.fr/Herve.Jegou/projects/ann.html, and directly use their codes for evaluation.
