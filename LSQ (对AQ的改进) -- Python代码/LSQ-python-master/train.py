import numpy as np
from sift import *
from lsq import LSQ, C_update_tf
from opq import train_opq_np
import multiprocessing as mp
from utils import quantization_error, read_config
import os
import json

def train():
    config = read_config()

    _, train, _, _ = sift1m_read()

    D = train.shape[1]

    np.random.shuffle(train)

    ITER, ILS_ITER, ICM_ITER, RAND_ORD, N_PERT, M, K, ILS_BATCH, SGD_BATCH, LSQR = config.ITER, config.ILS_ITER, config.ICM_ITER, config.RAND_ORD, config.N_PERT, config.M, config.K, config.ILS_BATCH, config.SGD_BATCH, config.LSQR

    try:
        os.mkdir('./result')
    except:
        pass

    with mp.Pool(mp.cpu_count()) as pool:
        lsq = LSQ(ILS_BATCH, M, K, pool, logging=True)
        upctf = C_update_tf(SGD_BATCH, M, K, D)
        C, B, R, _ = train_opq_np(train, pool, M, K, 10)
        
        _, _, d = C.shape
        Cf = np.zeros([M*K, d*M])

        for i in range(M):
            Cf[i*K:(i+1)*K, i*d:(i+1)*d] = C[i]

        B += np.arange(0, M*K, K)[None, :]

        C, _ = lsq.train(train, B, Cf.dot(R.T), upctf.update_C_tf, ITER, ILS_ITER, ICM_ITER, RAND_ORD, N_PERT, LSQR)

        np.save('./result/C.npy', C)


if __name__ == "__main__":
    train()