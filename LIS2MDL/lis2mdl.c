#include "lis2mdl.h"

u_lis2mdl_reg_t lis2mdl_reg_t;
extern I2C_HandleTypeDef hi2c1;

uint8_t status_reg_magneto,
		mag_xl,	mag_xh,
		mag_yl, mag_yh,
		mag_zl, mag_zh,
		kx_tscp, kx_pos;
		
int16_t mag_outx,
		mag_outy,
		mag_outz,
		mag_angle,
		mag_x, mag_y, mag_z,
		mag_xsign, mag_ysign,
		mag_zsign,angle,temp_angle;
		
double  mag_xgauss,
		mag_ygauss,
		mag_zgauss,
		plane1, plane2,
		direction;

void config_lis2mdl_reg_a_softreset(void)
{
	lis2mdl_reg_t.lis2mdl_byte				= 0x03;				/* Default */
	lis2mdl_reg_t.cfg_reg_a.md          	= 0;
//	lis2mdl_reg_t.cfg_reg_a.odr         	= 0;
//	lis2mdl_reg_t.cfg_reg_a.lp          	= 0;
	lis2mdl_reg_t.cfg_reg_a.soft_rst    	= 1;
//	lis2mdl_reg_t.cfg_reg_a.reboot      	= 0;
//	lis2mdl_reg_t.cfg_reg_a.comp_temp_en	= 0;
}

void config_lis2mdl_reg_a_reboot(void)
{
	lis2mdl_reg_t.lis2mdl_byte				= 0x03;				/* Default */
	lis2mdl_reg_t.cfg_reg_a.md          	= 0;
//	lis2mdl_reg_t.cfg_reg_a.odr         	= 0;
//	lis2mdl_reg_t.cfg_reg_a.lp          	= 0;
//	lis2mdl_reg_t.cfg_reg_a.soft_rst    	= 0;
	lis2mdl_reg_t.cfg_reg_a.reboot      	= 1;
//	lis2mdl_reg_t.cfg_reg_a.comp_temp_en	= 0;
}

void config_lis2mdl_reg_a_odr(void)
{
	lis2mdl_reg_t.lis2mdl_byte				= 0x03;				/* Default */
	lis2mdl_reg_t.cfg_reg_a.md          	= LIS2MDL_HIGH_RESOLUTION;
	lis2mdl_reg_t.cfg_reg_a.odr         	= LIS2MDL_ODR_100Hz;
//	lis2mdl_reg_t.cfg_reg_a.lp          	= 0;
//	lis2mdl_reg_t.cfg_reg_a.soft_rst    	= 0;
//	lis2mdl_reg_t.cfg_reg_a.reboot      	= 0;
	lis2mdl_reg_t.cfg_reg_a.comp_temp_en	= 1;
}

void config_lis2mdl_reg_b(void)
{	
	lis2mdl_reg_t.lis2mdl_byte				  = 0x00;			/* Default */
//	lis2mdl_reg_t.cfg_reg_b.lpf               = 0;
	lis2mdl_reg_t.cfg_reg_b.set_rst           = LIS2MDL_SENS_OFF_CANC_EVERY_ODR;	/* OFF_CANC + Set_FREQ */
//	lis2mdl_reg_t.cfg_reg_b.int_on_dataoff    = 0;
//	lis2mdl_reg_t.cfg_reg_b.off_canc_one_shot = 0;
}

void config_lis2mdl_reg_c(void)
{
	lis2mdl_reg_t.lis2mdl_byte				 = 0x00;			/* Default */
//	lis2mdl_reg_t.cfg_reg_c.drdy_on_pin		 = 0;
//	lis2mdl_reg_t.cfg_reg_c.self_test  		 = 0;
//	lis2mdl_reg_t.cfg_reg_c.not_used_01		 = 0;
//	lis2mdl_reg_t.cfg_reg_c.ble        		 = 0;
//	lis2mdl_reg_t.cfg_reg_c.bdu        		 = 0;
//	lis2mdl_reg_t.cfg_reg_c.i2c_dis    		 = 0;
//	lis2mdl_reg_t.cfg_reg_c.int_on_pin 		 = 0;
//	lis2mdl_reg_t.cfg_reg_c.not_used_02		 = 0;
}

