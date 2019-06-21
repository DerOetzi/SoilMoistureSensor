#include "bme280.h"

#include <math.h>
#include <Wire.h>

BME280::BME280(void) {
  _i2caddr = BME280_ADDRESS;

  _temp_offset  = 0.0;
  _temp_offset_fine = 0.0;
  _temperature = 0.0;
  _humidity = 0.0;
  _pressure = 0.0;
}

void BME280::setTempOffset(float offset) {
  _temp_offset = offset;
  _temp_offset_fine = ((int32_t) (offset * 100)) << 8;
  _temp_offset_fine = (_temp_offset_fine - 128) / 5;
}

void BME280::readSensor(void) {
  _readTemperature();
  _readHumidity();
  _readPressure();
}

float BME280::getTemperature(void) {
  return _temperature_adjusted;
}

float BME280::getHumidity(void) {
  return _humidity;
}

float BME280::getPressure(void) {
  return _pressure;
}

bool BME280::begin() {
  Wire.begin(0, 2);
  if (_read8(BME280_REGISTER_CHIPID) != 0x60) {
    return false;
  }

  _readSensorCalibrationData();

  _write8(BME280_REGISTER_CONTROLHUMID, 0x01);
  _write8(BME280_REGISTER_CONFIG, 0xA0);
  _write8(BME280_REGISTER_CONTROL, 0x27); //0x3F

  return true;
}

void BME280::_readTemperature(void) {
  int32_t var1, var2;

  int32_t adc_T = _read24(BME280_REGISTER_TEMPDATA);

  adc_T >>= 4;

  var1  = ((((adc_T >> 3) - ((int32_t)_cal_data.dig_T1 << 1))) *
           ((int32_t)_cal_data.dig_T2)) >> 11;

  var2  = (((((adc_T >> 4) - ((int32_t)_cal_data.dig_T1)) *
             ((adc_T >> 4) - ((int32_t)_cal_data.dig_T1))) >> 12) *
           ((int32_t)_cal_data.dig_T3)) >> 14;

  _t_fine = var1 + var2;
  _temperature  = (_t_fine * 5 + 128) >> 8;
  _temperature = _temperature / 100;

  _temperature_adjusted = _temperature + _temp_offset;
}


void BME280::_readPressure(void) {
  int64_t var1, var2, p;

  int32_t adc_P = _read24(BME280_REGISTER_PRESSUREDATA);

  adc_P >>= 4;

  var1 = ((int64_t)_t_fine) - 128000;

  var2 = var1 * var1 * (int64_t)_cal_data.dig_P6;
  var2 += ((var1 * (int64_t)_cal_data.dig_P5) << 17);
  var2 += (((int64_t)_cal_data.dig_P4) << 35);

  var1 = ((var1 * var1 * (int64_t)_cal_data.dig_P3) >> 8) +
         ((var1 * (int64_t)_cal_data.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_cal_data.dig_P1) >> 33;

  if (var1 == 0) {
    _pressure = 0.0;
  }

  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;

  var1 = (((int64_t)_cal_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;

  var2 = (((int64_t)_cal_data.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_cal_data.dig_P7) << 4);

  _pressure = (float)p / 256;
  _pressure /= 100.0F;
}


void BME280::_readHumidity(void) {
  int32_t adc_H = _read16(BME280_REGISTER_HUMIDDATA);

  int32_t v_x1_u32r;

  v_x1_u32r = (_t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)_cal_data.dig_H4) << 20) -
                  (((int32_t)_cal_data.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
               (((((((v_x1_u32r * ((int32_t)_cal_data.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)_cal_data.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                  ((int32_t)2097152)) * ((int32_t)_cal_data.dig_H2) + 8192) >> 14));


  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)_cal_data.dig_H1)) >> 4));


  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;

  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;

  float h = (v_x1_u32r >> 12);

  _humidity = h / 1024.0;
  
  float temp_exp1 = (float) exp((17.67 * _temperature) / (_temperature + 243.5));
  float temp_exp2 = (float) exp((17.67 * _temperature_adjusted) / (_temperature_adjusted + 243.5));
  _humidity = (temp_exp1 * (273.15 + _temperature_adjusted) * _humidity) / (temp_exp2 * (273.15 + _temperature));
}

void BME280::_readSensorCalibrationData(void) {
  _cal_data.dig_T1 = _read16_LE(BME280_DIG_T1_REG);
  _cal_data.dig_T2 = _readS16_LE(BME280_DIG_T2_REG);
  _cal_data.dig_T3 = _readS16_LE(BME280_DIG_T3_REG);

  _cal_data.dig_P1 = _read16_LE(BME280_DIG_P1_REG);
  _cal_data.dig_P2 = _readS16_LE(BME280_DIG_P2_REG);
  _cal_data.dig_P3 = _readS16_LE(BME280_DIG_P3_REG);
  _cal_data.dig_P4 = _readS16_LE(BME280_DIG_P4_REG);
  _cal_data.dig_P5 = _readS16_LE(BME280_DIG_P5_REG);
  _cal_data.dig_P6 = _readS16_LE(BME280_DIG_P6_REG);
  _cal_data.dig_P7 = _readS16_LE(BME280_DIG_P7_REG);
  _cal_data.dig_P8 = _readS16_LE(BME280_DIG_P8_REG);
  _cal_data.dig_P9 = _readS16_LE(BME280_DIG_P9_REG);

  _cal_data.dig_H1 = _read8(BME280_DIG_H1_REG);
  _cal_data.dig_H2 = _readS16_LE(BME280_DIG_H2_REG);
  _cal_data.dig_H3 = _read8(BME280_DIG_H3_REG);
  _cal_data.dig_H4 = (_read8(BME280_DIG_H4_REG) << 4) | (_read8(BME280_DIG_H4_REG + 1) & 0xF);
  _cal_data.dig_H5 = (_read8(BME280_DIG_H5_REG + 1) << 4) | (_read8(BME280_DIG_H5_REG) >> 4);
  _cal_data.dig_H6 = (int8_t)_read8(BME280_DIG_H6_REG);
}

void BME280::_write8(byte reg, byte value) {
  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)value);
  Wire.endTransmission();
}

uint8_t BME280::_read8(byte reg) {
  uint8_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)_i2caddr, (byte)1);
  value = Wire.read();

  return value;
}

int16_t BME280::_readS16(byte reg) {
  return (int16_t)_read16(reg);
}

int16_t BME280::_readS16_LE(byte reg) {
  return (int16_t)_read16_LE(reg);
}

uint16_t BME280::_read16(byte reg) {
  uint16_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)_i2caddr, (byte)2);
  value = (Wire.read() << 8) | Wire.read();

  return value;
}

uint16_t BME280::_read16_LE(byte reg) {
  uint16_t temp = _read16(reg);

  return (temp >> 8) | (temp << 8);
}

uint32_t BME280::_read24(byte reg) {
  uint32_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)_i2caddr, (byte)3);
  value = Wire.read();
  value <<= 8;
  value |= Wire.read();
  value <<= 8;
  value |= Wire.read();

  return value;
}
