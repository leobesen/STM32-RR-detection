import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from PyEMD import EEMD
from scipy import fftpack

seconds = 15 #[1:50]

time_init = 0
time_end = 15
# sample rate
ppg_sr = 100
acc_sr = 20
fsr_sr = 10
# FFT
samplingFrequency_PPG = 100
samplingInterval_PPG = 1 / 100
samplingInterval_FSR = 1 / 10

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
eemd = EEMD()
#eemd.extrema_detection="parabol"

# 0:15, 5:20, 10:25, 15:30, 20:35, 25:40, 30:45, 35:50
time_init = 0
time_end = 15 
for i in range(0,8,1):
    ppg_ = ppg_data[ppg_sr*time_init:ppg_sr*time_end]
    fsr_ = fsr_data[fsr_sr*time_init:fsr_sr*time_end]
    eIMFs_1 = eemd.eemd(ppg_)
    for j in range(len(eIMFs_1)):
        if(j==4):
            continue
        else:
            ppg_ = ppg_ - eIMFs_1[j]
    eIMFs_2 = eemd.eemd(ppg_)
    # FFT PPG
    fourierTransform = fftpack.fft(eIMFs_2[5],n=1024)
    amplitude = np.abs(fourierTransform)
    sample_freq = fftpack.fftfreq(len(eIMFs_2[5]),d=samplingInterval_PPG)
    amp_freq = np.array([amplitude,sample_freq])
    amp_pos = amp_freq[0].argmax()
    peak_freq_ppg = amp_freq[1][amp_pos]
    # FFT FSR
    fourierTransform = fftpack.fft(fsr_,n=128)
    amplitude = np.abs(fourierTransform)
    sample_freq = fftpack.fftfreq(len(fsr_),d=samplingInterval_FSR)
    amp_freq = np.array([amplitude,sample_freq])
    amp_pos = amp_freq[0][1:].argmax()
    peak_freq_fsr = amp_freq[1][amp_pos]

    print("PPG: " + str(peak_freq_ppg) + " Hz, FSR: " + str(peak_freq_fsr) + " Hz")
    
    
    #plt.plot(sample_freq[0:int(len(sample_freq)/20)], amplitude[0:int(len(sample_freq)/20)]) # without DC
        
    time_init += 5
    time_end += 5

#plt.show()
#-----------------------------------------------------------------#

