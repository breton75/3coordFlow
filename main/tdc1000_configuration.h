#ifndef TDC1000_CONFIGURATION_H
#define TDC1000_CONFIGURATION_H

#include "TDC1000_registers.h"

/*! --------------- CONFIG_0 -------------- !*/
#define TDC1000_CONFIG_0 (TDC1000_CONFIG_0_TX_FREQ_DIV_8 | \
                          TDC1000_CONFIG_0_NUM_TX_5)


/*! --------------- CONFIG_1 -------------- !*/
#define TDC1000_CONFIG_1 (TDC1000_CONFIG_1_NUM_AVG_1     | \
                          TDC1000_CONFIG_1_NUM_RX_0)


/*! --------------- CONFIG_2 -------------- !*/
#define TDC1000_CONFIG_2 (TDC1000_CONFIG_2_VCOM_SEL_INTERNAL   | \
                          TDC1000_CONFIG_2_MEAS_MODE_TOF       | \
                          TDC1000_CONFIG_2_DAMPING_DISABLED    | \
                          TDC1000_CONFIG_2_CH_SWP_DISABLED     | \
                          TDC1000_CONFIG_2_EXT_CHSEL_ENABLED   | \
                          TDC1000_CONFIG_2_CH_SEL_1            | \
                          TDC1000_CONFIG_2_TOF_MEAS_MODE_2)


/*! --------------- CONFIG_3 -------------- !*/
#define TDC1000_CONFIG_3 (TDC1000_CONFIG_3_TEMP_MODE_REF_RTD1_RTD2 | \
                          TDC1000_CONFIG_3_TEMP_RDT_SEL_PT1000     | \
                          TDC1000_CONFIG_3_TEMP_CLK_DIV_8          | \
                          TDC1000_CONFIG_3_BLANKING_DISABLED       | \
                          TDC1000_CONFIG_3_ECHO_QUAL_THLD_125mV)


/*! --------------- CONFIG_4 -------------- !*/
#define TDC1000_CONFIG_4 (TDC1000_CONFIG_4_RECEIVE_MODE_SINGLE_ECHO  | \
                          TDC1000_CONFIG_4_TRIG_EDGE_POLARITY_RISING | \
                          TDC1000_CONFIG_4_TX_PH_SHIFT_POS_1F)


/*! --------------- TOF_1 -------------- !*/
#define TDC1000_TOF_1 (TDC1000_TOF_1_PGA_GAIN_0db       | \
                       TDC1000_TOF_1_PGA_CTRL_ACTIVE    | \
                       TDC1000_TOF_1_LNA_CTRL_ACTIVE    | \
                       TDC1000_TOF_1_LNA_FB_CAPACITIVE  | \
                       TDC1000_TOF_1_TIMING_REG_0 )


/*! --------------- TOF_0 -------------- !*/
#define TDC1000_TOF_0 TDC1000_TOF_0_TIMING_REG_0


/*! --------------- TIMEOUT -------------- !*/
#define TDC1000_TIMEOUT (TDC1000_TIMEOUT_FORCE_SHORT_TOF_DISABLED     | \
                         TDC1000_TIMEOUT_SHORT_TOF_BLANK_PERIOD_64xT0 | \
                         TDC1000_TIMEOUT_ECHO_TIMEOUT_ENABLED         | \
                         TDC1000_TIMEOUT_TOF_TIMEOUT_CTRL_256xT0)


/*! --------------- CLOCK_RATE -------------- !*/
#define TDC1000_CLOCK_RATE (TDC1000_CLOCK_RATE_CLOCKIN_DIV_1          | \
                            TDC1000_CLOCK_RATE_AUTOZERO_PERIOD_64xT0)

#endif // TDC1000_CONFIGURATION_H
