import numpy as np
import math
import scipy.sparse.linalg as lng
from utils import *
import scipy.sparse as spe
import multiprocessing as mp
import time
import os
from deprecated import deprecated
import json

    def batch_ils(self, i:int, icm_iter:int, randord:bool, npert:int):
        # old = np.copy(self.B[i*self._batch_size:(i+1)*self._batch_size])
        B = self.B[i*self._batch_size:(i+1)*self._batch_size]
        X = self.X[i*self._batch_size:(i+1)*self._batch_size]
        N, M = B.shape
        # pre-compute <x, Cibi> for all [M*K, D]
        # [N, D] @ [D, M*K] -> [N, M*K]
        x_ci = -2 * (X @ self.C.T)

        ''' perturb '''
        if npert > 0:
            s = np.random.choice(M, N)
            pertidx = np.array([(s + x) % M for x in np.random.choice(M, npert)]).T
            pertvals = np.random.randint(0, self._k, (N, npert))

            for i in range(N):
                B[i, pertidx[i]] = pertvals[i]
        ''' perturb '''

        if randord:
            m_list = np.random.permutation(M)
        else:
            m_list = np.arange(M)
        
        for _ in range(icm_iter):
            for i in m_list:
                # with other B fixed, find ||X-CB+CiBi|| that argmin(i)
                # given -2<x, Cij> + ||Cij||^2 + sum(Cij, Ckl) over K entries and the minimum index is the best
                # the mth <x, Cij>, j: 1~k, [N, K]
                p = x_ci[:, i*self._k:(i+1)*self._k]

                ''' assert p correct '''
                # print(np.sum((p - -2 * X @ self.C[i*self._k:(i+1)*self._k].T) ** 2 ))
                # input()


                # the mth ||Cij||^2, [1, K]
                q = self.C_unary[:, i*self._k:(i+1)*self._k]

                ''' assert q correct '''
                # print(np.sum((q - np.sum(self.C[i*self._k:(i+1*self._k)] ** 2, -1)) ** 2))
                # input()

                # find the ith row (Ci: with other Cj:)
                # it has [M, K, K] element, for each [K, K] find the Bj col (Ci: with Cjbj)
                # and sum up -> [N, K]
                # [M, M, K, K] -> [M, K, K] indexed by [N, M] -> [N, M, K] -> [N, K]
                # since the self-matrix [K, K] = 0, it don't affect the computation

                # r = np.zeros((N, self._k))
                # for kk in range(M):
                #     if kk == i:
                #         continue
                #     r += self.C_pair[i, kk, :, B[:, kk]]
                r = np.sum(self.C_pair[i, range(M), :, B], 1)
                
                ''' assert r correct '''
                '''
                ll = np.zeros((N, self._k))
                for kk in range(N):
                    for aa in range(M):
                        if aa == i:
                            continue
                        lllll = self.C[aa*self._k:(aa+1)*self._k][B[kk, aa]]
                        ll[kk] += self.C[i*self._k:(i+1)*self._k] @ lllll
                
                print(np.sum((r - ll) ** 2))
                input()
                '''                
                i_out = p + q + r
                # [N]
                B[:, i] = np.argmin(i_out, 1)

        # check new is good
        # print(mse(X, B, self.C)[0])
        # print(mse(X, old, self.C)[0])
        # input()
        return B