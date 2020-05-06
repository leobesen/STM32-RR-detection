from PyEMD import EEMD
import numpy as np 

class Functions:
    def __init__(self):
        self.alpha = 0.99
        self.imfs_number = 5
        self.eemd = EEMD()
        self.eemd.extrema_detection="parabol"
    
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