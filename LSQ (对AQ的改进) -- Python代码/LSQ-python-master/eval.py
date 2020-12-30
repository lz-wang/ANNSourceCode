import numpy as np
from sift import *
from utils import quantization_error, read_config, reconstruct
import os
import json
import tensorflow as tf
from tqdm import trange

def eval():
    config = read_config()

    C, B = restore(config.M, config.K)

    base, _, query, gt = sift1m_read()

    D = base.shape[1]

    del config

    config = tf.ConfigProto(allow_soft_placement=True)
    config.gpu_options.allow_growth = True

    quantized = reconstruct(C, B)
    
    with tf.Session(config=config) as sess:
        total_distortion = np.mean(np.sum(np.square(quantized - base), axis=1))
        print('total distortion:', total_distortion)

        recall = np.zeros([query.shape[0], 100], dtype=np.float32)

        database = tf.constant(quantized, dtype=tf.float32)
        query_gpu = tf.placeholder(tf.float32, shape=(1, D))
        print(database.get_shape().as_list())
        distance = tf.norm(query_gpu - database, ord='euclidean', axis=1)
        
        print(distance.get_shape().as_list())
        _, index = tf.nn.top_k(-distance, k=100, sorted=True)
        print(index.get_shape().as_list())
        sess.run(tf.global_variables_initializer())

        for i in trange(query.shape[0], ncols=50):
            q = query[i][None, :]
            top100 = sess.run(index, {query_gpu:q})
            gt1 = gt[i][0]
            recall[i] = np.cumsum(np.isin(top100, gt1))
    recall = np.mean(recall, 0)

    np.savetxt('./result/recall.txt', recall)


def restore(M, K):
    if not os.path.isfile('./result/C.npy'):
        raise FileExistsError("Can't find codebook in ./result/C.npy")
    C = np.load('./result/C.npy')
    if not os.path.isfile('./result/B.npy'):
        raise FileExistsError("Can't find codebook in ./result/C.npy")
    B = np.load('./result/B.npy')
    B = B.astype(np.int)
    B += np.arange(0, M*K, K)[None, :]
    return C, B


if __name__ == "__main__":
    eval()
