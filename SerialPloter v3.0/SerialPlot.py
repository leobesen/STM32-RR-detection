'''
    Modules
'''
from header import *
from DataPlot import *
from DSP_Functions import Functions
from drawnow import drawnow
import pandas as pd 

# classes
ppg = PPG(True)
ppg_DCR = PPG_DC_Remove(False)
ppg_ImpR = PPG_Impulse_Remove(False)
ppg_peak = PPG_Peak_Finder()
ppg_eemd = PPG_EEMD(False)
acc_1 = ACC(False)
acc_2 = ACC(False)
acc_3 = ACC(False)
acc_4 = ACC(False)
acc_ImpR = ACC_Impulse_Remove(False)
fsr = FSR(True)
counter = Counters()
plot = DataPlt()
func = Functions()
# Open serial port  
s = Serial()
s.initSerial()

ppg_n_points = 0

# Test port
if(s.portAvailable):
    # (CTRL + C) to end program
    try:
        while True:
            #read data
            data = s.readSerial()
            #data = np.array(data)
            # Force sensitive resistor (FSR)
            try:
                if(int(data[0])==28):
                    if(int(data[1])==1):
                        # print("FSR/r/n");
                        if(fsr.enable):
                            for i in range(3,13,1):
                                if(len(fsr.fsr) >= plot.numPoints_FSR):
                                    fsr.fsr.pop(0)
                                fsr.fsr.append(int(data[i])) # 2 - 11

                    if(int(data[1])==2):
                        if(acc_1.enable):
                            for i in range(3,18,3):
                                if(len(acc_1.X) >= plot.numPoints_ACC):
                                    acc_1.X.pop(0)
                                    acc_1.Y.pop(0)
                                    acc_1.Z.pop(0)
                                acc_1.X.append(float(data[i])*2.0/32768.0)
                                acc_1.Y.append(float(data[i+1])*2.0/32768.0) 
                                acc_1.Z.append(float(data[i+2])*2.0/32768.0) 
                        if(acc_2.enable):
                            for i in range(18,33,3):
                                if(len(acc_2.X) >= plot.numPoints_ACC):
                                    acc_2.X.pop(0)
                                    acc_2.Y.pop(0)
                                    acc_2.Z.pop(0)
                                acc_2.X.append(float(data[i])*2.0/32768.0)
                                acc_2.Y.append(float(data[i+1])*2.0/32768.0) 
                                acc_2.Z.append(float(data[i+2])*2.0/32768.0) 
                        if(acc_3.enable):        
                            for i in range(33,48,3):
                                if(len(acc_3.X) >= plot.numPoints_ACC):
                                    acc_3.X.pop(0)
                                    acc_3.Y.pop(0)
                                    acc_3.Z.pop(0)
                                acc_3.X.append(float(data[i])*2.0/32768.0)
                                acc_3.Y.append(float(data[i+1])*2.0/32768.0) 
                                acc_3.Z.append(float(data[i+2])*2.0/32768.0) 
                        if(acc_4.enable):
                            for i in range(48,63,3):
                                if(len(acc_4.X) >= plot.numPoints_ACC):
                                    acc_4.X.pop(0)
                                    acc_4.Y.pop(0)
                                    acc_4.Z.pop(0)
                                acc_4.X.append(float(data[i])*2.0/32768.0)
                                acc_4.Y.append(float(data[i+1])*2.0/32768.0) 
                                acc_4.Z.append(float(data[i+2])*2.0/32768.0) 
                    
                    if(int(data[1])==3):
                        if(ppg.enable):
                            for i in range(3,28,1):
                                if(len(ppg.RED_LED) >= plot.numPoints_PPG):
                                    ppg.RED_LED.pop(0)
                                ppg.RED_LED.append(int(data[i]))
                            for i in range(28,53,1):
                                if(len(ppg.IR_LED) >= plot.numPoints_PPG):
                                    ppg.IR_LED.pop(0)
                                ppg.IR_LED.append(int(data[i]))

            except:
                print("error getting data")

            #DSP            
            #ACC Impulse Remove
            if(acc_1.enable):
                if(acc_ImpR.enable):
                    if(len(acc_ImpR.X_temp) < acc_ImpR.N):
                        acc_ImpR.X_temp.append(acc.X[-1])
                        acc_ImpR.Y_temp.append(acc.Y[-1])
                        acc_ImpR.Z_temp.append(acc.Z[-1])
                    else:
                        acc_ImpR.X_Imp_Removed.append(func.impulseRemoval(acc_ImpR.X_temp,acc_ImpR.N))
                        acc_ImpR.Y_Imp_Removed.append(func.impulseRemoval(acc_ImpR.Y_temp,acc_ImpR.N))
                        acc_ImpR.Z_Imp_Removed.append(func.impulseRemoval(acc_ImpR.Z_temp,acc_ImpR.N))
                        if(len(acc_ImpR.X_Imp_Removed) >= plot.numPoints_ACC):
                            acc_ImpR.X_Imp_Removed.pop(0)
                            acc_ImpR.Y_Imp_Removed.pop(0)
                            acc_ImpR.Z_Imp_Removed.pop(0)
                        acc_ImpR.X_temp.pop(0)
                        acc_ImpR.Y_temp.pop(0)
                        acc_ImpR.Z_temp.pop(0)
                        acc_ImpR.X_temp.append(acc.X[-1])
                        acc_ImpR.Y_temp.append(acc.Y[-1])
                        acc_ImpR.Z_temp.append(acc.Z[-1])
            #PPG EMD
            if(ppg.enable):
                if(ppg_eemd.enable):
                    if(len(ppg.IR_LED) == plot.numPoints_PPG):
                        if(ppg_n_points >= 500): # 5 seconds
                            ppg_ = np.array(ppg.IR_LED)
                            eIMFs_1 = func.eemd_ppg(ppg_)
                            for j in range(len(eIMFs_1)):
                                if(j==4):
                                    continue
                                else:
                                    ppg_ = ppg_ - eIMFs_1[j]
                            eIMFs_2 = func.eemd_ppg(ppg_)
                            # FFT PPG
                            peak_freq_ppg = func.fft(eIMFs_2[5],100,1024)
                            # FFT FSR
                            peak_freq_fsr = func.fft(fsr.fsr,10,128)
                            print("PPG: " + str(peak_freq_ppg) + " Hz, FSR: " + str(peak_freq_fsr) + " Hz")
                            ppg_n_points = 0

            if counter.cnt == 0:
                imfs_IR = [[],[],[],[],[],[],[],[],[]]
                eIMFs_2 = [[],[],[],[],[],[],[],[],[]]
            
            #Choose data to plot
            plot.numPlots = 2
            plot.var["0"] = ppg.RED_LED
            plot.var["1"] = ppg.IR_LED
            plot.var["2"] = acc_3.Z
            plot.var["3"] = acc_4.Z
            plot.var["4"] = imfs_IR[5] + imfs_IR[6]
            plot.var["5"] = ppg.IR_LED

            plot.var_label["0"] = 'fsr.fsr'
            plot.var_label["1"] = 'ppg.IR_LED'
            plot.var_label["2"] = 'eIMFs_2[5]'
            plot.var_label["3"] = 'imfs_IR[6]'
            plot.var_label["4"] = 'imfs_IR[5] + imfs_IR[6]'
            plot.var_label["5"] = 'ppg.IR_LED'

            plot.var_style["0"] = 'go-'
            plot.var_style["1"] = 'ro-'
            plot.var_style["2"] = 'bo-'
            plot.var_style["3"] = 'go-'
            plot.var_style["4"] = 'ko-'
            plot.var_style["5"] = 'co-'

            #Plot Control
            plot.plotCounter=0
            drawnow(plot.makeFig)
            plt.pause(.0000001)

            #Control counters
            counter.cnt+=1
            plot.plotCounter+=1
            counter.cnt_peak+=1
            ppg_eemd.cnt_emd+=1
            #print(counter.cnt)
            #print(fsr.fsr[0:5])

            # if (counter.cnt >= 256):
            #     if (counter.cnt%40 == 0):
            #         func.fft(ppg_DCR.IR_DC_Removed[0:256], 256)

            # #save data
            # if(len(ppg.IR_LED) == plot.numPoints_PPG):
            #     dict_FSR = {}
            #     dict_ACC = {}
            #     dict_PPG = {}
            #     dict_FSR['FSR'] = fsr.fsr
            #     dict_ACC['ACC.Z'] = acc.Z
            #     dict_PPG['PPG.IR'] = ppg.IR_LED
                
            #     dt = pd.DataFrame(list(dict_FSR['FSR']),columns = ['FSR'])
            #     dt.to_csv('fsr_data.csv')
                
            #     dt = pd.DataFrame(list(dict_ACC['ACC.Z']),columns = ['ACC'])
            #     dt.to_csv('acc_data.csv')
                
            #     dt = pd.DataFrame(list(dict_PPG['PPG.IR']),columns = ['PPG'])
            #     dt.to_csv('ppg_data.csv') 



    except KeyboardInterrupt:
        print ("Program closed by Keyboard interrupt") 

