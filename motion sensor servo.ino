// TUGAS 3 PROGRAMMER GMRT
// Nama : Adrian Enrico DIbyantara
// NIM  : 26/583045/TK/66456


#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>
#include <math.h>

// PIN SERVO

#define SERVO1_PIN 13
#define SERVO2_PIN 12
#define SERVO3_PIN 4
#define SERVO4_PIN 2
#define SERVO5_PIN 26

// PIN PIR

#define PIR_PIN 23

// PIN MPU6050

#define SDA_PIN 21
#define SCL_PIN 22

// OBJECT

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

Adafruit_MPU6050 mpu;

// KONSTANTA


// Posisi tengah servo (90 derajat = 0 derajat pada sistem.)
const int INITIAL_POSITION = 90;
// Variabel untuk menyimpan posisi yaw
float yaw = 0.0;

// Sudut untuk mode uji lewat Serial Monitor
float testRoll = 0.0;
float testPitch = 0.0;
float testYaw = 0.0;
bool manualTestMode = true;
bool automaticDemoMode = false;

// Menyimpan waktu pembacaan sebelumnya
unsigned long previousTime = 0;


// FUNGSI:SERVO KEMBALI KE POSISI AWAL

void semuaServoKeAwal()
{
    servo1.write(INITIAL_POSITION);
    servo2.write(INITIAL_POSITION);
    servo3.write(INITIAL_POSITION);
    servo4.write(INITIAL_POSITION);
    servo5.write(INITIAL_POSITION);
}

void tampilkanBantuan()
{
    Serial.println("Mode uji manual aktif.");
    Serial.println("w/s: roll +/-10 | a/d: pitch +/-10 | q/e: yaw +/-10");
    Serial.println("x: reset sudut | z: demo otomatis | m: mode MPU6050");
}

void tulisPosisiServo(float roll, float pitch, float yawAngle)
{
    servo1.write(constrain(INITIAL_POSITION - roll, 0, 180));
    servo2.write(constrain(INITIAL_POSITION + roll, 0, 180));
    servo3.write(constrain(INITIAL_POSITION + pitch, 0, 180));
    servo4.write(constrain(INITIAL_POSITION + pitch, 0, 180));
    servo5.write(constrain(INITIAL_POSITION + yawAngle, 0, 180));
}

void tampilkanStatus(float roll, float pitch, float yawAngle, const char *mode)
{
    int servo1Position = constrain(INITIAL_POSITION - roll, 0, 180);
    int servo2Position = constrain(INITIAL_POSITION + roll, 0, 180);
    int servo3Position = constrain(INITIAL_POSITION + pitch, 0, 180);
    int servo4Position = constrain(INITIAL_POSITION + pitch, 0, 180);
    int servo5Position = constrain(INITIAL_POSITION + yawAngle, 0, 180);

    Serial.print(mode);
    Serial.print(" | Roll: ");
    Serial.print(roll);
    Serial.print(" | Pitch: ");
    Serial.print(pitch);
    Serial.print(" | Yaw: ");
    Serial.print(yawAngle);
    Serial.print(" | Servo1: ");
    Serial.print(servo1Position);
    Serial.print(" | Servo2: ");
    Serial.print(servo2Position);
    Serial.print(" | Servo3: ");
    Serial.print(servo3Position);
    Serial.print(" | Servo4: ");
    Serial.print(servo4Position);
    Serial.print(" | Servo5: ");
    Serial.println(servo5Position);
}

void prosesPerintahSerial()
{
    while (Serial.available() > 0)
    {
        char command = Serial.read();

        if (command == 'w') testRoll += 10;
        if (command == 's') testRoll -= 10;
        if (command == 'a') testPitch += 10;
        if (command == 'd') testPitch -= 10;
        if (command == 'q') testYaw += 10;
        if (command == 'e') testYaw -= 10;

        if (command == 'x')
        {
            testRoll = 0;
            testPitch = 0;
            testYaw = 0;
        }

        if (command == 'm')
        {
            manualTestMode = false;
            automaticDemoMode = false;
            previousTime = millis();
            Serial.println("Mode MPU6050 aktif.");
        }

        if (command == 't')
        {
            manualTestMode = true;
            automaticDemoMode = false;
            Serial.println("Mode uji manual aktif.");
        }

        if (command == 'z')
        {
            manualTestMode = true;
            automaticDemoMode = !automaticDemoMode;
            Serial.println(automaticDemoMode
                ? "Demo otomatis aktif."
                : "Demo otomatis berhenti.");
        }

        if (command == 'h') tampilkanBantuan();
    }
}

// FUNGSI: GERAKAN KETIKA PIR MENDETEKSI GERAKAN

