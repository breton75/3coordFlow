#ifndef HEADER_FILE_TI_TDC1000_REGISTER_SETTINGS_H
#define HEADER_FILE_TI_TDC1000_REGISTER_SETTINGS_H

/*! ------------ TDC1000 REGISTERS FIELD VALUES ------------- !*/

/* CONFIG_0 */
#define TDC1000_CONFIG_0_TX_FREQ_DIV_2          0
#define TDC1000_CONFIG_0_TX_FREQ_DIV_4          (1 << 5)  /** 3 бита **/  
#define TDC1000_CONFIG_0_TX_FREQ_DIV_8          (2 << 5)
#define TDC1000_CONFIG_0_TX_FREQ_DIV_16         (3 << 5)
#define TDC1000_CONFIG_0_TX_FREQ_DIV_32         (4 << 5)
#define TDC1000_CONFIG_0_TX_FREQ_DIV_64         (5 << 5)
#define TDC1000_CONFIG_0_TX_FREQ_DIV_128        (6 << 5)
#define TDC1000_CONFIG_0_TX_FREQ_DIV_256        (7 << 5)

#define TDC1000_CONFIG_0_NUM_TX_5               5         /** 0 - 31 **/

/* CONFIG_1 */
#define TDC1000_CONFIG_1_NUM_AVG_1              0
#define TDC1000_CONFIG_1_NUM_AVG_2              (1 << 3)  /** 3 бита **/  
#define TDC1000_CONFIG_1_NUM_AVG_4              (2 << 3)
#define TDC1000_CONFIG_1_NUM_AVG_8              (3 << 3)
#define TDC1000_CONFIG_1_NUM_AVG_16             (4 << 3)
#define TDC1000_CONFIG_1_NUM_AVG_32             (5 << 3)
#define TDC1000_CONFIG_1_NUM_AVG_64             (6 << 3)
#define TDC1000_CONFIG_1_NUM_AVG_128            (7 << 3)

#define TDC1000_CONFIG_1_NUM_RX_0               0
#define TDC1000_CONFIG_1_NUM_RX_1               1 /** 3 бита **/  
#define TDC1000_CONFIG_1_NUM_RX_2               2
#define TDC1000_CONFIG_1_NUM_RX_3               3
#define TDC1000_CONFIG_1_NUM_RX_4               4
#define TDC1000_CONFIG_1_NUM_RX_5               5
#define TDC1000_CONFIG_1_NUM_RX_6               6
#define TDC1000_CONFIG_1_NUM_RX_7               7

/* CONFIG_2 */
#define TDC1000_CONFIG_2_VCOM_SEL_INTERNAL      0
#define TDC1000_CONFIG_2_VCOM_SEL_EXTERNAL      (1 << 7)

#define TDC1000_CONFIG_2_MEAS_MODE_TOF          0
#define TDC1000_CONFIG_2_MEAS_MODE_TEMPERATURE  (1 << 6)

#define TDC1000_CONFIG_2_DAMPING_DISABLED       0
#define TDC1000_CONFIG_2_DAMPING_ENABLED        (1 << 5)

#define TDC1000_CONFIG_2_CH_SWP_DISABLED        0
#define TDC1000_CONFIG_2_CH_SWP_ENABLED         (1 << 4)

#define TDC1000_CONFIG_2_EXT_CHSEL_DISABLED     0
#define TDC1000_CONFIG_2_EXT_CHSEL_ENABLED      (1 << 3)

#define TDC1000_CONFIG_2_CH_SEL_1               0
#define TDC1000_CONFIG_2_CH_SEL_2               (1 << 2)

#define TDC1000_CONFIG_2_TOF_MEAS_MODE_0        0
#define TDC1000_CONFIG_2_TOF_MEAS_MODE_1        1
#define TDC1000_CONFIG_2_TOF_MEAS_MODE_2        2

/* CONFIG_3 */
#define TDC1000_CONFIG_3_TEMP_MODE_REF_RTD1_RTD2        0
#define TDC1000_CONFIG_3_TEMP_MODE_REF_RTD1             (1 << 6)
                                                        
#define TDC1000_CONFIG_3_TEMP_RDT_SEL_PT1000            0
#define TDC1000_CONFIG_3_TEMP_RDT_SEL_PT500             (1 << 5)
                                                        
#define TDC1000_CONFIG_3_TEMP_CLK_DIV_8                 0
#define TDC1000_CONFIG_3_TEMP_CLK_DIV_TX_FREQ_DIV       (1 << 4)
                                                        
#define TDC1000_CONFIG_3_BLANKING_DISABLED              0
#define TDC1000_CONFIG_3_BLANKING_ENABLED               (1 << 3)
                                                        
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_35mV            0
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_50mV            1
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_75mV            2
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_125mV           3
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_220mV           4
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_410mV           5
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_775mV           6
#define TDC1000_CONFIG_3_ECHO_QUAL_THLD_1500mV          7

