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
import tensorflow as tf
import random

def pprint(d:dict):
    print(json.dumps(d, indent=4))

class C_update_tf():
    def __init__(self, batch_size, m, k, D):
        self._m = m
        self._k = k
        self._batch_size = batch_size
        self.X_batch = tf.placeholder(tf.float32, [self._batch_size, D])
        self.C_tf = tf.Variable(tf.zeros([m, k, D]))
        self.B_batch = tf.placeholder(tf.int32, [self._batch_size, m])
        self.C_in = tf.placeholder(tf.float32, [m, k, D])
        self.Inference_C(self.X_batch, self.C_tf, self.B_batch)

    def update_C_tf(self, sess, X, C_in, B):
        C_in = C_in.reshape([self._m, self._k, -1])
        N, M = B.shape
        sess.run(self.feed_c, {self.C_in:C_in})
        index = np.random.permutation(N)
        for i in range(N // self._batch_size):
            b = np.copy(B[index[i*self._batch_size:(i+1)*self._batch_size]])
            b -= np.arange(0, self._m*self._k, self._k)
            x = X[index[i*self._batch_size:(i+1)*self._batch_size]]
            _ = sess.run([self.C_Loss, self.C_train], {self.X_batch:x, self.B_batch:b})
        return sess.run(self.C_tf).reshape([self._m*self._k, -1])

    def Inference_C(self, X, C, B):
        self.feed_c = tf.assign(self.C_tf, self.C_in)
        reconstruct = []
        N, M = B.get_shape().as_list()
        for i in range(M):
            codes = C[i]
            reconstruct.append(tf.gather(codes, B[:, i]))
        reconstruct = tf.add_n(reconstruct)
        self.C_Loss = tf.reduce_sum(tf.squared_difference(X, reconstruct), 1)
        self.GlobalStep = tf.Variable(0, trainable=False, name="Step_1")
        lr_C = tf.train.exponential_decay(5e-2, global_step=self.GlobalStep, decay_steps=5000, decay_rate=0.95, staircase=True)
        self.C_train = tf.train.AdamOptimizer(lr_C).minimize(self.C_Loss, self.GlobalStep)

class LSQ():
    def __init__(self, batch_size, m:int, k:int, pool, logging:bool=False):
        self._m = m
        self._k = k
        self._logging = logging
        self._batch_size = batch_size
        self._pool = pool

    def __getstate__(self):
        self_dict = self.__dict__.copy()
        del self_dict['_pool']
        return self_dict
    
    def __setstate__(self, state):
        self.__dict__.update(state)

    def one_hot_encode(self):
        # return a sparse array such that:
        # C @ B picks the index of choosed codeword
        N, M = self.B.shape
        total = self.B.size

        # [0*M, 1*M, 2*M, ..., N*M]
        row = (np.arange(0, N)[:, None] * np.ones([N, M], int)).reshape(-1)
        col = self.B.reshape(-1)
        # return a [N, M*K] matrix with M entry of 1
        return spe.coo_matrix((np.ones(total, bool), (row, col)), shape=(N, M*self._k))


    def lsqr(self, i):
        # for every dim, B @ C[:, i] = X[:, i], so we use B, X[:, i] to solve C[:, i]
        # [N, M*K] @ [M*K] -> [N]
        return lng.lsqr(self.one_hot, self.X[:, i], atol=1e-6, btol=1e-6)[0]

    def update_C(self, lsqr_lsmr:bool):
        N, D = self.X.shape
        _, M = self.B.shape
        self.one_hot = self.one_hot_encode()
        if lsqr_lsmr:
            res = self._pool.map(self.lsqr, range(D))
            return np.array(res).T
        else:
            for i in range(D):
                self.C[:, i] = lng.lsmr(self.one_hot, self.X[:, i])[0]
                return self.C

    def batch_ils(self, i:int, icm_iter:int, randord:bool, npert:int):
        B = self.B[i*self._batch_size:(i+1)*self._batch_size]
        X = self.X[i*self._batch_size:(i+1)*self._batch_size]
        N, M = B.shape
        x_ci = -2 * (X @ self.C.T)

        if npert > 0:
            # according to test, this is way fast and have the same distribution with np.random.choice
            pertidx = np.zeros([N, npert], int)
            r = range(M)
            for i in range(N):
                pertidx[i] = random.sample(r, npert)
            pertvals = np.random.randint(0, self._k, (N, npert)) + pertidx * self._k
            # for i in range(N):
            #     B[i, pertidx[i]] = pertvals[i]
            B[np.indices(pertidx.shape)[0], pertidx] = pertvals

        if randord:
            m_list = np.random.permutation(M)
        else:
            m_list = np.arange(M)
        
        for _ in range(icm_iter):
            for i in m_list:
                p = x_ci[:, i*self._k:(i+1)*self._k]
                q = self.C_unary[:, i*self._k:(i+1)*self._k]
                r = np.sum(self.C_pair[i*self._k:(i+1)*self._k, B], 2).T
                # [N]
                B[:, i] = np.argmin(p + q + r, 1) + (self._k * i)
        return B
        
    # @deprecated("using batch_ils(...)")
    def batch_ils_pool(self, tup):
        i, icm_iter, randord, npert = tup
        B = self.B[i*self._batch_size:(i+1)*self._batch_size]
        X = self.X[i*self._batch_size:(i+1)*self._batch_size]
        N, M = B.shape
        x_ci = -2 * (X @ self.C.T)

        ''' perturb '''
        if npert > 0:
            s = np.random.choice(M, N)
            pertidx = np.array([(s + x) % M for x in np.random.choice(M, npert)]).T
            pertvals = np.random.randint(0, self._k, (N, npert))
            B[np.indices(pertidx.shape)[0], pertidx] = pertvals
        ''' perturb '''

        if randord:
            m_list = np.random.permutation(M)
        else:
            m_list = np.arange(M)
        
        for _ in range(icm_iter):
            for i in m_list:
                B[:, i] = np.argmin(x_ci[:, i*self._k:(i+1)*self._k] + self.C_unary[:, i*self._k:(i+1)*self._k] + np.sum(self.C_pair[i*self._k:(i+1)*self._k, B], 2).T, 1) + (self._k * i)
        return B

    def pre_compute(self):
        M = self._m
        # pre-compute the ||Cibi||^2 for all [M*K, D] codewords
        # [M*K, D] -> [1, M*K]
        self.C_unary= np.sum(np.square(self.C), -1)[None, :]

        # [M*K, M*K]
        self.C_pair = 2 * (self.C @ self.C.T)

        for i in range(M):
            self.C_pair[i*self._k:(i+1)*self._k, i*self._k:(i+1)*self._k] = 0

    def iterative_local_search(self, icm_iter:int, randord:bool, npert:int):
        old_B = np.copy(self.B)

        if self._logging:
            start = time.time()

        if self.X.shape[0] % self._batch_size == 0:
            total_batch = self.X.shape[0] // self._batch_size
        else:
            total_batch = (self.X.shape[0] // self._batch_size) + 1
        res = self._pool.map(self.batch_ils_pool, [(i, icm_iter, randord, npert) for i in range(total_batch)])
        
        self.B = np.concatenate(res, 0)

        assert self.B.shape[0] == old_B.shape[0], "old B and new B number mismatch, where old: {0}, new {1}".format(old_B.shape[0], self.B.shape[0])

        if self._logging:
            end = time.time()
            print("use %.3f s" % (end-start))
        e_old = mse(self.X, old_B, self.C)
        e_new = mse(self.X, self.B, self.C)
        worse = e_new >= e_old
        
        if self._logging:
            print('worse: %d' % np.sum(worse))
        self.B[worse] = old_B[worse]
    
    def train(self, X:np.ndarray, B:np.ndarray, C:np.ndarray, update_C_tf, n_iter:int, ils_iter:int, icm_iter:int, randord:bool, npert:int, use_lsqr:bool):
        """[summary]
        
        Arguments:
            X {np.ndarray} -- a [N, D] array
            R {np.ndarray} -- a [D, D] rotation matrix
            B {np.ndarray} -- init codes with [N, ?], B must be shift encoding. i.e., [0, 78, 190, 201] means [0, 14, 62, 9] in each codebook
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

        if not use_lsqr:
            assert callable(update_C_tf), "param: update_C_tf is not a function"

        config = tf.ConfigProto(allow_soft_placement=True)
        config.gpu_options.allow_growth = True

        print(f"LSQ training on {X.shape} datas")
        
        info = {
            "codebook" : self._m,
            "codewords" : self._k,
            "bitlength" : self._m * int(math.log2(self._k)),
            "# perturb" : npert,
            "# icm" : icm_iter,
            "rand_ord" : randord,
            "use_lsqr": use_lsqr
        }
        pprint(info)

        print('raw: e=%.4e\n' % quantization_error(X, self.B, self.C))
        if not use_lsqr:
            sess = tf.Session(config=config)
            sess.run(tf.global_variables_initializer())
        for i in range(1, n_iter + 1):
            if self._logging:
                print('[%3d]: update C' % i)
                start = time.time()
            if use_lsqr:
                c = self.update_C(True)
                assert c.shape == self.C.shape
                self.C = c
            else:
                self.C = update_C_tf(sess, self.X, self.C, self.B)
            if self._logging:
                error = quantization_error(X, self.B, self.C)
                end = time.time()
                print('[%3d]: update C with e=%.4e, use %.3fs\n' % (i, error, end-start))
                print('[%3d]: pre-compute C' % i)
            self.pre_compute()
            if self._logging:
                print('[%3d]: ILS' % i)
            for j in range(1, ils_iter + 1):
                self.iterative_local_search(icm_iter, randord, npert)
                if self._logging:
                    print("[%3d]: ILS # %2d finished" %(i, j))         
                    
            error = quantization_error(X, self.B, self.C)
            print('[%3d]: e=%.4e\n' % (i, error))

        if not use_lsqr:
            sess.close()

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
        info = {
            "codebook" : self._m,
            "codewords" : self._k,
            "bitlength" : self._m * int(math.log2(self._k)),
            "# perturb" : npert,
            "# icm" : icm_iter,
            "rand_ord" : randord
        }
        pprint(info)
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