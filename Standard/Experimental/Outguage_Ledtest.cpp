#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <wiringPi.h> // Include wiringPi

#define BUFLEN 512

// General
#define PSI 14.504 // BAR->PSI
#define MPH 2.237  // M/S->MPH
#define KPH 3.6    // M/S->KMH

#define bool char
#define true 1
#define false 0

// Outgauge Flags
#define OG_TURBO 8192
#define OG_KM 16384
#define OG_BAR 32768

// Light Flags
#define DL_FULLBEAM (1 << 1)
#define DL_HANDBRAKE (1 << 2)
#define DL_TC (1 << 4)
#define DL_SIGNAL_L (1 << 5)
#define DL_SIGNAL_R (1 << 6)
#define DL_SIGNAL_ANY (1 << 7)
#define DL_OILWARN (1 << 8)
#define DL_BATTERY (1 << 9)
#define DL_ABS (1 << 10)

// Outgauge structure
typedef struct Outgauge_t {
    unsigned time;
    char car[4];
    unsigned short flags;
    char gear;
    char plid;
    float speed;
    float rpm;
    float turbo;
    float engTemp;
    float fuel;
    float oilPressure;
    float oilTemp;
    unsigned dashLights;
    unsigned showLights;
    float throttle;
    float brake;
    float clutch;
    char display1[16];
    char display2[16];
    int id;
} Outgauge_t;

bool is_metric = false;

// Define GPIO pins for output
 int LTS = 3;
 int RTS = 14;
 int FULLBEAM_PIN = 15;
 int HAND_BRAKE_PIN = 16;
 int LOWOIL_PIN = 4;
 int BADBATTERY_PIN = 21;
 int RPM_PIN = 17;
 int SPEED_PIN = 18;
 int OIL_PRESSURE_PIN = 19;
 int ENGINE_TEMP_PIN = 20;
 int ABS_PIN =  2;

void Outgauge_Output(Outgauge_t* outgauge) {
    // Set LED's
   digitalWrite(LTS,            (outgauge->showLights & DL_SIGNAL_L) ? HIGH : LOW);
   digitalWrite(RTS,            (outgauge->showLights & DL_SIGNAL_R) ? HIGH : LOW);
   digitalWrite(FULLBEAM_PIN,   (outgauge->showLights & DL_FULLBEAM) ? HIGH : LOW);
   digitalWrite(HAND_BRAKE_PIN, (outgauge->showLights & DL_HANDBRAKE) ? HIGH : LOW);
   digitalWrite(LOWOIL_PIN,     (outgauge->showLights & DL_OILWARN) ? HIGH : LOW);
   digitalWrite(BADBATTERY_PIN, (outgauge->showLights & DL_BATTERY) ? HIGH : LOW); 
   digitalWrite(ABS_PIN,        (outgauge->showLights & DL_ABS) ? HIGH : LOW);

    // Map RPM, Speed, Oil Pressure, Engine Temp to GPIO pins
    digitalWrite(RPM_PIN, (outgauge->rpm > 0) ? HIGH : LOW); // Example: HIGH if RPM > 0
    digitalWrite(SPEED_PIN, (outgauge->speed > 0) ? HIGH : LOW); // Example: HIGH if Speed > 0
    digitalWrite(OIL_PRESSURE_PIN, (outgauge->oilPressure > 0) ? HIGH : LOW); // Example: HIGH if Oil Pressure > 0
    digitalWrite(ENGINE_TEMP_PIN, (outgauge->engTemp > 0) ? HIGH : LOW); // Example: HIGH if Engine Temp > 0
}

int main(int argc, char* argv[]) {
    int port = 4444;
    std::string ip = "0.0.0.0"; // Listen on all interfaces

    // Initialize wiringPi
    wiringPiSetupGpio(); // Initialize wiringPi
    pinMode(LTS, OUTPUT);
    pinMode(RTS, OUTPUT);
    pinMode(FULLBEAM_PIN, OUTPUT); 
    pinMode(HAND_BRAKE_PIN, OUTPUT); // 
    pinMode(LOWOIL_PIN, OUTPUT);
    pinMode(BADBATTERY_PIN, OUTPUT);
    pinMode(ABS_PIN, OUTPUT);
    pinMode(RPM_PIN, OUTPUT);
    pinMode(SPEED_PIN, OUTPUT);
    pinMode(OIL_PRESSURE_PIN, OUTPUT);
    pinMode(ENGINE_TEMP_PIN, OUTPUT);

    // Create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Bind socket to local port
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("Error binding socket");
        close(sock);
        return 1;
    }

    // Loop to read data
    char buf[BUFLEN];
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);

    while (true) {
        int recv_len = recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr*)&remote_addr, &remote_addr_len);
        if (recv_len < 0) {
            perror("Error receiving data");
            break;
        }

        Outgauge_Output((Outgauge_t*)buf);
    }

    // Cleanup
    close(sock);
    return 0;
}
