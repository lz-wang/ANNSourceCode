
include("../src/read/read_datasets.jl")
include("../src/utils.jl")
include("../src/opq/OPQ.jl");
include("../src/linscan/Linscan.jl");

function demo_opq(
  dataset_name="SIFT1M",
  nread::Integer=Int(1e4)) # Increase this to 1e5 to use the full dataset

  # === Hyperparams ===
  m       = 8
  h       = 256
  verbose = true
  nquery  = Int(1e4)
  knn     = Int(1e3) # Compute recall up to
  b       = Int( log2(h) * m )
  niter   = 10

  # === Train ===
  x_train              = read_dataset(dataset_name, nread )
  C, B, R, train_error = train_opq(x_train, m, h, niter, "natural", verbose)
  @printf("Error in training is %e\n", train_error[end])

  # === Encode the base set ===
  nread_base = Int(1e6)
  x_base     = read_dataset(dataset_name * "_base", nread_base )
  B_base     = quantize_opq( x_base, R, C, verbose )
  base_error = qerror_opq( x_base, B_base, C, R )
  @printf("Error in base is %e\n", base_error)

  # === Compute recall ===
  x_query = read_dataset( dataset_name * "_query", nquery, verbose )
  gt      = read_dataset( dataset_name * "_groundtruth", nquery, verbose )
  if dataset_name == "SIFT1M" || dataset_name == "GIST1M"
    gt = gt + 1;
  end
  gt = convert( Vector{UInt32}, gt[1,1:nquery] )
  B_base  = convert( Matrix{UInt8}, B_base-1 )

  print("Querying m=$m ... ");
  @time dists, idx = linscan_opq( B_base, x_query[:,1:nquery], C, b, R, knn )
  println("done");

  rec = eval_recall( gt, idx, knn );

end # function train_process

# train
demo_opq()
