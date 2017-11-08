#ifndef TDC7200_REGISTERS_H
#define TDC7200_REGISTERS_H

/*! ---------- TDC7200 REGISTERS FIELD VALUES ---------- !*/

/* CONFIG1 */
#define TDC7200_CONFIG1_FORCE_CALIBRATION_NO      0           /** 1 бит **/
#define TDC7200_CONFIG1_FORCE_CALIBRATION_ALWAYS  (1 << 7)    

#define TDC7200_CONFIG1_PARITY_DISABLED           0           /** 1 бит **/
#define TDC7200_CONFIG1_PARITY_ENABLED            (1 << 6)    

#define TDC7200_CONFIG1_TRIGG_EDGE_RISING         0           /** 1 бит **/
#define TDC7200_CONFIG1_TRIGG_EDGE_FALLING        (1 << 5)    

#define TDC7200_CONFIG1_STOP_EDGE_RISING          0           /** 1 бит **/
#define TDC7200_CONFIG1_STOP_EDGE_FALLING         (1 << 4)    

#define TDC7200_CONFIG1_START_EDGE_RISING         0           /** 1 бит **/
#define TDC7200_CONFIG1_START_EDGE_FALLING        (1 << 3)    

#define TDC7200_CONFIG1_MEAS_MODE_1               0           /** 2 бита **/
#define TDC7200_CONFIG1_MEAS_MODE_2               (1 << 1)    

#define TDC7200_CONFIG1_START_MEAS_NO             0           /** 1 бит **/
#define TDC7200_CONFIG1_START_MEAS_NEW            1           

/* CONFIG2 */
#define TDC7200_CONFIG2_CALIBRATION2_PERIODS_2    0      
#define TDC7200_CONFIG2_CALIBRATION2_PERIODS_10   (1 << 6)    /** 2 бита **/
#define TDC7200_CONFIG2_CALIBRATION2_PERIODS_20   (2 << 6)
#define TDC7200_CONFIG2_CALIBRATION2_PERIODS_40   (3 << 6)

#define TDC7200_CONFIG2_AVG_CYCLES_1              0       
#define TDC7200_CONFIG2_AVG_CYCLES_2              (1 << 3)    /** 3 бита **/
#define TDC7200_CONFIG2_AVG_CYCLES_4              (2 << 3)
#define TDC7200_CONFIG2_AVG_CYCLES_8              (3 << 3)
#define TDC7200_CONFIG2_AVG_CYCLES_16             (4 << 3)
#define TDC7200_CONFIG2_AVG_CYCLES_32             (5 << 3)
#define TDC7200_CONFIG2_AVG_CYCLES_64             (6 << 3)
#define TDC7200_CONFIG2_AVG_CYCLES_128            (7 << 3)

#define TDC7200_CONFIG2_NUM_STOP_1                0
#define TDC7200_CONFIG2_NUM_STOP_2                1           /** 3 бита **/
#define TDC7200_CONFIG2_NUM_STOP_3                2
#define TDC7200_CONFIG2_NUM_STOP_4                3
#define TDC7200_CONFIG2_NUM_STOP_5                4

/* INT_STATUS */
#define TDC7200_INT_STATUS_MEAS_COMPLETE_FALSE        0
#define TDC7200_INT_STATUS_MEAS_COMPLETE_TRUE         (1 << 4)  

#define TDC7200_INT_STATUS_MEAS_STARTED_FALSE         0
#define TDC7200_INT_STATUS_MEAS_STARTED_TRUE          (1 << 3)
                                                      
#define TDC7200_INT_STATUS_CLOCK_CNTR_OVF_INT_FALSE   0
#define TDC7200_INT_STATUS_CLOCK_CNTR_OVF_INT_TRUE    (1 << 2)

#define TDC7200_INT_STATUS_COARSE_CNTR_OVF_INT_FALSE  0
#define TDC7200_INT_STATUS_COARSE_CNTR_OVF_INT_TRUE   (1 << 1)

#define TDC7200_INT_STATUS_NEW_MEAS_INT_FALSE         0
#define TDC7200_INT_STATUS_NEW_MEAS_INT_TRUE          1

/* INT_MASK */
#define TDC7200_INT_MASK_CLOCK_CNTR_OVF_MASK_DISABLED   0
#define TDC7200_INT_MASK_CLOCK_CNTR_OVF_MASK_ENABLED    (1 << 2)

#define TDC7200_INT_MASK_COARSE_CNTR_OVF_MASK_DISABLED  0
#define TDC7200_INT_MASK_COARSE_CNTR_OVF_MASK_ENABLED   (1 << 1)

#define TDC7200_INT_MASK_NEW_MEAS_MASK_DISABLED         0
#define TDC7200_INT_MASK_NEW_MEAS_MASK_ENABLED          1

/* COARSE & CLOCK COUNTER OVERFLOW DEFAULT VALUES */
#define TDC7200_COARSE_CNTR_OVF_H_0xFF                  0xFF
#define TDC7200_COARSE_CNTR_OVF_L_0xFF                  0xFF
#define TDC7200_CLOCK_CNTR_OVF_H_0xFF                   0xFF
#define TDC7200_CLOCK_CNTR_OVF_L_0xFF                   0xFF

/* CLOCK COUNTER STOP MASK */
#define TDC7200_CLOCK_CNTR_STOP_MASK_H_0x00             0
#define TDC7200_CLOCK_CNTR_STOP_MASK_L_0x00             0



/*! ------------ TDC7200 REGISTER ADRESSES -------------- */
#define TDC7200_CONFIG1_ADRESS                        0x00 
#define TDC7200_CONFIG2_ADRESS                        0x01 

#define TDC7200_INT_STATUS_ADRESS                     0x02 
#define TDC7200_INT_MASK_ADRESS                       0x03 

#define TDC7200_COARSE_CNTR_OVF_H_ADRESS              0x04 
#define TDC7200_COARSE_CNTR_OVF_L_ADRESS              0x05 

#define TDC7200_CLOCK_CNTR_OVF_H_ADRESS               0x06 
#define TDC7200_CLOCK_CNTR_OVF_L_ADRESS               0x07 

#define TDC7200_CLOCK_CNTR_STOP_MASK_H_ADRESS         0x08 
#define TDC7200_CLOCK_CNTR_STOP_MASK_L_ADRESS         0x09

#define TDC7200_TIME1_ADRESS                          0x10
#define TDC7200_CLOCK_COUNT1_ADRESS                   0x11
#define TDC7200_TIME2_ADRESS                          0x12
#define TDC7200_CLOCK_COUNT2_ADRESS                   0x13
#define TDC7200_TIME3_ADRESS                          0x14
#define TDC7200_CLOCK_COUNT3_ADRESS                   0x15
#define TDC7200_TIME4_ADRESS                          0x16
#define TDC7200_CLOCK_COUNT4_ADRESS                   0x17
#define TDC7200_TIME5_ADRESS                          0x18
#define TDC7200_CLOCK_COUNT5_ADRESS                   0x19
#define TDC7200_TIME6_ADRESS                          0x1A

#define TDC7200_CALIBRATION1_ADRESS                   0x1B
#define TDC7200_CALIBRATION2_ADRESS                   0x1C


#endif // TDC7200_REGISTERS_H
