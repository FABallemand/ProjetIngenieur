# Arduino Device

## Description

This Arduino device is used to collect data. Mounted on a vehicle, it will be able to collect accelerometer and GPS data and store them localy on a micro-SD card.  

## Build

All components are mounted on a custom PCB (design files located in **pcb** folder).  

- Arduino Nano  
- MPU-6065 IMU (Inertial Measurement Unit) (Accelerometer and Gyroscope)  
- BN-880 GPS  
- Micro-SD card reader  
- Push button  

## Usage

On it has been build, the program located in the **program** folder has to be flashed on the Arduino. The device will start/stop recording when the button is pressed.

## Ressouces

- DroneBot Workshop: [Build an Electronic Level with MPU-6050 and Arduino](https://www.youtube.com/watch?v=XCyRXMvVSCw&ab_channel=DroneBotWorkshop)  
- fire-DIY: [Calibrer le capteur MPU6050 avec un Arduino](https://www.firediy.fr/article/calibrer-le-capteur-mpu6050-avec-un-arduino-drone-ch-5)  
- DroneBot Workshop: [GPS Modules with Arduino and Raspberry Pi](https://www.youtube.com/watch?v=kwk3qzaIcCU&ab_channel=DroneBotWorkshop)  
- DroneBot Workshop: [Using SD Cards with Arduino - Record Servo Motor Movements](https://www.youtube.com/watch?v=PQhQfww-qGQ&ab_channel=DroneBotWorkshop)  
- DroneBot Workshop: [Arduino Real Time Clock - Using the Tiny RTC](https://www.youtube.com/watch?v=lyvoOEO-Ncg&ab_channel=DroneBotWorkshop)  
- Arduino: [State Change Detection (Edge Detection) for pushbuttons](https://docs.arduino.cc/built-in-examples/digital/StateChangeDetection)  
- How To Mechatronics: [How I2C Communication Works and How To Use It with Arduino](https://www.youtube.com/watch?v=6IAkYpmA1DQ&ab_channel=HowToMechatronics)