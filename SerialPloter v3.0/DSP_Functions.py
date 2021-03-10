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
        self.samplingFrequency_PPG = 100
        self.samplingFrequency_ACC = 20
        self.samplingFrequency_FSR = 10
        self.samplingInterval_PPG = 1 / 100
        self.samplingInterval_ACC = 1 / 20
        self.samplingInterval_FSR = 1 / 10

    
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

    def fft(self, signal, sr, l):
        

        fourierTransform = fftpack.fft(signal,n=l)
        amplitude = np.abs(fourierTransform)
        sample_freq = fftpack.fftfreq(len(signal),d=1/sr)
        amp_freq = np.array([amplitude,sample_freq])
        #print(amp_freq.shape)
        amp_pos = amp_freq[0][1:len(amp_freq[0]/2)].argmax()
        peak_freq = amp_freq[1][amp_pos]
        peak_energy = amp_freq[0][amp_pos]
        plt.figure()
        plt.plot(sample_freq[1:int(len(sample_freq)/2)], amplitude[1:int(len(sample_freq)/2)],'b-') # without DC
        #plt.show()

        return [peak_freq, peak_energy]

    def chooseBestAccAxis(self, signal,sr,l):
        
        # FFT on each axis of all acc's
        [peak_freq, peak_energy] = self.fft(signal,sr,l)
        # Select signal with major frequency bean between 0.2 and 0.6 Hz

        # Select signal with greater energy on that frequency
        return AccAxis
    