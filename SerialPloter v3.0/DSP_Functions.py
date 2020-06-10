from PyEMD import EEMD
import numpy as np 
import matplotlib.pyplot as plt 
from scipy import fftpack


class Functions:
    def __init__(self):
        self.alpha = 0.99
        self.imfs_number = 5
        self.eemd = EEMD()
        self.eemd.extrema_detection="parabol"
        self.samplingFrequency = 40
        self.samplingInterval = 1 / 40
        self.time = np.arange(0, 5, self.samplingInterval)

    
    def dcRemoval(self,x, w):
        w_n = x + self.alpha*w
        y = w_n - w
        return y,w_n

    def impulseRemoval(self, samples,N):
        mean = np.mean(samples)
        pos = 0
        neg = 0
        diff_pos = 0
        for i in range(0, N,1):
            if(samples[i] > mean):
                pos+=1
                diff_pos = samples[i] - mean
            elif (samples[i] < mean):
                neg+=1
        return (mean + (pos - neg)*diff_pos / (N*N))

    def eemd_ppg(self,ppg_signal):
        eIMFs = self.eemd.eemd(ppg_signal)
        return eIMFs

    def fft(self, signal, l):
        
        fourierTransform = fftpack.fft(signal,n=l)
        amplitude = np.abs(fourierTransform)
        sample_freq = fftpack.fftfreq(len(signal),d=self.samplingInterval)
        
        amp_freq = np.array([amplitude,sample_freq])
        amp_pos = amp_freq[0,1:].argmax()
        
        peak_freq = amp_freq[1,amp_pos+1]
        print(peak_freq)
        #plt.plot(sample_freq[1:int(len(sample_freq)/2)], amplitude[1:int(len(sample_freq)/2)],'b-') # without DC
        #plt.show()
    