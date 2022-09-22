# HACK: Do this because the sys.path doesn't find user site-packages right now.
import sys
sys.path.append('C:\\Users\\jklei\\AppData\\Local\\Packages\\PythonSoftwareFoundation.Python.3.10_qbz5n2kfra8p0\\LocalCache\\local-packages\\Python310\\site-packages')

# Imports
import numpy as np
import matplotlib.pyplot as plt

A = 4
B = 3
C = A + B
D = np.identity(2)

print(A)
print(D)

x = np.linspace(0,1,10)
y = 5*x + 4

plt.scatter(x,y)
plt.show()