void gerakanPIR()
{
    Serial.println("Gerakan eksternal terdeteksi!");

     // Posisi ini bebas ditentukan programmer.
     
     // MISAL kita menggunakan:
     
     // Servo 1 = 120 derajat
     // Servo 2 = 120 derajat
     // Servo 3 = 60 derajat
     // Servo 4 = 60 derajat
     // Servo 5 = 120 derajat
     

    servo1.write(120);
    servo2.write(120);
    servo3.write(60);
    servo4.write(60);
    servo5.write(120);

    // Servo berada pada posisi tersebut selama 1 detik
    delay(1000);

    // Setelah itu kembali ke posisi awal
    semuaServoKeAwal();

    Serial.println("Servo kembali ke posisi awal.");
}

// SETUP

void setup()
{
// Memulai komunikasi Serial Monitor
    Serial.begin(115200);

// MEMULAI I2C
    

    Wire.begin(SDA_PIN, SCL_PIN);


    
// MEMULAI PIR
    

    pinMode(PIR_PIN, INPUT);

// MENGHUBUNGKAN SERVO KE GPIO

    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    servo3.attach(SERVO3_PIN);
    servo4.attach(SERVO4_PIN);
    servo5.attach(SERVO5_PIN);

    // SEMUA SERVO KE POSISI AWAL

    semuaServoKeAwal();

   // MEMULAI MPU6050

    if (!mpu.begin())
    {
        Serial.println("MPU6050 tidak ditemukan!");

        // Program berhenti jika MPU6050 tidak ditemukan
        while (1)
        {
            delay(10);
        }
    }

    // KONFIGURASI MPU6050

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    previousTime = millis();


    Serial.println("--------------------------------");
    Serial.println("MPU6050 berhasil ditemukan!");
    Serial.println("Semua servo berada pada posisi awal.");
    Serial.println("Sistem siap.");
    Serial.println("--------------------------------");
    tampilkanBantuan();
}

// LOOP

void loop()
{
    prosesPerintahSerial();

    if (automaticDemoMode)
    {
        float timeSeconds = millis() / 1000.0;
        float demoRoll = 90.0 * sin(timeSeconds);
        float demoPitch = 90.0 * cos(timeSeconds);
        float demoYaw = 90.0 * sin(timeSeconds * 0.5);

        tulisPosisiServo(demoRoll, demoPitch, demoYaw);
        tampilkanStatus(demoRoll, demoPitch, demoYaw, "Demo");
        delay(100);
        return;
    }

    if (manualTestMode)
    {
        testRoll = constrain(testRoll, -90, 90);
        testPitch = constrain(testPitch, -90, 90);
        testYaw = constrain(testYaw, -90, 90);

        tulisPosisiServo(testRoll, testPitch, testYaw);
        tampilkanStatus(testRoll, testPitch, testYaw, "Uji");
        delay(100);
        return;
    }

    // Membaca data MPU6050

    sensors_event_t acceleration;
    sensors_event_t gyro;
    sensors_event_t temperature;

    mpu.getEvent(
        &acceleration,
        &gyro,
        &temperature
    );


    // Membaca PIR

    int pirState = digitalRead(PIR_PIN);


    if (pirState == HIGH)
    {
        gerakanPIR();
    }

    else
    {

        // ROLL


        float roll = atan2(
            acceleration.acceleration.y,
            acceleration.acceleration.z
        ) * 180.0 / PI;



        // PITCH


        float pitch = atan2(
            -acceleration.acceleration.x,
            sqrt(
                acceleration.acceleration.y *
                acceleration.acceleration.y
                +
                acceleration.acceleration.z *
                acceleration.acceleration.z
            )
        ) * 180.0 / PI;


        // Batasi Roll dan Pitch

        roll = constrain(roll, -90, 90);
        pitch = constrain(pitch, -90, 90);



        // YAW


        unsigned long currentTime = millis();

        float deltaTime =
            (currentTime - previousTime) / 1000.0;

        previousTime = currentTime;

        float gyroZ =
            gyro.gyro.z * 180.0 / PI;

        float yawChange =
            gyroZ * deltaTime;

        yaw = yaw + yawChange;

        yaw = constrain(yaw, -90, 90);



        // SERVO 1 & 2 - ROLL


        int servo1Position =
            INITIAL_POSITION - roll;

        int servo2Position =
            INITIAL_POSITION + roll;



        // SERVO 3 & 4 - PITCH

        int servo3Position =
            INITIAL_POSITION + pitch;

        int servo4Position =
            INITIAL_POSITION + pitch;


        // SERVO 5 - YAW

        int servo5Position =
            INITIAL_POSITION + yaw;



        // KIRIM KE SERVO

        tulisPosisiServo(roll, pitch, yaw);


   
        // SERIAL MONITOR
   
        tampilkanStatus(roll, pitch, yaw, "MPU6050");


        delay(50);
    }
}
