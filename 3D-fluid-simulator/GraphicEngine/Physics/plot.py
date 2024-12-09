import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
import matplotlib.cm as cm
import matplotlib.ticker as ticker
import numpy as np
import argparse
import os


def ReadData(filename, granularity):
    data_t = np.dtype(
        [('pos', [('x', np.int32), ('y', np.int32)]), ('val', np.float32)])
    data = np.fromfile(filename, dtype=data_t)
    if (data.shape[0] != 0):
        values_data = np.array([val
                                for _, val in data])
        pos_data = np.array([[pos['x'], pos['y']]
                            for pos, _ in data])
        colormap = cm.ScalarMappable(norm=Normalize(
            vmax=np.max(values_data), vmin=np.min(values_data)), cmap=cm.jet)

        max_pos = (int(max(pos_data[:, 0])), int(max(pos_data[:, 1])))
        min_pos = (int(min(pos_data[:, 0])), int(min(pos_data[:, 1])))
        values_data = np.reshape(values_data, (3, values_data.shape[0]//3))
        pos_data = np.reshape(
            pos_data, (3, pos_data.shape[0]//3, pos_data.shape[1]))

        offset_size = int(1 // granularity)

        mesh = np.zeros(
            [pos_data.shape[0], (max_pos[0] - min_pos[0] + 2 * offset_size), (max_pos[1] - min_pos[1] + 2 * offset_size), 4])

        color_data = np.array(
            [colormap.to_rgba(values_data[i].tolist(), alpha=1)
             for i in range(values_data.shape[0])])
        pos_data = np.flip(pos_data, axis=1)
        color_data = np.flip(color_data, axis=1)
        print(pos_data.shape)
        print(color_data.shape)

        for i in range(color_data.shape[0]):
            for j in range(color_data.shape[1]):
                for offset_x in range(0, 2 * offset_size):
                    for offset_y in range(0, 2 * offset_size):
                        mesh[i][pos_data[i][j][0] - min_pos[0] + offset_x][pos_data[i]
                                                                           [j][1] - min_pos[1] + offset_y] = color_data[i][j]

        return mesh, values_data, (np.min(values_data[1]), np.max(values_data[1])), (min_pos, max_pos)
    return None, None, None, None


def plot(data, values, min_max, bounds, granularity):
    if data is not None:
        fig, ax = plt.subplots(2, 2, figsize=(16, 9))
        axis_ticks = [np.linspace(0, data.shape[1], 5, dtype=int),
                      np.linspace(bounds[0][0] * granularity,
                                  bounds[1][0] * granularity, 5, dtype=float),
                      np.linspace(bounds[0][1] * granularity, bounds[1][1] * granularity, 5, dtype=float)]
        axis_ticks[1] = np.round(axis_ticks[1], 1)
        axis_ticks[2] = np.round(axis_ticks[2], 1)

        imgXY = ax[0, 0].imshow(data[0], origin='lower',
                                cmap='jet', vmin=min_max[0], vmax=min_max[1])
        ax[0, 0].set_title("Projection XY")
        plt.setp(ax[0, 0], xticks=axis_ticks[0], yticks=axis_ticks[0],
                 xticklabels=axis_ticks[1], yticklabels=axis_ticks[2], xlabel="axis X", ylabel="axis Y")
        fig.colorbar(imgXY, ax=ax[0, 0])

        imgYZ = ax[1, 0].imshow(
            data[1], origin='lower', cmap='jet', vmin=min_max[0], vmax=min_max[1])
        ax[1, 0].set_title("Projection YZ")
        plt.setp(ax[1, 0], xticks=axis_ticks[0], yticks=axis_ticks[0],
                 xticklabels=axis_ticks[1], yticklabels=axis_ticks[2], xlabel="axis Z", ylabel="axis Y")
        fig.colorbar(imgYZ, ax=ax[1, 0])

        imgXZ = ax[0, 1].imshow(
            data[2], origin='lower', cmap='jet', vmin=min_max[0], vmax=min_max[1])
        ax[0, 1].set_title("Projection XZ")
        plt.setp(ax[0, 1], xticks=axis_ticks[0], yticks=axis_ticks[0],
                 xticklabels=axis_ticks[1], yticklabels=axis_ticks[2], xlabel="axis X", ylabel="axis Z")
        fig.colorbar(imgXZ, ax=ax[0, 1])

        imgHist = ax[1, 1].hist(values[0], bins=20, density=True)
        print(values[0].shape)
        ax[1, 1].set_title("Values distribution")
        # ax[1, 1].yaxis.set_major_formatter(ticker.PercentFormatter(xmax=0.01))

        plt.show()


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--filename", type=str, required=True)
    arg_parser.add_argument("--granularity", type=float, required=True)
    data_filepath = arg_parser.parse_args().filename
    granularity = arg_parser.parse_args().granularity
    data, values, min_max, bounds = ReadData(data_filepath, granularity)
    plot(data, values, min_max, bounds, granularity)
