#ifndef TDC7200_CONFIGURATION_H
#define TDC7200_CONFIGURATION_H

#include "TDC7200_registers.h"

/*! --------------- CONFIG1 -------------- !*/
#define TDC7200_CONFIG1 (TDC7200_CONFIG1_FORCE_CALIBRATION_ALWAYS  | \
                        TDC7200_CONFIG1_PARITY_DISABLED           | \
                        TDC7200_CONFIG1_TRIGG_EDGE_RISING         | \
                        TDC7200_CONFIG1_STOP_EDGE_RISING          | \
                        TDC7200_CONFIG1_START_EDGE_RISING         | \
                        TDC7200_CONFIG1_MEAS_MODE_2               | \
                        TDC7200_CONFIG1_START_MEAS_NO)       


/*! --------------- CONFIG2 -------------- !*/
#define TDC7200_CONFIG2 (TDC7200_CONFIG2_CALIBRATION2_PERIODS_10 | \
                        TDC7200_CONFIG2_AVG_CYCLES_1             | \
                        TDC7200_CONFIG2_NUM_STOP_1)


/*! --------------- INT_STATUS -------------- !*/
#define TDC7200_INT_STATUS 0


/*! --------------- INT_MASK -------------- !*/
#define TDC7200_INT_MASK (TDC7200_INT_MASK_CLOCK_CNTR_OVF_MASK_ENABLED    | \
                          TDC7200_INT_MASK_COARSE_CNTR_OVF_MASK_ENABLED   | \
                          TDC7200_INT_MASK_NEW_MEAS_MASK_ENABLED)


/*! --------------- COARSE_CNTR_OVF_H -------------- !*/
#define TDC7200_COARSE_CNTR_OVF_H 0xFF

/*! --------------- COARSE_CNTR_OVF_L -------------- !*/
#define TDC7200_COARSE_CNTR_OVF_H 0xFF

/*! --------------- CLOCK_CNTR_OVF_H -------------- !*/
#define TDC7200_CLOCK_CNTR_OVF_H 0xFF

/*! --------------- CLOCK_CNTR_STOP_MASK_H -------------- !*/
#define TDC7200_CLOCK_CNTR_STOP_MASK_H 0x0

/*! --------------- CLOCK_CNTR_STOP_MASK_H -------------- !*/
#define TDC7200_CLOCK_CNTR_STOP_MASK_L 0x0


///*! ---------------  -------------- !*/
//#define TDC7200_



#endif // TDC7200_CONFIGURATION_H
