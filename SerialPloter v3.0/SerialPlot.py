'''
    Modules
'''
from header import *
from DataPlot import *
from DSP_Functions import Functions
from drawnow import drawnow

# classes
ppg = PPG(True)
ppg_DCR = PPG_DC_Remove(False)
ppg_ImpR = PPG_Impulse_Remove(False)
ppg_peak = PPG_Peak_Finder()
ppg_eemd = PPG_EEMD(True)
acc = ACC(False)
acc_ImpR = ACC_Impulse_Remove(False)
fsr = FSR(True)
counter = Counters()
plot = DataPlt()
func = Functions()
# Open serial port
s = Serial()
s.initSerial()
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
                if(int(data[0])==64000):
                    # print(int(data[0]))
                    if(fsr.enable):
                        for i in range(2,12,1):
                            if(len(fsr.fsr) >= plot.numPoints_FSR):
                                fsr.fsr.pop(0)
                            fsr.fsr.append(int(data[i])) # 2 - 11
                    # Accelerometer (ACC)
                    if(acc.enable):
                        for i in range(0,20,1):
                            if(len(acc.X) >= plot.numPoints_ACC):
                                acc.X.pop(0)
                                acc.Y.pop(0)
                                acc.Z.pop(0)
                            acc.X.append(float(data[12+i])*2.0/32768.0) # 12 - 31
                            acc.Y.append(float(data[32+i])*2.0/32768.0) # 32 - 51
                            acc.Z.append(float(data[52+i])*2.0/32768.0) # 52 - 71
                elif(int(data[0])==44800):
                    # Photoplethysmography (PPG)
                    if(ppg.enable):
                        for i in range(2,100,1):
                            if(len(ppg.IR_LED) >= plot.numPoints_PPG):
                                #ppg.RED_LED.pop(0)
                                ppg.IR_LED.pop(0)
                            #ppg.RED_LED.append(int(data[7+i]))
                            ppg.IR_LED.append(int(data[i])) # 2 - 102
            except:
                print("error getting data")

            #DSP
            #PPG DC Remove
            if(ppg.enable):
                if(ppg_DCR.enable):
                    RED_dcR, ppg_DCR.RED_bw = func.dcRemoval(ppg.RED_LED[-1], ppg_DCR.RED_bw)
                    IR_dcR, ppg_DCR.IR_bw = func.dcRemoval(ppg.IR_LED[-1], ppg_DCR.IR_bw)
                    if(len(ppg_DCR.RED_DC_Removed) >= plot.numPoints_PPG):
                        ppg_DCR.RED_DC_Removed.pop(0)
                        ppg_DCR.IR_DC_Removed.pop(0)    
                    ppg_DCR.RED_DC_Removed.append(RED_dcR)
                    ppg_DCR.IR_DC_Removed.append(IR_dcR)
            
            #PPG Impulse Remove
            if(ppg.enable):
                if(ppg_ImpR.enable):
                    if(len(ppg_ImpR.IR_temp) < ppg_ImpR.N):
                        for i in range(5,0,-1):
                            ppg_ImpR.IR_temp.append(ppg.IR_LED[-i])
                            ppg_ImpR.RED_temp.append(ppg.RED_LED[-i])
                    else:

                        ppg_ImpR.IR_Imp_Removed.append(func.impulseRemoval(ppg_ImpR.IR_temp,ppg_ImpR.N))
                        ppg_ImpR.RED_Imp_Removed.append(func.impulseRemoval(ppg_ImpR.RED_temp,ppg_ImpR.N))
                        if(len(ppg_ImpR.IR_Imp_Removed) >= plot.numPoints_PPG):
                            ppg_ImpR.IR_Imp_Removed.pop(0)
                            ppg_ImpR.RED_Imp_Removed.pop(0)
                        ppg_ImpR.IR_temp.pop(0)
                        ppg_ImpR.RED_temp.pop(0)
                        ppg_ImpR.IR_temp.append(ppg.IR_LED[-1])
                        ppg_ImpR.RED_temp.append(ppg.RED_LED[-1])
            
            # #PPG peak finder
            # if(ppg.enable):
            #     if(ppg_ImpR.enable):
            #         IR_array = np.array(ppg_ImpR.IR_Imp_Removed)
            #         if(counter.cnt>=3):
            #             if((IR_array[-2] > IR_array[-3]) & (IR_array[-2] > IR_array[-1])):
            #                 if(IR_array[-2] > 0):
            #                     last_positive_peak = IR_array[-2]
            #             if ((IR_array[-2] < IR_array[-3]) & (IR_array[-2] < IR_array[-1])):
            #                 if(IR_array[-2] < 0):
            #                     ppg_peak.diff_pos_neg_peak.append(last_positive_peak - IR_array[-2])
            #             else:
            #                 ppg_peak.diff_pos_neg_peak.append(0)

            #             diff = len(ppg_peak.diff_pos_neg_peak) - plot.numPointToPlot
            #             if(diff > 0):
            #                 for i in range(diff):
            #                     ppg_peak.diff_pos_neg_peak.pop(0)
            
            #ACC Impulse Remove
            if(acc.enable):
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
                    if(ppg_eemd.cnt_emd >= 5):
                        if(len(ppg.IR_LED) == plot.numPoints_PPG):
                            imfs_IR = func.eemd_ppg(np.array(ppg.IR_LED))
                            ppg_eemd.cnt_emd = 0
                            # func.fft(imfs_IR[4], 1024)
                            #func.fft(fsr.fsr, 1024)
            if counter.cnt == 0:
                imfs_IR = [[],[],[],[],[],[],[],[],[]]
            
            #Choose data to plot
            plot.numPlots = 5
            plot.var["0"] = fsr.fsr
            plot.var["1"] = imfs_IR[6]
            plot.var["2"] = imfs_IR[7]
            plot.var["3"] = imfs_IR[6] + imfs_IR[7]
            plot.var["4"] = ppg.IR_LED

            plot.var_label["0"] = 'fsr.fsr'
            plot.var_label["1"] = 'imfs_IR[6]'
            plot.var_label["2"] = 'imfs_IR[7]'
            plot.var_label["3"] = 'imfs_IR[6] + imfs_IR[7]'
            plot.var_label["4"] = 'ppg.IR_LED'

            plot.var_style["0"] = 'bo-'
            plot.var_style["1"] = 'yo-'
            plot.var_style["2"] = 'ro-'
            plot.var_style["3"] = 'go-'
            plot.var_style["4"] = 'ko-'

            #Plot Control
            #if(plot.plotCounter >= plot.plotCounterMax):
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


    except KeyboardInterrupt:
        print ("Program closed by Keyboard interrupt") 

