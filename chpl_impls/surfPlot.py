import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits import mplot3d

filePath = sys.argv[1]
x = np.zeros(1)
y = np.zeros(1)

with open(filePath + '.meta', 'r') as meta:
    md = meta.readline().split(',')
    nx = int(md[0])
    ny = int(md[1])
    xLen = float(md[2])
    yLen = float(md[3])
    x = np.linspace(0.0, xLen, nx)
    y = np.linspace(0.0, yLen, ny)

X, Y = np.meshgrid(y, x)
Z = np.loadtxt(filePath + '.dat').T

# fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
fig = plt.figure()
ax = plt.axes(projection='3d')
surf = ax.plot_surface(X, Y, Z, cmap=cm.viridis, linewidth=0)

ax.set_xlabel('X')
ax.set_ylabel('Y')

ax.set_title(sys.argv[2])
fig.colorbar(surf, label=sys.argv[3])

ax.view_init(elev=25, azim=220)
plt.savefig(filePath + '.png')
