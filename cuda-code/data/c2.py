import nrrd
import h5py
import numpy as np

# Load the NRRD file
data, header = nrrd.read("jet4_0.486_v.nrrd")  # (3, Y, X, Z)
print(data.shape)
vx, vy, vz = data[0], data[1], data[2]  # (Y, X, Z)

# Sizes
ny, nx, nz = vx.shape

# Spacing (handle NaN fallback)
spacing = header.get("spacings", [np.nan, 0.007812, 0.007812, 0.007812])
sx, sy, sz = spacing[1], spacing[2], spacing[3]
dx, dy, dz = sx, sy, sz

# Offsets (origin), default to 0
x0, y0, z0 = 0.0, 0.0, 0.0

# Create a 4-channel field: [vx, vy, vz, valid]
field = np.zeros((ny, nx, nz, 3), dtype=np.float32)
field[:, :, :, 0] = vx
field[:, :, :, 1] = vy
field[:, :, :, 2] = vz
# field[:, :, :, 3] = 1.0  # mark as valid everywhere


# Save to HDF5
with h5py.File("jet_v4.h5", "w") as f:
    f.create_dataset("field", data=field, compression="gzip")
    f.create_dataset("offset", data=np.array([x0, y0, z0], dtype=np.float32))
    f.create_dataset("scale", data=np.array([1 / dx, 1 / dy, 1 / dz], dtype=np.float32))

    f.attrs["description"] = "CUDA-friendly 3D float4 vector field"
    f.attrs["source"] = "Converted from jet4_0.486_v.nrrd"
