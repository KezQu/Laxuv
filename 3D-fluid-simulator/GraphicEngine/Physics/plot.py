import matplotlib.pyplot as plt
import numpy as np
import argparse
import os


def ReadData(filename):
    data = []
    with open(filename, "r") as f:
        data = [
            [val for val in line.encode()] for line in f
        ]
    data = np.array(data).reshape(1200, 1200, 16)
    print(data.shape)
    return data


def plot(data, title, xlabel, ylabel):
    resolution = 2400
    plt.imshow(data)
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    # plt.xlim((-resolution/2, resolution/2))
    # plt.ylim((-resolution/2, resolution/2))
    plt.show()


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--filename", type=str, required=True)
    data_filepath = arg_parser.parse_args().filename
    data = ReadData(data_filepath)
    # plot(data, "Velocity vs Time", "Time", "Velocity")
