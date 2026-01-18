import nrrd
import h5py
import numpy as np

# Load the NRRD file
data, header = nrrd.read("jet4_0.486_v.nrrd")  # (3, Y, X, Z)
print(data.shape)
vx, vy, vz = data[0], data[1], data[2]  # (Y, X, Z)

Xmax = np.array([vx[1].max(), vy[1].max(),vz[1].max()]).max()
Xmin = np.array([vx[1].min(), vy[1].min(),vz[1].min()]).min()
Ymax = np.array([vx[2].max(), vy[2].max(),vz[2].max()]).max()
Ymin = np.array([vx[2].min(), vy[2].min(),vz[2].min()]).min()
Zmax = np.array([vx[3].max(), vy[3].max(),vz[3].max()]).max()
Zmin = np.array([vx[3].min(), vy[3].min(),vz[3].min()]).min()

print("Xmax, Xmin = (" + str(Xmax) + ", " + str(Xmin) + ")")
print("Ymax, Ymin = (" + str(Ymax) + ", " + str(Ymin) + ")")
print("Zmax, Zmin = (" + str(Zmax) + ", " + str(Zmin) + ")")
print(f'Array shape: {vx.shape}')
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
# with h5py.File("jet_v4.h5", "w") as f:
#     f.create_dataset("field", data=field, compression="gzip")
#     f.create_dataset("offset", data=np.array([x0, y0, z0], dtype=np.float32))
#     f.create_dataset("scale", data=np.array([1 / dx, 1 / dy, 1 / dz], dtype=np.float32))
#
#     f.attrs["description"] = "CUDA-friendly 3D float4 vector field"
#     f.attrs["source"] = "Converted from jet4_0.486_v.nrrd"

# Read here to verify
with h5py.File("jet_v4.h5", "r") as f:
    field_read = f["field"][:]
    offset_read = f["offset"][:]
    scale_read = f["scale"][:]

print("Field shape in HDF5:", field_read.shape)
print("Offset in HDF5:", offset_read)
print("Scale in HDF5:", scale_read)

# Print some sample values for verification
print("Sample field values at (0,0,0):", field_read[0, 0, 0, :])
print("Sample field values at (ny//2, nx//2, nz//2):", field_read[ny//2, nx//2, nz//2, :])
print("Sample field values at (ny-1, nx-1, nz-1):", field_read[ny-1, nx-1, nz-1, :])