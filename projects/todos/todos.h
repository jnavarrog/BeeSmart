#define DS18B20_AMOUNT 4

typedef enum Pfe_Sensor_Type {
  PFE_SENSOR_TYPE_TEMPERATURE_TEMPERATURE,
  PFE_SENSOR_TYPE_TEMPERATURE_ADDRESS,
  PFE_SENSOR_TYPE_WEIGHT_WEIGHT,
  PFE_SENSOR_TYPE_SERVO_ON,
  PFE_SENSOR_TYPE_SERVO_OFF
} Pfe_Sensor_Type;

typedef enum Pfe_Sensor_Configuration_Type {
  PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_START,
  PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_INDEX,
  PFE_SENSOR_CONFIGURATION_TYPE_SERVO_ON,
  PFE_SENSOR_CONFIGURATION_TYPE_SERVO_OFF
} Pfe_Sensor_Configuration_Type;

typedef struct Pfe_Sensor_Configuration {
  bool                temperature_sensor_started;
  uint8_t             temperature_sensor_index;
  Hx711_Reading_Type  weight_reading_type;
} Pfe_Sensor_Configuration;

extern const struct sensors_sensor pfe_sensors;
