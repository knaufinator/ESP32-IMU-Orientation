#include "MPU9250.h"
#include "eeprom_utils.h"

MPU9250 mpu;

void setup() {
    Serial.begin(500000);
    Wire.begin();
    delay(2000);

    if (!mpu.setup(0x68)) {
       
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }
    
    EEPROM.begin(0x80);
    
    loadCalibration();
    print_calibration();

   mpu.selectFilter(QuatFilterSel::MAHONY);
   mpu.setMagneticDeclination(-8.22);
}

void calibrateMPU()
{
    Serial.println("Accel Gyro calibration");
    Serial.println("Please leave the device still on the flat plane.");
    mpu.verbose(true);
    mpu.calibrateAccelGyro();

    Serial.println("Mag calibration will start in 5sec.");
    Serial.println("Please Wave device in a figure eight until done.");

    delay(5000);
    mpu.calibrateMag();

    print_calibration();
    mpu.verbose(false);

   //  save to eeprom
   saveCalibration();
  }

void loop() {
   
   if (mpu.update()) {
        static uint32_t prev_ms = millis();
        
        //print tememetry at most every 1ms
        if (millis() > prev_ms + 1) {
            print_all();
            prev_ms = millis();
        }
    }

  
  while(Serial.available()) {
    // get the character
    char ch = Serial.read();
  
    // echo the character to serial monitor or PuTTY
    Serial.write(ch);

    //if user inputs "1" start calibration sequence
    if(ch == '1')
    {
      calibrateMPU();
      loadCalibration();
      print_calibration();
    }
  }
}

void print_all() {
  
   //prints to serial - yaw,pitch,roll,linaccellerationx,yz, and xyzw quaternions for 3d orientation
    Serial.print("X");
    Serial.print(mpu.getYaw(),10); Serial.print(",");
    Serial.print(mpu.getPitch(), 10);Serial.print(",");
    Serial.println(mpu.getRoll(), 10); Serial.print(",");
    
    Serial.print(mpu.getLinearAccX(),10); Serial.print(",");
    Serial.print(mpu.getLinearAccY(), 10);Serial.print(",");
    Serial.println(mpu.getLinearAccZ(), 10);Serial.print(",");

    Serial.print(mpu.getQuaternionX(),10); Serial.print(",");
    Serial.print(mpu.getQuaternionY(), 10);Serial.print(",");
    Serial.println(mpu.getQuaternionZ(), 10);Serial.print(",");
    Serial.print(mpu.getQuaternionW(),10);

    Serial.println("E");  
}

void print_calibration() {
    Serial.println("< calibration parameters >");
    Serial.println("accel bias [g]: ");
    Serial.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.println();
    Serial.println("gyro bias [deg/s]: ");
    Serial.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.println();
    Serial.println("mag bias [mG]: ");
    Serial.print(mpu.getMagBiasX());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasY());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasZ());
    Serial.println();
    Serial.println("mag scale []: ");
    Serial.print(mpu.getMagScaleX());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleY());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleZ());
    Serial.println();
}
