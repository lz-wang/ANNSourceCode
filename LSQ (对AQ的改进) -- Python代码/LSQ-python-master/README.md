# LSQ-python
A numpy implementation of Local Search Quantization.

Please refer to https://github.com/una-dinosauria/local-search-quantization for more information.


# Demo
Please install anaconda (python 3.7) for best performance.

Run `python main.py` for demo in SIFT1M.

**NOTE**: Current implementation doesn't contain the accelerations for retrieval (the extra bits for storing norms).

Default settings (edit in `main.py`):
```python
M = 8
K = 256
bitlength = 32

Iteration = 100
ILS = 8
ICM = 4
Perturbation = 4
RandomOrder = True
EncodeIter = 16
```

We use OPQ-NP run 10 times to initialize codebook and **won't do OTQ initialization**, we observed that the initialization won't take too much effects on performance. Then for encoding, we perform 16x ILS which is equvilent to LSQ-16.

The result comparisons are conducted on SIFT1M, Recall@{1, 10, 100} for 32-bit and 64-bit:

<table class="tg">
  <tr>
    <th class="tg-s6z2"></th>
    <th class="tg-s6z2" colspan="3">32 bits</th>
    <th class="tg-s6z2" colspan="3">64 bits</th>
  </tr>
  <tr>
    <td class="tg-baqh"></td>
    <td class="tg-baqh">R@1</td>
    <td class="tg-baqh">R@10</td>
    <td class="tg-baqh">R@100</td>
    <td class="tg-baqh">R@1</td>
    <td class="tg-baqh">R@10</td>
    <td class="tg-baqh">R@100</td>
  </tr>
  <tr>
    <td class="tg-s6z2">Julia</td>
    <td class="tg-s6z2">11.43</td>
    <td class="tg-baqh">40.48</td>
    <td class="tg-baqh">80.52</td>
    <td class="tg-s6z2">33.23</td>
    <td class="tg-baqh">78.37</td>
    <td class="tg-baqh">98.72</td>
  </tr>
  <tr>
    <td class="tg-s6z2">Python</td>
    <td class="tg-s6z2">11.99</td>
    <td class="tg-baqh">40.54</td>
    <td class="tg-baqh">80.66</td>
    <td class="tg-s6z2">34.34</td>
    <td class="tg-baqh">78.47</td>
    <td class="tg-baqh">98.48</td>
  </tr>
</table>
