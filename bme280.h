#ifndef __BME280_H__

#define __BME280_H__

#include "Arduino.h"

#define BME280_ADDRESS 0x76

#define    BME280_DIG_T1_REG   0x88
#define    BME280_DIG_T2_REG   0x8A
#define    BME280_DIG_T3_REG   0x8C

#define    BME280_DIG_P1_REG   0x8E
#define    BME280_DIG_P2_REG   0x90
#define    BME280_DIG_P3_REG   0x92
#define    BME280_DIG_P4_REG   0x94
#define    BME280_DIG_P5_REG   0x96
#define    BME280_DIG_P6_REG   0x98
#define    BME280_DIG_P7_REG   0x9A
#define    BME280_DIG_P8_REG   0x9C
#define    BME280_DIG_P9_REG   0x9E    

#define    BME280_DIG_H1_REG   0xA1
#define    BME280_DIG_H2_REG   0xE1
#define    BME280_DIG_H3_REG   0xE3
#define    BME280_DIG_H4_REG   0xE4
#define    BME280_DIG_H5_REG   0xE5
#define    BME280_DIG_H6_REG   0xE7
    
#define    BME280_REGISTER_CHIPID       0xD0
#define    BME280_REGISTER_VERSION      0xD1
#define    BME280_REGISTER_SOFTRESET    0xE0
#define    BME280_REGISTER_CAL26        0xE1
#define    BME280_REGISTER_CONTROLHUMID     0xF2
#define    BME280_REGISTER_CONTROL          0xF4
#define    BME280_REGISTER_CONFIG           0xF5
#define    BME280_REGISTER_PRESSUREDATA     0xF7
#define    BME280_REGISTER_TEMPDATA         0xFA
#define    BME280_REGISTER_HUMIDDATA        0xFD

struct BME280_Calibration_Data {
    public:
        uint16_t dig_T1;
        int16_t  dig_T2;
        int16_t  dig_T3;
    
        uint16_t dig_P1;
        int16_t  dig_P2;
        int16_t  dig_P3;
        int16_t  dig_P4;
        int16_t  dig_P5;
        int16_t  dig_P6;
        int16_t  dig_P7;
        int16_t  dig_P8;
        int16_t  dig_P9;
    
        uint8_t  dig_H1;
        int16_t  dig_H2;
        uint8_t  dig_H3;
        int16_t  dig_H4;
        int16_t  dig_H5;
        int8_t   dig_H6;
    
};

class BME280{
    
public:
  BME280(void);

  bool begin(void);
  
  void setTempOffset(float);

  void readSensor(void);

  float getTemperature(void);
  float getHumidity(void);
  float getPressure(void);
    
private:
  BME280_Calibration_Data _cal_data;
  
  void _readTemperature(void);
  void _readPressure(void);
  void _readHumidity(void);
  void _readSensorCalibrationData(void);
  
  float    _temp_offset;
  int32_t  _temp_offset_fine;
  float    _temperature;
  float    _temperature_adjusted;
  float    _humidity;
  float    _pressure;

  uint8_t   _i2caddr;
  int32_t   _sensorID;
  int32_t   _t_fine;

  void      _write8(byte reg, byte value);
  uint8_t   _read8(byte reg);
  uint16_t  _read16(byte reg);
  uint32_t  _read24(byte reg);
  int16_t   _readS16(byte reg);
  uint16_t  _read16_LE(byte reg);
  int16_t   _readS16_LE(byte reg);
};

#endif
