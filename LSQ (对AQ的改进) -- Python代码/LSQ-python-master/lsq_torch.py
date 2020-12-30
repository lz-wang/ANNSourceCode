import numpy as np
import torch
from torch import nn
import math
from utils import *
import multiprocessing as mp
import time
import os
import json

def pprint(d:dict):
    print(json.dumps(d, indent=4))


class ICM(nn.Module):
    def __init__(self, M, K, icm_iter, randord, npert, batch):
        self.icm_iter = icm_iter
        self.randord = randord
        self.npert = npert
        self._m = M
        self._k = K
        self._n = batch
    
    def forward(self, X:torch.Tensor, B:torch.Tensor, C_unary, C_pair):
        x_ci = -2 * (X @ self.C.T)

        ''' perturb '''
        if self.npert > 0:
            pertidx = torch.multinomial(torch.ones((self._n, self._m)), num_samples=self.npert, replacement=False)
            pertvals = torch.randint(0, self._k, (self._n, self.npert))
            ix = torch.arange(0, self._n).unsqueeze(1).repeat(1, self.npert)
            B[ix, pertidx] = pertvals
        ''' perturb '''

        if self.randord:
            m_list = np.random.permutation(self._m)
        else:
            m_list = np.arange(self._m)
        
        for _ in range(self.icm_iter):
            for i in m_list:
                B[:, i] = torch.argmin(x_ci[:, i*self._k:(i+1)*self._k] + C_unary[:, i*self._k:(i+1)*self._k] + torch.sum(C_pair[i, range(self._m), :, B], 1), 1)
        return B

class PreCompute(nn.Module):
    def __init__(self, m, k):
        self._m = m
        self._k = k
    
    def forward(self, C):
        C_unary = torch.sum(C ** 2, -1).unsqueeze(0)
        C_pair = 2 * (C @ C.t())
        for i in range(self._m):
            C_pair[i*self._k:(i+1)*self._k, i*self._k:(i+1)*self._k] = 0
        return C_unary, C_pair

class UpdateC(nn.Module):
    def __init__(self):
        pass

    def forward(self, X, C, B):
        return torch.sum((X - torch.sum(C[B], 1)) ** 2, 1)

class LSQ():
    def __init__(self, m:int, k:int, icm_iter, randord, npert, batch_size, logging:bool=False):
        self._m = m
        self._k = k
        self._logging = logging
        self._batch_size = batch_size
        self._UpdateC = UpdateC()
        self._PreCompute = PreCompute(m, k)
        self._ICM = ICM(m, k, icm_iter, randord, npert, batch_size)

    def update_C(self,):
        pass

    def pre_compute(self):
        M = self._m
        # pre-compute the ||Cibi||^2 for all [M*K, D] codewords
        # [M*K, D] -> [1, M*K]
        self.C_unary= np.sum(np.square(self.C), -1)[None, :]

        # pre-compute pairwise term of <Cibi, Cjbj>
        # [M, M, K, K]
        # C_pair[0, 2] is the C0 with C2 (K code with K code)
        self.C_pair = np.zeros([M, M, self._k, self._k])
        for i in range(M):
            for j in range(M):
                if i == j:
                    continue
                # WHY TIMES 2 ???????????????? *FUCK*
                self.C_pair[i, j] = 2 * (self.C[i*self._k:(i+1)*self._k] @ (self.C[j*self._k:(j+1)*self._k].T))

    def iterative_local_search(self, icm_iter:int, randord:bool, npert:int):
        old_B = np.copy(self.B)

        if self._logging:
            start = time.time()
        for i in range(self.X.shape[0] // self._batch_size):
            self.B[i*self._batch_size:(i+1)*self._batch_size] = self.batch_ils(i, icm_iter, randord, npert)

        if self._logging:
            end = time.time()
            print(end-start)
        e_old = mse(self.X, old_B, self.C)
        e_new = mse(self.X, self.B, self.C)
        worse = e_new >= e_old
        
        if self._logging:
            print('worse: %d' % np.sum(worse))
        self.B[worse] = old_B[worse]
    
    def train(self, X:np.ndarray, B:np.ndarray, C:np.ndarray, pool, n_iter:int, ils_iter:int, icm_iter:int, randord:bool, npert:int):
        """[summary]
        
        Arguments:
            X {np.ndarray} -- a [N, D] array
            R {np.ndarray} -- a [D, D] rotation matrix
            B {np.ndarray} -- init codes with [N, ?]
            C {np.ndarray} -- init codebook with [M*K, D]
            n_iter {int} -- optimization iters
            ils_iter {int} -- ILS iterations during encoding
            icm_iter {int} -- ICM iterations
            randord {bool} -- whether to use random order
            npert {int} -- how many codes to perturb
        
        Keyword Arguments:
            logging {bool} -- whether to loggind (default: {False})
        """
        self.X = X
        self.B = B
        self.C = C

        print(f"LSQ training on {X.shape} datas")
        
        info = {
            "codebook" : self._m,
            "codewords" : self._k,
            "bitlength" : self._m * int(math.log2(self._k)),
            "# perturb" : npert,
            "# icm" : icm_iter,
            "rand_ord" : randord
        }
        pprint(info)

        print('raw: e %4e\n' % quantization_error(X, self.B, self.C))

        for i in range(1, n_iter + 1):
            if self._logging:
                print('[%3d]: update C' % i)
            self.update_C_pool(True, pool)
            if self._logging:
                print('[%3d]: pre-compute C' % i)
            self.pre_compute()
            if self._logging:
                print('[%3d]: ILS' % i)
            for j in range(1, ils_iter + 1):
                self.iterative_local_search(icm_iter, randord, npert)
                if self._logging:
                    print("[%3d]: ILS # %2d finished" %(i, j))

            error = quantization_error(X, self.B, self.C)
            print('[%3d]: e %4e\n' % (i, error))
        '''
        Quantized = reconstruct(C, B)

        dbnorms = np.sum(Quantized ** 2, 1)

        dbnormsq = k_means(dbnorms, self._k)
        cbnorms = dbnormsq.centers

        B_norms = dbnormsq.assignments.reshape([N])
        '''
        return self.C, self.B

        
    def encode(self, X:np.ndarray, B:np.ndarray, C:np.ndarray, n_iter:int, ils_iter:int, icm_iter:int, randord:bool, npert:int):
        """[summary]
        
        Arguments:
            X {np.ndarray} -- a [N, D] array
            R {np.ndarray} -- a [D, D] rotation matrix
            B {np.ndarray} -- init codes with [N, ?]
            C {np.ndarray} -- init codebook with [M*K, D]
            n_iter {int} -- optimization iters
            ils_iter {int} -- ILS iterations during encoding
            icm_iter {int} -- ICM iterations
            randord {bool} -- whether to use random order
            npert {int} -- how many codes to perturb
        """
        print(f"Encoding on {X.shape} datas")
        self.X = X
        self.B = B
        self.C = C
        for i in range(1, n_iter + 1):
            if self._logging:
                print('[%3d]: pre-compute C' % i)
            self.pre_compute()
            if self._logging:
                print('[%3d]: ILS' % i)
            for j in range(1, ils_iter + 1):
                self.iterative_local_search(icm_iter, randord, npert)
                if self._logging:
                    print("[%3d]: ILS # %2d finished" %(i, j))
        '''
        Quantized = reconstruct(C, B)

        dbnorms = np.sum(Quantized ** 2, 1)

        dbnormsq = k_means(dbnorms, self._k)
        cbnorms = dbnormsq.centers

        B_norms = dbnormsq.assignments.reshape([N])
        '''
        return self.B