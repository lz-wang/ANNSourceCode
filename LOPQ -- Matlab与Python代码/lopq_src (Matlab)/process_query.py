import sys, time
from numpy import *

# ---------------
# parameters
lopq_folder = "data/sift1m.bin.lopq/"
dataset = "sift1m"

# temporary output for query files
outfn = "tmp_query"

# ---------------

# load mi vocabularies
voc1 = genfromtxt(lopq_folder + "voc1.txt", dtype=float32)
voc2 = genfromtxt(lopq_folder + "voc2.txt", dtype=float32)

V = voc1.shape[0]
D2 = voc1.shape[1]
w = V;

# load rotations
R1 = [None] * V
R2 = [None] * V
mu1 = [None] * V
mu2 = [None] * V

for i in arange(V):
	R1[i] = genfromtxt(lopq_folder + "/R/lopq.c" + str(i+1) + ".R.txt", dtype=float32)
	R2[i] = genfromtxt(lopq_folder + "/R/lopq.c" + str(i+1+V) + ".R.txt", dtype=float32)
	mu1[i] = genfromtxt(lopq_folder + "/R/lopq.c" + str(i+1) + ".mu.txt", dtype=float32)
	mu2[i] = genfromtxt(lopq_folder + "/R/lopq.c" + str(i+1+V) + ".mu.txt", dtype=float32)
	
	
for line in sys.stdin:
	try:

		x = fromstring( line.strip(), dtype=float32, sep=' ')

		res1 = x[0:D2] - voc1
		res2 = x[D2:] - voc2
		
		sqd1 = (res1**2).sum(axis=1)
		sqd2 = (res2**2).sum(axis=1)
		idx1 = argsort(sqd1) # sorting
		idx2 = argsort(sqd2) # sorting
		sqd1=sqd1[idx1]
		idx1 = idx1 + 1
		sqd2=sqd2[idx2]
		idx2 = idx2 + 1
		
		idx1.astype(uint32).tofile( outfn + ".lbl.1.bin")
		idx2.astype(uint32).tofile( outfn + ".lbl.2.bin")
		sqd1.astype(float32).tofile( outfn + ".lbl.d.1.bin")
		sqd2.astype(float32).tofile( outfn + ".lbl.d.2.bin")
		
		proj1 = zeros( (V,D2), dtype=float32 )
		proj2 = zeros( (V,D2), dtype=float32 )

		i0 = 0;
		for i1 in idx1-1:
			proj1[i0,:] = dot( (res1[i1] - mu1[i1]), R1[i1])
			i0 = i0 + 1
		i0 = 0
		for i2 in idx2-1:
			proj2[i0,:] = dot( (res2[i2] - mu2[i2]), R2[i2])
			i0 = i0 + 1
		
		proj1.tofile( outfn + ".proj.1.bin")
		proj2.tofile( outfn + ".proj.2.bin")
		
		print ' - query processed ok'			
	except:
		print " - Error processing features! ";
