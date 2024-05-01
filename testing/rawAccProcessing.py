## @file rawAccProcessing.py
#  @author Jack Duignan (Jdu80@uclive.ac.nz)
#  @date 2024-04-25
#  @brief A series of functions for plotting the raw output of the 
#         accelerometer sampled at 50 Hz

import numpy as np
import matplotlib.pyplot as plt

POLL_RATE = 50 # Hz
SCALE_FACTOR = 0.004*9.81

stationary_file = "./testing/data/rawAccelStationary.txt"
x_roll_file = "./testing/data/rawAccelxroll.txt"
y_roll_file = "./testing/data/rawAccelyroll.txt"

def plot_stationary():
    """ Plot the stationary acceleration """
    stationary_data = np.loadtxt(stationary_file, skiprows=2, delimiter=",")
    stationary_data[:,0:-1] *= SCALE_FACTOR

    fig, (ax1, ax2, ax3) = plt.subplots(3, sharex=True)

    ax1.plot(np.arange(0, len(stationary_data[:, 0])), stationary_data[:, 0], 
             label="x")
    ax2.plot(np.arange(0, len(stationary_data[:, 0])), stationary_data[:, 1], 
             label="y")
    ax3.plot(np.arange(0, len(stationary_data[:, 0])), stationary_data[:, 2],
             label="z")

    ax1.set_title("x direction")
    ax2.set_title("y direction")
    ax3.set_title("z direction")

    fig.supxlabel(f"Data points poll = {POLL_RATE}Hz")
    fig.supylabel("Acceleration $m/s^2$")
    fig.tight_layout()

    plt.savefig("./testing/figs/stationary_raw.png")
    plt.show()

def plot_xroll():
    """ Plot the accelerometer when rolling on the x axis"""
    xroll_data = np.loadtxt(x_roll_file, skiprows=2, delimiter=",")
    xroll_data[:,0:-1] *= SCALE_FACTOR

    fig, (ax1, ax2, ax3) = plt.subplots(3, sharex=True)

    ax1.plot(np.arange(0, len(xroll_data[:, 0])), xroll_data[:, 0], 
             label="x")
    ax2.plot(np.arange(0, len(xroll_data[:, 0])), xroll_data[:, 1], 
             label="y")
    ax3.plot(np.arange(0, len(xroll_data[:, 0])), xroll_data[:, 2],
             label="z")

    ax1.set_title("x direction")
    ax2.set_title("y direction")
    ax3.set_title("z direction")

    fig.supxlabel(f"Data points poll = {POLL_RATE}Hz")
    fig.supylabel("Acceleration $m/s^2$")
    fig.tight_layout()

    plt.savefig("./testing/figs/xroll_raw.png")
    plt.show()


def plot_yroll():
    """ Plot the data from a yroll test """
    yroll_data = np.loadtxt(y_roll_file, skiprows=2, delimiter=",")
    yroll_data[:,0:-1] *= SCALE_FACTOR

    fig, (ax1, ax2, ax3) = plt.subplots(3, sharex=True)

    ax1.plot(np.arange(0, len(yroll_data[:, 0])), yroll_data[:, 0], 
             label="x")
    ax2.plot(np.arange(0, len(yroll_data[:, 0])), yroll_data[:, 1], 
             label="y")
    ax3.plot(np.arange(0, len(yroll_data[:, 0])), yroll_data[:, 2],
             label="z")

    ax1.set_title("x direction")
    ax2.set_title("y direction")
    ax3.set_title("z direction")

    fig.supxlabel(f"Data points poll = {POLL_RATE}Hz")
    fig.supylabel("Acceleration $m/s^2$")
    fig.tight_layout()

    plt.savefig("./testing/figs/yroll_raw.png")
    plt.show()



if __name__ == "__main__":
    plot_stationary()
    plot_xroll()
    plot_yroll()
