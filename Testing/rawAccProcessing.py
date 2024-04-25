import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("./Testing/rawAccel.txt", skiprows=2, delimiter=",")

plt.plot(np.arange(0, len(data[:, 0])), data[:, 0], label="x")
plt.plot(np.arange(0, len(data[:, 0])), data[:, 1], label="y")
plt.plot(np.arange(0, len(data[:, 0])), data[:, 2], label="z")

plt.legend()
plt.show()