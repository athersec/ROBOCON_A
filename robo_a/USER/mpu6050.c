#include <mpu6050.h>

/*
	The average values when mpu6050 starts
*/
double axd, ayd, azd, wxd, wyd, wzd;

uint8_t mpu6050_read(uint8_t reg)
{
	return i2c1_read(SlaveAddress, reg);
}

signed int mpu6050_get_data(uint8_t reg)
{
	return ((uint16_t)mpu6050_read(reg) << 8) | mpu6050_read(reg + 1);
}

double mpu6050_get_exact_data(uint8_t reg)
{
	double result = int2double(mpu6050_get_data(reg));
	switch(reg)
	{
		case ACCEL_XOUT_H:
			result -= axd;
			break;
		
		case ACCEL_YOUT_H:
			result -= ayd;
			break;
		
		case ACCEL_ZOUT_H:
			result -= azd;
			break;
		
		case GYRO_XOUT_H:
			result -= wxd;
			break;
		
		case GYRO_YOUT_H:
			result -= wyd;
			break;
		
		case GYRO_ZOUT_H:
			result -= wzd;
			break;
		
		default:
			break;
	}
	return result;
}

void mpu6050_set_average_values(void)
{
	uint16_t i;
	
	axd = 0;
	ayd = 0;
	azd = 0;
	wxd = 0;
	wyd = 0;
	wzd = 0;
	
	#define MPU_SUM 250
	
	for(i = 0; i < MPU_SUM; i++)
	{
		axd += int2double(mpu6050_get_data(ACCEL_XOUT_H)) / MPU_SUM;
		ayd += int2double(mpu6050_get_data(ACCEL_YOUT_H)) / MPU_SUM;
		azd += int2double(mpu6050_get_data(ACCEL_ZOUT_H)) / MPU_SUM;
		wxd += int2double(mpu6050_get_data(GYRO_XOUT_H)) / MPU_SUM;
		wyd += int2double(mpu6050_get_data(GYRO_YOUT_H)) / MPU_SUM;
		wzd += int2double(mpu6050_get_data(GYRO_ZOUT_H)) / MPU_SUM;
	}
}

void 	mpu6050_config(void)
{
	i2c1_write(SlaveAddress, PWR_MGMT_1, 0x00);
	i2c1_write(SlaveAddress, SMPLRT_DIV, 0x07);
	i2c1_write(SlaveAddress, CONFIG, 0x06);
	i2c1_write(SlaveAddress, GYRO_CONFIG, 0x18);
	i2c1_write(SlaveAddress, ACCEL_CONFIG, ACCEL_CONFIG_CMD);
	
	mpu6050_set_average_values();
}


/*
	Functions used to test mpu6050
*/

void mpu6050_print_default(void)
{
	mpu6050_set_average_values();
	
	printf("\naxd = %lf \n", axd);
	printf("\nayd = %lf \n", ayd);
	printf("\nazd = %lf \n", azd);
	printf("\nwxd = %lf \n", wxd);
	printf("\nwyd = %lf \n", wyd);
	printf("\nwzd = %lf \n", wzd);
}
