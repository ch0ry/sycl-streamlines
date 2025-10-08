import nrrd
import h5py

# data, header = nrrd.read("jet4_0.486_v.nrrd")
# file = h5py.File("jet_v2.h5", "r")
#
#
# def summarize_nrrd(filename):
#     data, header = nrrd.read(filename)
#
#     print(f"📁 File: {filename}")
#     print("🧮 Data shape:", data.shape)
#     print("📐 Data type:", data.dtype)
#     print("📝 Metadata (header):")
#     for key, value in header.items():
#         print(f"  {key}: {value}")
#
#
# # Example usage
# summarize_nrrd("jet4_0.486_v.nrrd")

# Transpose from (3, x, y, z) → (x, y, z, 3)
# data = data.transpose(1, 2, 3, 0)  # (128, 256, 128, 3)
#

# print(file.keys())
#
# print("File attributes:", list(file.attrs))
# # print("Dataset attributes:", list(file["/velocity"].attrs))
#
#
# def print_tree(name, obj):
#     print(name, "->", obj)
#

# for key, val in file["/volume"].attrs.items():
#     print(f"{key} = {val}")

# file.visititems(print_tree)
# with h5py.File("converted.h5", "w") as f:
#     dset = f.create_dataset("volume", data=data)
#
#     # Copiar todos los atributos del header como atributos del dataset
#     for key, value in header.items():
#         try:
#             # Algunos valores pueden no ser serializables directamente
#             dset.attrs[key] = value
#         except TypeError:
#             print(f"Advertencia: atributo '{key}' no se pudo guardar")
#
# # También puedes establecer solo los importantes
# if "spacings" in header and len(header["spacings"]) == 4:
#     dset.attrs["spacing"] = np.array(header["spacings"][1:], dtype=np.float32)  #

with h5py.File("jet_v3.h5", "r") as f:
    print("File attributes:", list(f.attrs))
    print("Dataset attributes:", list(f["/field"].attrs))
