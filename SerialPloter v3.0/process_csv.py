import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

class SpO2_HR:
    def __init__(self):
        super().__init__()
        self.min_distance = 50 # mim distance between two peaks
        self.max_num = 50 # max number of peaks
        self.npks = 0 # number of peak values
        self.locs = np.array(np.zeros(self.max_num)) # index of the peak values
        self.x = [] # array of ppg values to process
        self.size = 0 # size of the array
        self.min_height = 0 # min height to find the peaks

    def find_peaks(self):
   
        # find all the peaks above the min height
        self.peaks_above_min_height()
        # remove peaks too close - min distance
        #self.remove_close_peaks()
        # npks = min()

    def peaks_above_min_height(self):
   
        i = 1
        self.npks = 0
        # i = [1,size-1]
        while i < self.size-1:
            if (self.x[i] > self.min_height and self.x[i] > self.x[i-1]):  # find left edge of potential peaks
                n_width = 1
                while i+n_width < self.size and self.x[i] == self.x[i+n_width]:  # find flat peaks
                    n_width+=1
                if self.x[i] > self.x[i+n_width] and self.npks < self.max_num: # find right edge of peaks
                    self.locs[self.npks] = i
                    self.npks+=1   
                    # for flat peaks, peak location is left edge
                    i += n_width+1
                    i += self.min_distance
                else:
                    i += n_width
            else:
                i+=1
        print('peaks_above_min_height: ')
        print(self.locs)
        print("----")
    


    def remove_close_peaks(self):    
        
        self.sort_indices_descend() #Order peaks from large to small
        # new_locs = []
        # i=0
        # while i < self.npks:
        #     for j in range(i+1,self.npks):
        #         dist = self.locs[i] - self.locs[j]
        #         if np.abs(dist) < self.min_distance:

        
        # Remove peaks separated by less than MIN_DISTANCE
        for i in range(-1,self.npks):
            n_old_npks = self.npks
            self.npks = i+1 ### 0
            for j in range(i+1,n_old_npks,1):
                n_dist =  self.locs[j] - ((i==-1)*(1)+(i!=-1)*self.locs[i])
                if n_dist > self.min_distance or n_dist < -self.min_distance:
                    self.locs[self.npks] = self.locs[j]
                    self.npks+=1

        self.sort_ascend() # Resort indices int32_to ascending order


    def sort_ascend(self):
        # Sort array in ascending order (insertion sort algorithm)
        for i in range(1, self.size): 
            key = self.x[i] 
            j = i-1
            while j >= 0 and key < self.x[j] : 
                    self.x[j + 1] = self.x[j] 
                    j -= 1
            self.x[j + 1] = key 

    def sort_indices_descend(self):
        #Sort indices according to descending order (insertion sort algorithm) 
        for i in range(1,self.npks):
            n_temp = self.locs[i]
            j=i
            while (j > 0) and (n_temp > self.locs[j-1]):
                self.locs[j] = self.locs[j-1]
                j-=1
            self.locs[j] = n_temp

        print("sort_indices_descend")
        print(self.locs)
        print('-----')


seconds = 10 #[1:50]

time_init = 20
time_end = 30
# sample rate
ppg_sr = 100
acc_sr = 20
fsr_sr = 10

# read data from csv file and convert to array
df = pd.read_csv('fsr_data.csv') 
fsr_data = np.array(df['FSR'])
#print(fsr_data.shape)
df = pd.read_csv('acc_data.csv') 
acc_data = np.array(df['ACC'])
#print(acc_data.shape)
df = pd.read_csv('ppg_data.csv') 
ppg_data = np.array(df['PPG'])
#print(ppg_data.shape)

# plot data
plt.figure(num=1)
# plt.subplot(3,1,1)
# plt.plot(fsr_data[time_init*fsr_sr:time_end*fsr_sr], 'b-', label='fsr')                                  
# plt.grid(True)                             
# plt.legend(loc='upper left')

# plt.subplot(3,1,2)
# plt.plot(acc_data[time_init*acc_sr:time_end*acc_sr], 'g-', label='acc Z')                                  
# plt.grid(True)                             
# plt.legend(loc='upper left')

plt.subplot(3,1,1)
plt.plot(acc_data[time_init*acc_sr:time_end*acc_sr], 'r-', label='ACC')                                  
plt.grid(True)                             
plt.legend(loc='upper left')



spo2_hr = SpO2_HR()
BUFFER_SIZE = ir_buffer_length = seconds*acc_sr
MA4_SIZE = 20
ir_buffer = []
for i in range(seconds*acc_sr):
    ir_buffer.append(acc_data[i + time_init*acc_sr])
#print(len(ir_buffer))
#print(ir_buffer_length)
# calculates DC mean and subtract DC from IR
ir_mean=0; 
for k in range(ir_buffer_length): 
    ir_mean += ir_buffer[k];
ir_mean=ir_mean/ir_buffer_length;
    
# remove DC and invert signal so that we can use peak detector as valley detector
x = np.array(np.zeros(ir_buffer_length))
for k in range(ir_buffer_length): 
    x[k]= (ir_buffer[k] - ir_mean); 

plt.subplot(3,1,2)
plt.plot(x, 'b-', label='ACC')                                  
plt.grid(True)                             
plt.legend(loc='upper left')
    
# 4 pt Moving Average
for k in range(BUFFER_SIZE-MA4_SIZE):
    for i in range(MA4_SIZE):
        x[k] += x[k+i] 
    x[k]/=MA4_SIZE;

z=1
for k in range(BUFFER_SIZE-MA4_SIZE,BUFFER_SIZE,1):
    for i in range(MA4_SIZE-z):
        x[k] += x[k+i] 
    x[k]/=MA4_SIZE;
    z += 1

plt.subplot(3,1,3)
plt.plot(x, 'c-', label='ACC')                                  
plt.grid(True)                             
plt.legend(loc='upper left') 


# calculate threshold  
th1=0
for k in range(BUFFER_SIZE):
    th1+=np.abs(x[k]);

th1=th1/BUFFER_SIZE
print('th1: ' + str(th1))
# if(th1<30):
#     th1=30 # min allowed
# if(th1>60):
#     th1=60 # max allowed


spo2_hr.max_num = 50
spo2_hr.min_distance = 30
spo2_hr.min_height = th1
spo2_hr.size = BUFFER_SIZE
spo2_hr.x = list(x).copy()
spo2_hr.find_peaks()

locs = []
for i in range(len(spo2_hr.locs)):
    locs.append(int(spo2_hr.locs[i]))

plt.subplot(3,1,3)    
plt.plot(locs[0:spo2_hr.npks],x[locs[0:spo2_hr.npks]], 'ko', label='acc')   
peak_interval_sum=0

if spo2_hr.npks>=2:
    for k in range(1,spo2_hr.npks):
         peak_interval_sum += (spo2_hr.locs[k]-spo2_hr.locs[k-1])
    peak_interval_sum=peak_interval_sum/(spo2_hr.npks-1)
    heart_rate =(acc_sr*60)/peak_interval_sum
    hr_valid  = 1
else: 
    heart_rate = -999 # unable to calculate because number of peaks are too small
    hr_valid  = 0

print("RR: " + str(heart_rate) + ' bhpm')


plt.show()




