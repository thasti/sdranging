import numpy as np
import matplotlib.pyplot as plt

range_data = np.loadtxt("range_data.csv")

target = 38315.40
avg_meas = np.mean(range_data)
error = avg_meas - target

plt.axhline(y=target, color='black', label="Prediction")
plt.plot(range_data, '.', color='black', label="Measurement")
plt.legend()
plt.title("Es'Hail 2 ranging results - mean error: %.02f km" % error)
plt.xlabel("Measurement number")
plt.ylabel("Slant range [km]")
plt.ylim((target-5, target+10))
plt.grid()
plt.show()
