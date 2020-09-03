import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from PyEMD import EMD
from scipy import fftpack

seconds = 15 #[1:50]

time_init = 0
time_end = 5
# sample rate
ppg_sr = 100
acc_sr = 20
fsr_sr = 10


# read data from csv file and convert to array
df = pd.read_csv('fsr_data.csv') 
fsr_data = np.array(df['FSR'])
print(fsr_data.shape)
df = pd.read_csv('acc_data.csv') 
acc_data = np.array(df['ACC'])
print(acc_data.shape)
df = pd.read_csv('ppg_data.csv') 
ppg_data = np.array(df['PPG'])
print(ppg_data.shape)


# EEMD process
emd = EMD()
#eemd.extrema_detection="parabol"

eIMFs_1 = emd(ppg_data[ppg_sr*time_init:ppg_sr*time_end])
#-----------------------------------------------------------------#
plt.figure(num=1)
for i in range(len(eIMFs_1)):
    plt.subplot(len(eIMFs_1)+3,1,i+1)
    plt.plot(eIMFs_1[i], 'b-', label='eIMF_'+str(i))                                  
    plt.grid(True)                             
    plt.legend(loc='upper left')

plt.subplot(len(eIMFs_1)+3,1,len(eIMFs_1)+1)
plt.plot(ppg_data[ppg_sr*time_init:ppg_sr*time_end], 'b-', label='ppg')                                  
plt.grid(True)                             
plt.legend(loc='upper left')


new_ppg = ppg_data[ppg_sr*time_init:ppg_sr*time_end]
for i in range(0,4,1):
    if(i==4):
        continue
    else:
        new_ppg = new_ppg - eIMFs_1[i]

plt.subplot(len(eIMFs_1)+3,1,len(eIMFs_1)+2)
plt.plot(new_ppg, 'b-', label='new_ppg')                                  
plt.grid(True)                             
plt.legend(loc='upper left')

plt.subplot(len(eIMFs_1)+3,1,len(eIMFs_1)+3)
plt.plot(fsr_data[fsr_sr*time_init:fsr_sr*time_end], 'b-', label='fsr')                                  
plt.grid(True)                             
plt.legend(loc='upper left')
#----------------------------------------------------------------------------#
plt.figure(num=2)
plt.subplot(3,1,1)
plt.plot(new_ppg, 'b-', label='new_ppg')                                  
plt.grid(True)                             
plt.legend(loc='upper left')

plt.subplot(3,1,2)
plt.plot(eIMFs_1[4], 'b-', label='eIMFs_1[4]')                                  
plt.grid(True)                             
plt.legend(loc='upper left')

plt.subplot(3,1,3)
plt.plot(fsr_data[fsr_sr*time_init:fsr_sr*time_end], 'b-', label='fsr')                                  
plt.grid(True)                             
plt.legend(loc='upper left')

#-----------------------------------------------------------------------------#
eIMFs_2 = emd(new_ppg)
eIMFs_3 = emd(eIMFs_1[4])

plt.figure(num=3)
plt.subplot(3,1,1)
plt.plot(eIMFs_2[5], 'b-', label='new_ppg eIMFs_2')                                  
plt.grid(True)                             
plt.legend(loc='upper left')

plt.subplot(3,1,2)
plt.plot(eIMFs_3[5], 'b-', label='eIMFs_1[4] - eIMFs_3')                                  
plt.grid(True)                             
plt.legend(loc='upper left')

plt.subplot(3,1,3)
plt.plot(fsr_data[fsr_sr*time_init:fsr_sr*time_end], 'b-', label='fsr')                                  
plt.grid(True)                             
plt.legend(loc='upper left')

plt.show()
