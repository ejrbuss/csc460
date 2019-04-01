/*
 * Adapted from Roomba2 and Neil's code. (make a good reference for this)
 */

#include "Roomba.h"

namespace Roomba {
    
    bool sensor_ir       = false;
    bool sensor_bumper   = false;
    Roomba_State_t state = Move_State;

    static int serial_num = 0;
    static int brc_pin = 0;
    bool initialized = true;
    
    //
    // Private Function Prototypes
    //
    void start_serial(long baud);
    void write_serial(char val);
    bool read_serial(char *val);
    int available_serial();
    
    //serial_connector determines which UART the Roomba is connected to (0, 1, etc)
    //brc_pin determines where the baud rate change pin is connected.
    void configure(int serial_connector, int baud_pin) {
        serial_num = serial_connector;
        brc_pin = baud_pin;
        pinMode(brc_pin, OUTPUT);
    }

    // Checks the remaining power level.
    bool check_power(unsigned int *power) {
        //Serial.printtln("cp");
        char battery_charge_high = 0;
        char battery_charge_low = 0;
        
        while(read_serial(&battery_charge_high));
        write_serial(SENSORS);
        write_serial(25);
        bool return_value = false;
        delay(50);
        return_value = read_serial(&battery_charge_high);
        read_serial(&battery_charge_low);

        if (return_value) {
            *power = 0;
            *power = (battery_charge_high << 8) | (battery_charge_low);
        }
        
        return return_value;
    }

    // Checks the total power capacity.
    bool check_power_capacity(unsigned int *power) {
        //Serial.printtln("cpc");
        char battery_charge_high = 0;
        char battery_charge_low = 0;
        
        while(read_serial(&battery_charge_high));
        write_serial(SENSORS);
        write_serial(26);
        bool return_value = false;
        delay(50);
        return_value = read_serial(&battery_charge_high);
        read_serial(&battery_charge_low);

        if (return_value) {
            *power = 0;
            *power = (battery_charge_high << 8) | (battery_charge_low);
        }
        return return_value;
    }

    void power_off() {
        write_serial(STOP);
    }
    
    void init(int serial_connector, int baud_pin) {
        serial_num = serial_connector;
        brc_pin = baud_pin;
        pinMode(brc_pin, OUTPUT);
        Serial1.println("Init");
        start_serial(19200U);
        digitalWrite(brc_pin, HIGH);
        
        Serial1.println("Setting Baud");
        //Set baud rate by togling the brc pin 3 times.
        delay(2500);
        digitalWrite(brc_pin, LOW);
        delay(300);
        digitalWrite(brc_pin, HIGH);
        delay(300);
        digitalWrite(brc_pin, LOW);
        delay(300);
        digitalWrite(brc_pin, HIGH);
        delay(300);
        digitalWrite(brc_pin, LOW);
        delay(300);
        digitalWrite(brc_pin, HIGH);    

        start_serial(19200U);
        //Power on
        write_serial(START);
        delay(200);
        //Enter safe mode
        write_serial(SAFE);

        Serial1.println("Done");
    }

    void drive(int velocity, int radius) {
        // Serial1.println("Drive");
        write_serial(DRIVE);
        write_serial(HIGH_BYTE(velocity));
        write_serial(LOW_BYTE(velocity));
        write_serial(HIGH_BYTE(radius));
        write_serial(LOW_BYTE(radius));
    }

    void dock() {
        write_serial(DOCK);
    }

    void get_sensor_data() {
        static bool waiting_for_data = false;
        if (!waiting_for_data) {
            write_serial(SENSORS);
            write_serial(SENSOR_IR);
            write_serial(SENSORS);
            write_serial(SENSOR_BUMPER);
            waiting_for_data = true;
        } else if (available_serial() >= 2) {
            char data;
            read_serial(&data);
            sensor_ir     = data ? true : false;
            read_serial(&data);
            sensor_bumper = data & 0b11 ? true : false;
            waiting_for_data = false;
        }
    }

    void get_data() {
        char val;
        while(read_serial(&val));
    }
    
    void send_command(i8 x, i8 y) {
        if(!initialized) {
            init(serial_num, brc_pin);
            initialized = true;
        }
        
        char command;
        
        if (state == Move_State) {
            if (x > -10 && x < 10 && y > -10 && y < 10) {
                command = 's';
            } else if (x > -10 && x < 10 && y < 0) {
                command = 'f';
            } else if (x > -10 && x < 10 && y > 0) {
                command = 'b';
            } else if (x > 0) {
                command = 'r';
            } else {
                command = 'l';
            }
        } else {
            // Do not allow forward or backward motion
            if (x > -10 && x < 10) {
                command = 's';
            } else if (x > 0) {
                command = 'r';
            } else {
                command = 'l';
            }
        }
        
        Serial1.print(x);
        Serial1.print(", ");
        Serial1.print(y);
        Serial1.print(", ");
        Serial1.println(command);
        
        switch(command) {
            case 'f': 
                drive(150, 32768);
                break;
            case 'b':
                drive(-150, 32768);
                break;
            case 'r':
                drive(50, -1);
                break;
            case 'l':
                drive(50, 1);
                break;
            case 's':
                drive(0,0);
                break;
            case 'd':
                dock();
                break;
            case 'p':
                power_off();
                initialized = false;
                break;
            default:
                break;
        }
    }
    
    void load_song() {
        write_serial(SONG);
        write_serial(0);    // song number
        write_serial(1);    // song length (number of notes)

        write_serial(60);   // middle C
        write_serial(25);   // duration, in 1/64ths of a second
    }

    void play_song(int num) {
        write_serial(PLAY);
        write_serial(num);  // song number
    }

    void start_serial(long baud) {
        switch(serial_num){
        case 0:
            Serial.end();
            Serial.begin(baud);
            break;
        case 1:
            Serial1.end();
            Serial1.begin(baud);
            break;
        case 2:
            Serial2.end();
            Serial2.begin(baud);
            break;
        case 3:
            Serial3.end();
            Serial3.begin(baud);
            break;
        }
    }

    bool read_serial(char *val) {
        switch(serial_num){
        case 0:
            if(Serial.available()) {
                *val = Serial.read();
                return true;
            }
            break;
        case 1:
            if(Serial1.available()) {
                *val = Serial1.read();
                return true;
            }
            break;
        case 2:
            if(Serial2.available()) {
                *val = Serial2.read();
                return true;
            }
            break;
        case 3:
            if(Serial3.available()) {
                *val = Serial3.read();
                return true;
            }
            break;
        }
        return false;
    }

    void write_serial(char val) {
        switch(serial_num){
        case 0:
            Serial.write(val);
            break;
        case 1:
            Serial1.write(val);
            break;
        case 2:
            Serial2.write(val);
            break;
        case 3:
            Serial3.write(val);
            break;
        }
    }

    int available_serial() {
        switch(serial_num){
        case 0:
            return Serial.available();
        case 1:
            return Serial1.available();
        case 2:
            return Serial2.available();
        case 3:
            return Serial3.available();
        default:
            return 0;
        }
    }

}
