import pycuda.driver as cuda
import pycuda.autoinit  # Automatically initializes CUDA

def check_p2p_access():
    # Get the number of GPUs
    n_gpus = cuda.Device.count()

    print(f"Total GPUs: {n_gpus}")
    # Check P2P access between each pair of GPUs
    for i in range(n_gpus):
        for j in range(n_gpus):
            if i != j:
                dev_i = cuda.Device(i)
                dev_j = cuda.Device(j)
                can_access = dev_i.can_access_peer(dev_j)
                print(f"GPU {i} can access GPU {j}: {'Yes' if can_access else 'No'}")

if __name__ == "__main__":
    check_p2p_access()
