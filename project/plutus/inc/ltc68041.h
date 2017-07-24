#pragma once

typedef enum {
  LTC_AFE_REGISTER_CONFIG = 0,
  LTC_AFE_REGISTER_CELL_VOLTAGE_A,
  LTC_AFE_REGISTER_CELL_VOLTAGE_B,
  LTC_AFE_REGISTER_CELL_VOLTAGE_C,
  LTC_AFE_REGISTER_CELL_VOLTAGE_D,
  LTC_AFE_REGISTER_AUX_A,
  LTC_AFE_REGISTER_AUX_B,
  LTC_AFE_REGISTER_STATUS_A,
  LTC_AFE_REGISTER_STATUS_B,
  LTC_AFE_REGISTER_COMM,
  NUM_LTC_AFE_REGISTER
} LtcAfeRegister;

typedef enum {
  LTC_AFE_VOLTAGE_REGISTER_A,
  LTC_AFE_VOLTAGE_REGISTER_B,
  LTC_AFE_VOLTAGE_REGISTER_C,
  LTC_AFE_VOLTAGE_REGISTER_D,
  NUM_LTC_AFE_VOLTAGE_REGISTER
} LtcAfeVoltageRegister;

// command codes
// see Table 34 (p.49)
#define LTC6804_WRCFG_RESERVED                  (1 << 0)

#define LTC6804_RDCFG_RESERVED                  (1 << 1)

#define LTC6804_RDCVA_RESERVED                  (1 << 2)

#define LTC6804_RDCVB_RESERVED                  (1 << 2) | (1 << 1)

#define LTC6804_RDCVC_RESERVED                  (1 << 3)

#define LTC6804_RDCVD_RESERVED                  (1 << 3) | (1 << 1)

#define LTC6804_RDAUXA_RESERVED                 ((1 << 3) | (1 << 2))

#define LTC6804_RDAUXB_RESERVED                 ((1 << 3) | (1 << 2)) | (1 << 1)

#define LTC6804_RDSTATA_RESERVED                (1 << 4)

#define LTC6804_RDSTATB_RESERVED                (1 << 4) | (1 << 1)

#define LTC6804_ADCV_RESERVED                   ((1 << 9) | (1 << 6) | (1 << 5))

#define LTC6804_ADCOW_RESERVED                  ((1 << 3) | (1 << 5) | (1 << 9))

#define LTC6804_CVST_RESERVED                   ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 9))

#define LTC6804_ADAX_RESERVED                   (1 << 10) | (1 << 6) | (1 << 5)

#define LTC6804_CLRCELL_RESERVED                (1 << 0) | (1 << 4) | (1 << 8) | (1 << 9) | (1 << 10)

#define LTC6804_CLRAUX_RESERVED                 (1 << 1) | (1 << 4) | (1 << 8) | (1 << 9) | (1 << 10)

#define LTC6804_CLRSTAT_RESERVED                (1 << 0) | (1 << 1) | (1 << 4) | (1 << 8) | (1 << 9) | (1 << 10)

#define LTC6804_PLADC_RESERVED                  (1 << 2) | (1 << 4) | (1 << 8) | (1 << 9) | (1 << 10)

#define LTC6804_DIAGNC_RESERVED                 (1 << 0) | (1 << 2) | (1 << 4) | (1 << 8) | (1 << 9) | (1 << 10)

#define LTC6804_WRCOMM_RESERVED                 (1 << 0) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 10)

#define LTC6804_RDCOMM_RESERVED                 (1 << 1) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 10)

#define LTC6804_STCOMM_RESERVED                 (1 << 0) | (1 << 1) | (1 << 5) | (1 << 8) | (1 << 9) | (1 << 10)


// command bits
// see Table 35 (p. 50)
#define LTC6804_GPIO1_PD_ON                     (0 << 3)
#define LTC6804_GPIO1_PD_OFF                    (1 << 3)
#define LTC6804_GPIO2_PD_ON                     (0 << 4)
#define LTC6804_GPIO2_PD_OFF                    (1 << 4)
#define LTC6804_GPIO3_PD_ON                     (0 << 5)
#define LTC6804_GPIO3_PD_OFF                    (1 << 5)
#define LTC6804_GPIO4_PD_ON                     (0 << 6)
#define LTC6804_GPIO4_PD_OFF                    (1 << 6)
#define LTC6804_GPIO5_PD_ON                     (0 << 7)
#define LTC6804_GPIO5_PD_OFF                    (1 << 7)

#define LTC6804_CNVT_CELL_ALL                   0x00
#define LTC6804_CNVT_CELL_1_7                   0x01
#define LTC6804_CNVT_CELL_2_8                   0x02
#define LTC6804_CNVT_CELL_3_9                   0x03
#define LTC6804_CNVT_CELL_4_10                  0x04
#define LTC6804_CNVT_CELL_5_11                  0x05
#define LTC6804_CNVT_CELL_6_12                  0x06

#define LTC6804_ADCV_DISCHARGE_PERMITTED        (1 << 4)

#define LTC6804_ADCOPT                          (1 << 0)

#define LTC6804_SWTRD                           (1 << 1)

#define LTC6804_ADAX_GPIO1                      0x01
#define LTC6804_ADAX_MODE_FAST                  (0 << 8) | (1 << 7)
