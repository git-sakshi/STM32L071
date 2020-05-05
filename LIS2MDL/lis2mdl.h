#include <string.h>
#include <math.h>
#include <stdint.h>
#include "hw.h"
#include "stm32l0xx_hal_def.h"

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define LIS2MDL_Write_Address 		0x003C
#define LIS2MDL_Read_Address 		0x003D	 
/*=========================================================================*/

/* LIS2MDL REGISTERS ADDRESS */
#define LIS2MDL_OFFSET_X_REG_L          	0x45U
#define LIS2MDL_OFFSET_X_REG_H          	0x46U
#define LIS2MDL_OFFSET_Y_REG_L          	0x47U
#define LIS2MDL_OFFSET_Y_REG_H          	0x48U
#define LIS2MDL_OFFSET_Z_REG_L          	0x49U
#define LIS2MDL_OFFSET_Z_REG_H          	0x4AU
#define LIS2MDL_WHO_AM_I                	0x4FU
#define LIS2MDL_CFG_REG_A               	0x60U
#define LIS2MDL_CFG_REG_B               	0x61U
#define LIS2MDL_CFG_REG_C               	0x62U
#define LIS2MDL_INT_CRTL_REG            	0x63U
#define LIS2MDL_INT_SOURCE_REG          	0x64U
#define LIS2MDL_INT_THS_L_REG           	0x65U
#define LIS2MDL_INT_THS_H_REG           	0x66U
#define LIS2MDL_STATUS_REG              	0x67U
#define LIS2MDL_OUTX_L_REG              	0x68U
#define LIS2MDL_OUTX_H_REG              	0x69U
#define LIS2MDL_OUTY_L_REG              	0x6AU
#define LIS2MDL_OUTY_H_REG              	0x6BU
#define LIS2MDL_OUTZ_L_REG              	0x6CU
#define LIS2MDL_OUTZ_H_REG              	0x6DU
#define LIS2MDL_TEMP_OUT_L_REG          	0x6EU
#define LIS2MDL_TEMP_OUT_H_REG          	0x6FU

#define pi 									3.14159265359

/* CFG REG_A */
/* lis2mdl_operating_mode_set */
typedef enum {
  LIS2MDL_CONTINUOUS_MODE  		 = 0,
  LIS2MDL_SINGLE_TRIGGER   		 = 1,
  LIS2MDL_POWER_DOWN       		 = 2,
} lis2mdl_md_t;

/* lis2mdl_data_rate_set */
typedef enum {
  LIS2MDL_ODR_10Hz   			 = 0,
  LIS2MDL_ODR_20Hz   			 = 1,
  LIS2MDL_ODR_50Hz   			 = 2,
  LIS2MDL_ODR_100Hz  			 = 3,
} lis2mdl_odr_t;

/* lis2mdl_power_mode_set */
typedef enum {
  LIS2MDL_HIGH_RESOLUTION  		 = 0,
  LIS2MDL_LOW_POWER        		 = 1,
} lis2mdl_lp_t;

typedef struct {
  uint8_t md                     : 2;
  uint8_t odr                    : 2;
  uint8_t lp                     : 1;
  uint8_t soft_rst               : 1;
  uint8_t reboot                 : 1;
  uint8_t comp_temp_en           : 1;
} lis2mdl_cfg_reg_a_t;

/* CFG REG_B */
/* lis2mdl_low_pass_bandwidth_set_reset */
typedef enum {
  LIS2MDL_ODR_DIV_2  			= 0,
  LIS2MDL_ODR_DIV_4  			= 1,
} lis2mdl_lpf_t;

typedef enum {
  LIS2MDL_SET_SENS_ODR_DIV_63        = 0,
  LIS2MDL_SENS_OFF_CANC_EVERY_ODR    = 1,
  LIS2MDL_SET_SENS_ONLY_AT_POWER_ON  = 2,
} lis2mdl_set_rst_t;

typedef struct {
  uint8_t lpf                    : 1;
  uint8_t set_rst                : 2; /* OFF_CANC + Set_FREQ */
  uint8_t int_on_dataoff         : 1;
  uint8_t off_canc_one_shot      : 1;
  uint8_t not_used_01            : 3;
} lis2mdl_cfg_reg_b_t;

/* CFG REG_C */
/* Big Little Endian Selection */
typedef enum {
  LIS2MDL_LSB_AT_LOW_ADD  		 = 0,
  LIS2MDL_MSB_AT_LOW_ADD  		 = 1,
} lis2mdl_ble_t;

typedef enum {
  LIS2MDL_I2C_ENABLE   			 = 0,
  LIS2MDL_I2C_DISABLE  			 = 1,
} lis2mdl_i2c_dis_t;

/* lis2mdl_i2c_interface_set */
typedef struct {
  uint8_t drdy_on_pin            : 1;
  uint8_t self_test              : 1;
  uint8_t not_used_01            : 1;
  uint8_t ble                    : 1;
  uint8_t bdu                    : 1;
  uint8_t i2c_dis                : 1;
  uint8_t int_on_pin             : 1;
  uint8_t not_used_02            : 1;
} lis2mdl_cfg_reg_c_t;

/* CFG LIS2MDL_INT_CRTL_REG */
typedef struct {
  uint8_t ien                    : 1;
  uint8_t iel                    : 1;
  uint8_t iea                    : 1;
  uint8_t not_used_01            : 2;
  uint8_t zien                   : 1;
  uint8_t yien                   : 1;
  uint8_t xien                   : 1;
} lis2mdl_int_crtl_reg_t;

/* LIS2MDL_INT_SOURCE_REG */
typedef struct {
  uint8_t _int                   : 1;
  uint8_t mroi                   : 1;
  uint8_t n_th_s_z               : 1;
  uint8_t n_th_s_y               : 1;
  uint8_t n_th_s_x               : 1;
  uint8_t p_th_s_z               : 1;
  uint8_t p_th_s_y               : 1;
  uint8_t p_th_s_x               : 1;
} lis2mdl_int_source_reg_t;

typedef struct{
  uint8_t bit0       			: 1;
  uint8_t bit1       			: 1;
  uint8_t bit2       			: 1;
  uint8_t bit3       			: 1;
  uint8_t bit4       			: 1;
  uint8_t bit5       			: 1;
  uint8_t bit6       			: 1;
  uint8_t bit7       			: 1;
} bitwise_t;

typedef union{
  lis2mdl_cfg_reg_a_t            cfg_reg_a;
  lis2mdl_cfg_reg_b_t            cfg_reg_b;
  lis2mdl_cfg_reg_c_t            cfg_reg_c;
  lis2mdl_int_crtl_reg_t         int_crtl_reg;
  lis2mdl_int_source_reg_t       int_source_reg;
  bitwise_t                      bitwise;
  uint8_t                        lis2mdl_byte;
}u_lis2mdl_reg_t;

/* User defined functions */
void config_lis2mdl_reg_a_softreset(void);
void config_lis2mdl_reg_a_reboot(void);
void config_lis2mdl_reg_a_odr(void);
void config_lis2mdl_reg_b(void);
void config_lis2mdl_reg_c(void);

void init_lis2mdl_magneto(void);
void magneto_read_data(void);
void magneto_data_processing(void);
void magneto_gauss_value(void);
int	magneto_compass_heading(void);
int magneto_angle_fcn(void);
void magneto_sign_decimal_value(void);
