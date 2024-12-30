import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.cm as cm
import matplotlib.ticker as ticker
import numpy as np
import argparse
import os
from concurrent.futures import ProcessPoolExecutor
from functools import partial


# Helper function used to calculate a graph data for a specific plane (e.g. XY, YZ, XZ)
def CreateMesh(i, pos_data, color_data, min_pos, max_pos, offset_size):
    # Initialize an empty mesh with a given size
    mesh = np.zeros(
        [(max_pos[i] - min_pos[i] + 2 * offset_size), (max_pos[(i + 1) % 3] - min_pos[(i + 1) % 3] + 2 * offset_size), 4])
    # Specify an axis to be squashed based on the invocation index
    flatten_axis = (i+2) % 3
    # Sort positions and color values based on a position in a flattened axis to draw for furthest to nearest
    sort_indices = np.argsort(pos_data[:, flatten_axis])
    sorted_pos = pos_data[sort_indices]
    sorted_color = color_data[sort_indices]
    # Insert colors at a given position into an empty mesh
    for j in range(color_data.shape[0]):
        for offset_x in range(0, 2 * offset_size):
            for offset_y in range(0, 2 * offset_size):
                x_pos = sorted_pos[j][(
                    flatten_axis - 2) % 3] - min_pos[(flatten_axis - 2) % 3]
                y_pos = sorted_pos[j][(
                    flatten_axis - 1) % 3] - min_pos[(flatten_axis - 1) % 3]
                mesh[x_pos + offset_x][y_pos +
                                       offset_y] = sorted_color[j]
    return mesh


# Function responsible for reading binary data and retrieving position and value
def ReadData(filename, granularity):
    data_t = np.dtype(
        [('pos', [('x', np.int32), ('y', np.int32), ('z', np.int32)]), ('val', np.float32)])
    # Read binary data in a given format
    data = np.fromfile(filename, dtype=data_t)
    if (data.shape[0] != 0):
        # Split positions out of color values
        values_data = np.array([val
                                for _, val in data])
        pos_data = np.array([[pos['x'], pos['y'], pos['z']]
                            for pos, _ in data])
        # Create a color map based on a provided values
        colormap = cm.ScalarMappable(norm=Normalize(
            vmax=np.max(values_data), vmin=np.min(values_data)), cmap=cm.jet)
        # Create RGBA values out of raw values in floats
        color_data = np.array(colormap.to_rgba(values_data.tolist(), alpha=1))

        # Find max and min positions in every axis
        max_pos = (int(max(pos_data[:, 0])), int(
            max(pos_data[:, 1])), int(max(pos_data[:, 2])))
        min_pos = (int(min(pos_data[:, 0])), int(
            min(pos_data[:, 1])), int(min(pos_data[:, 2])))
        offset_size = int(1 // granularity)

        # Save arguments at a given positions
        CreateMeshPartial = partial(
            CreateMesh, pos_data=pos_data.copy(), color_data=color_data.copy(), min_pos=min_pos, max_pos=max_pos, offset_size=offset_size)
        # Create processes to fasten the calculation time for a specified planes
        with ProcessPoolExecutor() as executor:
            mesh = list(executor.map(CreateMeshPartial, range(3)))

        for i in range(3):
            mesh[i] = mesh[i].astype(np.float32)
        # Transform XY and XZ planes to preserve correct orientation
        mesh[0] = np.transpose(mesh[0], (1, 0, 2))
        mesh[2] = np.flip(mesh[2], axis=0)
        return mesh, values_data, (np.min(values_data), np.max(values_data)), (min_pos, max_pos)
    return None, None, None, None


# Function responsible for drawing provided data
def plot(data, values, unit, min_max, bounds):
    if data is not None:
        fig, ((ax_xy, ax_yz), (ax_xz, ax_dist)
              ) = plt.subplots(2, 2, figsize=(16, 9))

        xy_bounds = (bounds[0][0] * granularity, bounds[1][0] * granularity,
                     bounds[0][1] * granularity, bounds[1][1] * granularity)
        # Draw particles visible in XY plane
        imgXY = ax_xy.imshow(data[0], origin='lower',
                             cmap='jet', vmin=min_max[0], vmax=min_max[1], extent=xy_bounds, interpolation='none')
        ax_xy.set_title("Projection XY")
        plt.setp(ax_xy, xlabel="axis X [mm]", ylabel="axis Y [mm]")
        cbar_xy = fig.colorbar(imgXY, ax=ax_xy)
        cbar_xy.set_label("Value range in " + unit)

        yz_bounds = (bounds[0][2] * granularity, bounds[1][2] * granularity,
                     bounds[0][1] * granularity, bounds[1][1] * granularity)
        # Draw particles visible in YZ plane
        imgYZ = ax_yz.imshow(
            data[1], origin='lower', cmap='jet', vmin=min_max[0], vmax=min_max[1], extent=yz_bounds, interpolation='none')
        ax_yz.set_title("Projection YZ")
        plt.setp(ax_yz, xlabel="axis Z [mm]", ylabel="axis Y [mm]")
        cbar_yz = fig.colorbar(imgYZ, ax=ax_yz)
        cbar_yz.set_label("Value range in " + unit)

        xz_bounds = (bounds[0][0] * granularity, bounds[1][0] * granularity,
                     bounds[0][2] * granularity, bounds[1][2] * granularity)
        # Draw particles visible in XZ plane
        imgXZ = ax_xz.imshow(
            data[2], origin='lower', cmap='jet', vmin=min_max[0], vmax=min_max[1], extent=xz_bounds, interpolation='none')
        ax_xz.set_title("Projection XZ")
        plt.setp(ax_xz, xlabel="axis X [mm]", ylabel="axis Z [mm]")
        cbar_xz = fig.colorbar(imgXZ, ax=ax_xz)
        cbar_xz.set_label("Value range in " + unit)

        # Draw a distribution of a requested values
        imgHist = ax_dist.hist(values, bins=20)
        ax_dist.set_title("Values distribution")
        ax_dist.set_xlabel("Value range in " + unit)

        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--filename", type=str, required=True)
    arg_parser.add_argument("--granularity", type=float, required=True)
    arg_parser.add_argument("--unit", type=str, required=True)
    data_filepath = arg_parser.parse_args().filename
    granularity = arg_parser.parse_args().granularity
    unit = arg_parser.parse_args().unit
    data, values, min_max, bounds = ReadData(data_filepath, granularity)
    plot(data, values, unit, min_max, bounds)