void init_lis2mdl_magneto(void)
{
//	HAL_Delay(500);
	config_lis2mdl_reg_a_softreset();
	HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_Write_Address, LIS2MDL_CFG_REG_A, I2C_MEMADD_SIZE_8BIT, &lis2mdl_reg_t.lis2mdl_byte, 1, 100);
	HAL_Delay(5);
	config_lis2mdl_reg_a_reboot();
	HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_Write_Address, LIS2MDL_CFG_REG_A, I2C_MEMADD_SIZE_8BIT, &lis2mdl_reg_t.lis2mdl_byte, 1, 100);
	HAL_Delay(20);
	config_lis2mdl_reg_a_odr();
	HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_Write_Address, LIS2MDL_CFG_REG_A, I2C_MEMADD_SIZE_8BIT, &lis2mdl_reg_t.lis2mdl_byte, 1, 100);
	HAL_Delay(100);
	config_lis2mdl_reg_b();
	HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_Write_Address, LIS2MDL_CFG_REG_B, I2C_MEMADD_SIZE_8BIT, &lis2mdl_reg_t.lis2mdl_byte, 1, 100);
	HAL_Delay(100);
	config_lis2mdl_reg_c();
	HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_Write_Address, LIS2MDL_CFG_REG_C, I2C_MEMADD_SIZE_8BIT, &lis2mdl_reg_t.lis2mdl_byte, 1, 100);
} 

void magneto_read_data(void)
{
	//Delay required to read values from magneto_registers
	//HAL_Delay(20);
	/* The status register is an 8-bit read-only register. This register is used to indicate device status. */
	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_Read_Address, LIS2MDL_STATUS_REG, 1, &status_reg_magneto, 1, 100);
	/* The data output X registers are two 8-bit registers, data output X MSB register (69h) and output X LSB register (68h). */
	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_Read_Address, LIS2MDL_OUTX_L_REG, 1, &mag_xl, 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_Read_Address, LIS2MDL_OUTX_H_REG, 1, &mag_xh, 1, 100);
	/* The data output Y registers are two 8-bit registers, data output Y MSB register (6Bh) and output Y LSB register (6Ah) */
	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_Read_Address, LIS2MDL_OUTY_L_REG, 1, &mag_yl, 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_Read_Address, LIS2MDL_OUTY_H_REG, 1, &mag_yh, 1, 100);
	/* The data output Z registers are two 8-bit registers, data output Z MSB register (6Bh) and output Z LSB register (6Ah). */
	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_Read_Address, LIS2MDL_OUTZ_L_REG, 1, &mag_zl, 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_Read_Address, LIS2MDL_OUTZ_H_REG, 1, &mag_zh, 1, 100);
}

/* Read Magnetometer X,Y,Z Axis Raw Data */
void magneto_data_processing(void)
{
	mag_outx = (mag_xh << 8) | mag_xl;
	mag_outy = (mag_yh << 8) | mag_yl;
	mag_outz = (mag_zh << 8) | mag_zl;
}

/* Calculate 2's Complement of Magnetometer Data*/
void magneto_sign_decimal_value(void)
{
	mag_x = mag_outx - 0xFFFF;
	mag_y = mag_outy - 0xFFFF;
	mag_z = mag_outz - 0xFFFF;	
	
	mag_xsign = mag_x - 1;	
	mag_ysign = mag_y - 1;	
	mag_zsign = mag_z - 1;
}

/* Multiply by 1.5 to calculate sensitivity expressed in miliGauss*/
void magneto_gauss_value(void)
{
	mag_xgauss = mag_xsign * 1.5;
	mag_ygauss = mag_ysign * 1.5;
	mag_zgauss = mag_zsign * 1.5;
}

/* Magnetometer Angle Calculation */
int magneto_compass_heading(void)
{	
	plane1 = mag_xgauss;
	plane2 = mag_ygauss;
	if( plane1 == 0 )
	{
		if( plane2 < 0 )		direction = 90;
		else 								direction = 0;
	}
	else if ( plane1 != 0 )
	{
		direction = atan2( plane2,plane1 )*180/pi;
		{
			if( direction > 360 )		direction -= 360;
			if( direction < 0 )			direction += 360;
		}
	}
	else direction = direction;
	
	temp_angle = direction;
	//direction = 360-direction;	
	/* Convert angle according to true north */
	angle = 360-( temp_angle + 270 );

	return angle;
}

/* Magnetometer functions to calculate angle */
int magneto_angle_fcn(void)
{
	magneto_read_data();
	magneto_data_processing();
	magneto_sign_decimal_value();
	magneto_gauss_value();
	mag_angle =	magneto_compass_heading();
	return mag_angle;
}

