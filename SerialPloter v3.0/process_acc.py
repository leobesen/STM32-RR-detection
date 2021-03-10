import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from DSP_Functions import Functions
func = Functions()
acc = []
# read data from csv file and convert to array
df = pd.read_csv('data/acc_1_data_1.csv')
acc_1 = np.array(df)
df = pd.read_csv('data/acc_2_data_1.csv')
acc_2 = np.array(df)
df = pd.read_csv('data/acc_3_data_1.csv')
acc_3 = np.array(df)
df = pd.read_csv('data/acc_4_data_1.csv')
acc_4 = np.array(df)

for

peak_freq = []
peak_energy = []
for j in range(0,len(acc_1[0,:])):
    w = 0
    for i in range(0,len(acc_1[:,j])):
        [acc_1[i,j],w] = func.dcRemoval(acc_1[i,j],w)
    [f, e] = func.fft(acc_1[2000:2512,j], 20, 512)
    peak_freq.append(f)
    peak_energy.append(e)

print(peak_freq, peak_energy)

# plt.show()
