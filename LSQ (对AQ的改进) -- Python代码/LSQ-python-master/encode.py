import numpy as np
from sift import *
from lsq import LSQ, C_update_tf
from opq import train_opq_np
import multiprocessing as mp
from utils import quantization_error, read_config
import os
import json
import time

def save(B, M, K):
    B = np.copy(B)
    B -= np.arange(0, M*K, K)[None, :]
    assert np.all(B < 256)
    np.save('./result/B.npy', B.astype(np.uint8))

def encode():
    config = read_config()

    base, _, _, _ = sift1m_read()

    D = base.shape[1]

    ENCODE_ITER, ILS_ITER, ICM_ITER, RAND_ORD, N_PERT, M, K, ILS_BATCH = config.ENCODE_ITER, config.ILS_ITER, config.ICM_ITER, config.RAND_ORD, config.N_PERT, config.M, config.K, config.ILS_BATCH

    if not os.path.isfile('./result/C.npy'):
        raise FileExistsError("Can't find codebook in ./result/C.npy")

    with mp.Pool(mp.cpu_count()) as pool:
        lsq = LSQ(ILS_BATCH, M, K, pool, logging=False)
        C = np.load('./result/C.npy')
        B = np.random.randint(0, K, size=[base.shape[0], M]) + np.arange(0, M*K, K)[None, :]

        for i in range(ENCODE_ITER):
            print(f"Encoding # {i}")
            start = time.time()
            B = lsq.encode(base, B, C, 1, ILS_ITER, ICM_ITER, RAND_ORD, N_PERT)
            save(B, M, K)
            end = time.time()
            print('use %4ds' % (start-end))

        print(quantization_error(base, B, C))


if __name__ == "__main__":
    encode()