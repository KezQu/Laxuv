import matplotlib.pyplot as plt
import numpy as np
import argparse
import os


def ReadData(filename, resolution):
    data = np.fromfile(filename, dtype=np.uint8)
    data = np.array(data).reshape(3, resolution, resolution, 4)
    return data


def plot(data, resolution):
    fig, ax = plt.subplots(2, 2)

    imgXY = ax[0, 0].imshow(data[0], origin='lower', cmap='jet', vmin=0, vmax=2000)
    ax[0, 0].set_title("Projection XY")
    fig.colorbar(imgXY, ax=ax[0, 0])
    imgYZ = ax[1, 0].imshow(data[1], origin='lower', cmap='jet')
    ax[1, 0].set_title("Projection YZ")
    fig.colorbar(imgYZ, ax=ax[1, 0])
    imgXZ = ax[0, 1].imshow(data[2], origin='lower', cmap='jet')
    ax[0, 1].set_title("Projection XZ")
    fig.colorbar(imgXZ, ax=ax[0, 1])

    axis_ticks = (np.linspace(0, resolution, 10, dtype=int),
                  np.linspace(-resolution/20, resolution/20, 10, dtype=int))
    plt.setp(ax, xticks=axis_ticks[0], yticks=axis_ticks[0],
             xticklabels=axis_ticks[1], yticklabels=axis_ticks[1])
    plt.show()


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--filename", type=str, required=True)
    arg_parser.add_argument("--resolution", type=str, required=True)
    data_filepath = arg_parser.parse_args().filename
    resolution = int(arg_parser.parse_args().resolution)
    data = ReadData(data_filepath, resolution)
    plot(data, resolution)
