import numpy as np
import json

class config():
    def __init__(self, **entries):
        self.__dict__.update(entries)

def quantization_error(X, B, C):
    q_x = reconstruct(C, B)
    return np.mean(np.sum(np.square(X - q_x), 1))

def mse(X, B, C):
    q_x = reconstruct(C, B)
    return np.sum(np.square(X - q_x), 1)

def l2_difference(A, B):
    return np.sum(np.square(np.abs(A[:, None, :] - B)), 2)

def sqdist(a, b):
    # [N, k]
    d = -2 * b.dot(a.T) + np.sum(np.square(b), axis=1, keepdims=True)
    # [k, N]
    d = np.abs(d.T + np.sum(np.square(a), axis=1, keepdims=True))
    # [k, N]
    return d

def reconstruct(C, B):
    return np.sum(C[B], 1)

def read_config():
    with open("./config.json") as fp:
        return config(**json.load(fp))
