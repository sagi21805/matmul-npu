import matnpu
import numpy as np

# Generate some random data
a = np.random.randint(low=-128, high=127, size=(320, 320), dtype=np.int8)
b = np.random.randint(low=-128, high=127, size=(320, 320), dtype=np.int8)

# Your original computation
np_matmul = np.matmul(a.astype(np.int32), b.astype(np.int32))

# npu calculation
npu_matmul = matnpu.matmul_i32(a, b)

print(npu_matmul)

print(np_matmul)

print("same result:", np.array_equal(np_matmul, npu_matmul))