/* CONFIG_4 */
#define TDC1000_CONFIG_4_RECEIVE_MODE_SINGLE_ECHO       0
#define TDC1000_CONFIG_4_RECEIVE_MODE_MULTI_ECHO        (1 << 6)
                                                        
#define TDC1000_CONFIG_4_TRIG_EDGE_POLARITY_RISING      0
#define TDC1000_CONFIG_4_TRIG_EDGE_POLARITY_FALLING     (1 << 5)
                                                        
#define TDC1000_CONFIG_4_TX_PH_SHIFT_POS_1F             0x1F          /** 0 - 31 **/

/* TOF_1 */
#define TDC1000_TOF_1_PGA_GAIN_0db                      0
#define TDC1000_TOF_1_PGA_GAIN_3db                      (1 << 5)
#define TDC1000_TOF_1_PGA_GAIN_6db                      (2 << 5)
#define TDC1000_TOF_1_PGA_GAIN_9db                      (3 << 5)
#define TDC1000_TOF_1_PGA_GAIN_12db                     (4 << 5)
#define TDC1000_TOF_1_PGA_GAIN_15db                     (5 << 5)
#define TDC1000_TOF_1_PGA_GAIN_18db                     (6 << 5)
#define TDC1000_TOF_1_PGA_GAIN_21db                     (7 << 5)
                                                        
#define TDC1000_TOF_1_PGA_CTRL_ACTIVE                   0
#define TDC1000_TOF_1_PGA_CTRL_OFF                      (1 << 4)
                                                        
#define TDC1000_TOF_1_LNA_CTRL_ACTIVE                   0
#define TDC1000_TOF_1_LNA_CTRL_OFF                      (1 << 3)
                                                        
#define TDC1000_TOF_1_LNA_FB_CAPACITIVE                 0
#define TDC1000_TOF_1_LNA_FB_RESISTIVE                  (1 << 2)
                                                        
#define TDC1000_TOF_1_TIMING_REG_0                      0
                                                        
/* TOF_0 */                                             
#define TDC1000_TOF_0_TIMING_REG_0                      0

/* TIMEOUT */
#define TDC1000_TIMEOUT_FORCE_SHORT_TOF_DISABLED         0
#define TDC1000_TIMEOUT_FORCE_SHORT_TOF_ENABLED          (1 << 6)

#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_8xT0     0
#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_16xT0    (1 << 3)
#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_32xT0    (2 << 3)
#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_64xT0    (3 << 3)
#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_128xT0   (4 << 3)
#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_256xT0   (5 << 3)
#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_512xT0   (6 << 3)
#define TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_1024xT0  (7 << 3)

#define TDC1000_TIMEOUT_ECHO_TIMEOUT_ENABLED            0
#define TDC1000_TIMEOUT_ECHO_TIMEOUT_DISABLED           (1 << 2)

#define TDC1000_TIMEOUT_TOF_TIMEOUT_CTRL_128xT0         0
#define TDC1000_TIMEOUT_TOF_TIMEOUT_CTRL_256xT0         1
#define TDC1000_TIMEOUT_TOF_TIMEOUT_CTRL_512xT0         2
#define TDC1000_TIMEOUT_TOF_TIMEOUT_CTRL_1024xT0        3

/* CLOCK_RATE */
#define TDC1000_CLOCK_RATE_CLOCKIN_DIV_1                0
#define TDC1000_CLOCK_RATE_CLOCKIN_DIV_2                (1 << 2)

#define TDC1000_CLOCK_RATE_AUTOZERO_PERIOD_64xT0        0
#define TDC1000_CLOCK_RATE_AUTOZERO_PERIOD_128xT0       1
#define TDC1000_CLOCK_RATE_AUTOZERO_PERIOD_256xT0       2
#define TDC1000_CLOCK_RATE_AUTOZERO_PERIOD_512xT0       3


/*! ------------ TDC1000 REGISTER ADDRESSES -------------- */
#define TDC1000_CONFIG_0_ADRESS      0x0 
#define TDC1000_CONFIG_1_ADRESS      0x1 
#define TDC1000_CONFIG_2_ADRESS      0x2 
#define TDC1000_CONFIG_3_ADRESS      0x3 
#define TDC1000_CONFIG_4_ADRESS      0x4 
#define TDC1000_TOF_1_ADRESS         0x5 
#define TDC1000_TOF_0_ADRESS         0x6 
#define TDC1000_ERROR_FLAGS_ADRESS   0x7 
#define TDC1000_TIMEOUT_ADRESS       0x8 
#define TDC1000_CLOCK_RATE_ADRESS    0x9    


#endif
