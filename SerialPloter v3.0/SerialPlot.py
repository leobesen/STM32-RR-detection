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
ppg_eemd = PPG_EEMD(False)
acc = ACC(True)
acc_ImpR = ACC_Impulse_Remove(True)
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
            if(acc.enable):
                if(len(acc.X) >= plot.numPointToPlot):
                    acc.X.pop(0)
                    acc.Y.pop(0)
                    acc.Z.pop(0)
                acc.X.append(float(data[0])*2.0/32768.0)
                acc.Y.append(float(data[1])*2.0/32768.0)
                acc.Z.append(float(data[2])*2.0/32768.0)
            if(ppg.enable):
                if(len(ppg.IR_LED) >= plot.numPointToPlot):
                    ppg.RED_LED.pop(0)
                    ppg.IR_LED.pop(0)
                ppg.RED_LED.append(int(data[3]))
                ppg.IR_LED.append(int(data[4]))

            #DSP
            #PPG DC Remove
            if(ppg.enable):
                if(ppg_DCR.enable):
                    RED_dcR, ppg_DCR.RED_bw = func.dcRemoval(ppg.RED_LED[-1], ppg_DCR.RED_bw)
                    IR_dcR, ppg_DCR.IR_bw = func.dcRemoval(ppg.IR_LED[-1], ppg_DCR.IR_bw)
                    if(len(ppg_DCR.RED_DC_Removed) >= plot.numPointToPlot):
                        ppg_DCR.RED_DC_Removed.pop(0)
                        ppg_DCR.IR_DC_Removed.pop(0)    
                    ppg_DCR.RED_DC_Removed.append(RED_dcR)
                    ppg_DCR.IR_DC_Removed.append(IR_dcR)
            
            #PPG Impulse Remove
            if(ppg.enable):
                if(ppg_ImpR.enable):
                    if(len(ppg_ImpR.IR_temp) < ppg_ImpR.N):
                        ppg_ImpR.IR_temp.append(ppg.IR_LED[-1])
                        ppg_ImpR.RED_temp.append(ppg.RED_LED[-1])
                    else:
                        ppg_ImpR.IR_Imp_Removed.append(func.impulseRemoval(ppg_ImpR.IR_temp,ppg_ImpR.N))
                        ppg_ImpR.RED_Imp_Removed.append(func.impulseRemoval(ppg_ImpR.RED_temp,ppg_ImpR.N))
                        ppg_ImpR.IR_temp.pop(0)
                        ppg_ImpR.RED_temp.pop(0)
                        ppg_ImpR.IR_temp.append(ppg.IR_LED[-1])
                        ppg_ImpR.RED_temp.append(ppg.RED_LED[-1])
            
            #PPG peak finder
            if(ppg.enable):
                if(ppg_ImpR.enable):
                    IR_array = np.array(ppg_ImpR.IR_Imp_Removed)
                    if(counter.cnt>=3):
                        if((IR_array[-2] > IR_array[-3]) & (IR_array[-2] > IR_array[-1])):
                            if(IR_array[-2] > 0):
                                last_positive_peak = IR_array[-2]
                        if ((IR_array[-2] < IR_array[-3]) & (IR_array[-2] < IR_array[-1])):
                            if(IR_array[-2] < 0):
                                ppg_peak.diff_pos_neg_peak.append(last_positive_peak - IR_array[-2])
                        else:
                            ppg_peak.diff_pos_neg_peak.append(0)

                        diff = len(ppg_peak.diff_pos_neg_peak) - plot.numPointToPlot
                        if(diff > 0):
                            for i in range(diff):
                                ppg_peak.diff_pos_neg_peak.pop(0)

            
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
                        if(len(acc_ImpR.X_Imp_Removed) >= plot.numPointToPlot):
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
                    if(counter.cnt >= plot.numPointToPlot):
                        if(ppg_eemd.cnt_emd >= plot.numPointToPlot):
                            if(len(ppg.IR_LED) == plot.numPointToPlot):
                                imfs_IR = func.eemd_ppg(ppg.IR_LED)
                                ppg_eemd.cnt_emd = 0
                        else:
                            if(len(ppg_eemd.IR_EEMD) >= plot.numPointToPlot):
                                ppg_eemd.IR_EEMD.pop(0)
                                ppg_eemd.IR_HIST.pop(0)
                                ppg_eemd.ACC_Z_HIST.pop(0)
                            ppg_eemd.IR_EEMD.append(imfs_IR[ppg_eemd.imfs_number][ppg_eemd.cnt_emd-1])
                            ppg_eemd.IR_HIST.append(ppg.IR_LED[0])
                            ppg_eemd.ACC_Z_HIST.append(acc_ImpR.Z_Imp_Removed[0])
        
            
            #Choose data to plot
            plot.numPlots = 2
            plot.var1 = acc_ImpR.Z_Imp_Removed
            plot.var1_label = 'ACC Z'
            plot.var2 = ppg.IR_LED
            plot.var2_label = 'PPG IR'
            #Plot Control
            if(plot.plotCounter >= plot.plotCounterMax):
                plot.plotCounter=0
                if(plot.numPlots == 1):
                    drawnow(plot.makeFig_oneVar)
                elif(plot.numPlots == 2):
                    drawnow(plot.makeFig_twoVar)
                plt.pause(.0000001)

            #Control counters
            counter.cnt+=1
            plot.plotCounter+=1
            counter.cnt_peak+=1
            ppg_eemd.cnt_emd+=1


    except KeyboardInterrupt:
        print ("Program closed by Keyboard interrupt") 

