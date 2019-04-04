/*
 * Adapted from Roomba2 and Neil's code. (make a good reference for this)
 */

#include "Roomba.h"

#define SENSOR_BUMPER 7
#define SENSOR_IR     13

#define MAX_SPEED       200
#define TURN_SPEED      100
#define MAX_TURN_RADIUS 32768

#define HIGH_BYTE(x) (x >> 8)
#define LOW_BYTE(x)  (x & 0xFF)

// This is not an exhaustive list of commands.
#define START   128     // start the Roomba's serial command interface
#define BAUD    129     // set the SCI's baudrate (default on full power cycle is 57600
#define CONTROL 130     // enable control via SCI
#define SAFE    131     // enter safe mode
#define FULL    132     // enter full mode
#define POWER   133     // put the Roomba to sleep
#define SPOT    134     // start spot cleaning cycle
#define CLEAN   135     // start normal cleaning cycle
#define MAX     136     // start maximum time cleaning cycle
#define DRIVE   137     // control wheels
#define MOTORS  138     // turn cleaning motors on or off
#define LEDS    139     // activate LEDs
#define SONG    140     // load a song into memory
#define PLAY    141     // play a song that was loaded using SONG
#define SENSORS 142     // retrieve one of the sensor packets
#define DOCK    143     // force the Roomba to seek its dock.
#define STOP    173

namespace Roomba {
    
    Roomba_State_t state = Still_State;

    static HardwareSerial * roomba_serial;
    static int brc_pin = 0;

    void init(HardwareSerial * serial, int baud_pin) {
        
        roomba_serial = serial;
        brc_pin       = baud_pin;
        
        pinMode(brc_pin, OUTPUT);

        roomba_serial->end();
        roomba_serial->begin(19200U);

        digitalWrite(brc_pin, HIGH);        

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

        roomba_serial->end();
        roomba_serial->begin(19200U);

        //Power on
        roomba_serial->write(START);
        delay(200);
        roomba_serial->write(SAFE);

        u8 move_song_notes[]  = { 67, 25 };
        load_song(Move_Song, 1, move_song_notes);
        u8 still_song_notes[] = { 60, 25 };
        load_song(Still_Song, 1, still_song_notes);
    }

    void move(i8 x, i8 y) {
        
        i16 radius = MAX_TURN_RADIUS;
        if (x > 0) {
            radius = 1;
        } else if (x < 0) {
            radius = -1;
        }

        i16 velocity = x == 0 ? 0 : TURN_SPEED;
        
        if (state == Move_State && abs(y) > abs(x)) {
            radius   = MAX_TURN_RADIUS;
            velocity = (((i32) y) * MAX_SPEED) / 127;
            if (x != 0) {
                if (velocity > 0) {
                    velocity = clamp(velocity, TURN_SPEED, MAX_SPEED);
                } else {
                    velocity = clamp(velocity, -MAX_SPEED, -TURN_SPEED);
                }
            }
        }
        
        roomba_serial->write(DRIVE);
        roomba_serial->write(HIGH_BYTE(velocity));
        roomba_serial->write(LOW_BYTE(velocity));
        roomba_serial->write(HIGH_BYTE(radius));
        roomba_serial->write(LOW_BYTE(radius));
    }

    void load_song(Song_t song, u8 length, u8 * notes) {
        roomba_serial->write(SONG);
        roomba_serial->write(song);
        roomba_serial->write(length);
        i16 i = 0;
        for (i = 0; i < 2 * length; i++) {
            roomba_serial->write(notes[i]);
        }
    }

    void play_song(Song_t song) {
        roomba_serial->write(PLAY);
        roomba_serial->write((u8) song);
    }

    namespace Sensors {
        bool ir     = false;
        bool bumper = false;
        
        void update() {
            static bool waiting_for_data = false;
            if (!waiting_for_data) {
                roomba_serial->write(SENSORS);
                roomba_serial->write(SENSOR_IR);
                roomba_serial->write(SENSORS);
                roomba_serial->write(SENSOR_BUMPER);
                waiting_for_data = true;
            } else if (roomba_serial->available() >= 2) {
                ir     = roomba_serial->read() ? true : false;
                bumper = roomba_serial->read() & 0b11 ? true : false;
                waiting_for_data = false;
            }
        }
    }

}