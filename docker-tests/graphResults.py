import matplotlib.pyplot as plt
import pandas as pd

hip = pd.read_csv("hip/results/results.csv")
sycl = pd.read_csv("sycl/results/results.csv")
cuda = pd.read_csv("cuda/results/results.csv")


# for row in pd.read_csv("resultsHIP.csv"):

hip_rk4 = hip[hip["test"] == "rk4"]

sycl_rk4 = sycl[sycl["test"] == "rk4"]

cuda_rk4 = cuda[cuda["test"] == "rk4"]

print(sycl_rk4.columns)

plt.plot(sycl_rk4[" amount"], sycl_rk4[" time_elapsed"], label="SYCL")
plt.plot(hip_rk4[" amount"], hip_rk4[" time_elapsed"], label="HIP")
plt.plot(cuda_rk4[" amount"], cuda_rk4[" time_elapsed"], label="CUDA")

plt.xlabel("nsteps*nseeds")
plt.ylabel("time elapsed (ms)")
plt.title("HIP vs SYCL (RK4 only)")
plt.legend()
plt.grid(True)

plt.savefig(
    "syclvhip.png", dpi=300, bbox_inches="tight"
)  # Saves as a high-resolution PNG file

plt.show()
