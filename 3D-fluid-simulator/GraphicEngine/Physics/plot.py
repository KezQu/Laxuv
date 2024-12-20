import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.cm as cm
import matplotlib.ticker as ticker
import numpy as np
import argparse
import os
from concurrent.futures import ProcessPoolExecutor
from functools import partial


def CreateMesh(i, pos_data, color_data, min_pos, max_pos, offset_size):
    mesh = np.zeros(
        [(max_pos[i] - min_pos[i] + 2 * offset_size + 1), (max_pos[(i + 1) % 3] - min_pos[(i + 1) % 3] + 2 * offset_size + 1), 4])
    flatten_axis = (i+2) % 3
    sort_indices = np.argsort(pos_data[i][:, flatten_axis])
    for j in range(color_data.shape[1]):
        for offset_x in range(0, 2 * offset_size):
            for offset_y in range(0, 2 * offset_size):
                x_pos = pos_data[i][sort_indices][j][(
                    flatten_axis - 2) % 3] - min_pos[(flatten_axis - 2) % 3]
                y_pos = pos_data[i][sort_indices][j][(
                    flatten_axis - 1) % 3] - min_pos[(flatten_axis - 1) % 3]
                mesh[x_pos + offset_x][y_pos +
                                       offset_y] = color_data[i][sort_indices][j]
    return mesh


def ReadData(filename, granularity):
    data_t = np.dtype(
        [('pos', [('x', np.int32), ('y', np.int32), ('z', np.int32)]), ('val', np.float32)])
    data = np.fromfile(filename, dtype=data_t)
    if (data.shape[0] != 0):
        values_data = np.array([val
                                for _, val in data])
        pos_data = np.array([[pos['x'], pos['y'], pos['z']]
                            for pos, _ in data])
        colormap = cm.ScalarMappable(norm=Normalize(
            vmax=np.max(values_data), vmin=np.min(values_data)), cmap=cm.jet)

        max_pos = (int(max(pos_data[:, 0])), int(
            max(pos_data[:, 1])), int(max(pos_data[:, 2])))
        min_pos = (int(min(pos_data[:, 0])), int(
            min(pos_data[:, 1])), int(min(pos_data[:, 2])))

        values_data = np.reshape(values_data, (3, values_data.shape[0]//3))
        pos_data = np.reshape(
            pos_data, (3, pos_data.shape[0]//3, pos_data.shape[1]))

        offset_size = int(1 // granularity)

        color_data = np.array(
            [colormap.to_rgba(values_data[i].tolist(), alpha=1)
             for i in range(values_data.shape[0])])

        CreateMeshPartial = partial(
            CreateMesh, pos_data=pos_data, color_data=color_data, min_pos=min_pos, max_pos=max_pos, offset_size=offset_size)
        with ProcessPoolExecutor() as executor:
            mesh = list(executor.map(CreateMeshPartial, range(3)))

        for i in range(3):
            mesh[i] = mesh[i].astype(np.float32)
        mesh[0] = np.transpose(mesh[0], (1, 0, 2))
        mesh[2] = np.flip(mesh[2], axis=0)
        return mesh, values_data, (np.min(values_data[1]), np.max(values_data[1])), (min_pos, max_pos)
    return None, None, None, None


def plot(data, values, min_max, bounds, granularity):
    if data is not None:
        fig, ax = plt.subplots(2, 2, figsize=(16, 9))

        imgXY = ax[0, 0].imshow(data[0], origin='lower',
                                cmap='jet', vmin=min_max[0], vmax=min_max[1])
        ax[0, 0].set_title("Projection XY")
        plt.setp(ax[0, 0], xticks=[],
                 yticks=[],  xlabel="axis X", ylabel="axis Y")
        fig.colorbar(imgXY, ax=ax[0, 0])

        imgYZ = ax[1, 0].imshow(
            data[1], origin='lower', cmap='jet', vmin=min_max[0], vmax=min_max[1])
        ax[1, 0].set_title("Projection YZ")
        plt.setp(ax[1, 0], xticks=[],
                 yticks=[], xlabel="axis Z", ylabel="axis Y")
        fig.colorbar(imgYZ, ax=ax[1, 0])

        imgXZ = ax[0, 1].imshow(
            data[2], origin='lower', cmap='jet', vmin=min_max[0], vmax=min_max[1])
        ax[0, 1].set_title("Projection XZ")
        plt.setp(ax[0, 1], xticks=[],
                 yticks=[], xlabel="axis X", ylabel="axis Z")
        fig.colorbar(imgXZ, ax=ax[0, 1])

        imgHist = ax[1, 1].hist(values[0], bins=20)
        ax[1, 1].set_title("Values distribution")

        plt.show()


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--filename", type=str, required=True)
    arg_parser.add_argument("--granularity", type=float, required=True)
    data_filepath = arg_parser.parse_args().filename
    granularity = arg_parser.parse_args().granularity
    data, values, min_max, bounds = ReadData(data_filepath, granularity)
    plot(data, values, min_max, bounds, granularity)
