//#include "nRF52_MBED_PWM.h"
#include "PWM2.h"
#include "AudioManager.h"
#include "BLESender.h"

BLESender * sender;
AudioManager * audio;

unsigned long time_restart = 5000;
unsigned long last;

int mode = 0;
unsigned long ttt = 0;
unsigned long last_tt = 0;
unsigned long cycle=ttt; 
int f = 0;



void setup_BLE();
void setup_PWM();
void setup_AudioManager();
int freq_modulation(unsigned long t_0, unsigned long t);
void check_connection();
void check_speaker_shutdown();
void check_buffer_full();
void check_sending_complete();

void start_recording();
void start_sending();

void setup() {
  
    setup_BLE();
    setup_PWM();
    setup_AudioManager();
    //Serial.begin(9600);
}

void loop() {
    

   
    switch (mode) {
        case 0: {
            // Wait for connection
            check_connection();
            break;
            
        }
        case 1: {
            ttt = millis();
            while (last_tt+10<=ttt){
              if(f<17 && f%2==0){
                start_pwm2(f*125);
                }             
              else if(f<=17 && f%2==1){
                stop_pwm2();
 
              }
              f = f + 1;                                          
              last_tt = ttt;
              }             
              if (cycle+200<=ttt){f=0;cycle=ttt;last_tt=0;}
     
                                                
        
            // Wait for speaker shut down
                    
           check_speaker_shutdown();
            break;
        }
        case 2: {

            // Wait for buffer full
            check_buffer_full();
            break;
        }
        case 3: {
      
            // Wait for data to be sent
            check_sending_complete();
            break;
        }
        case 4: {
         
            // Wait for restart
            if (millis() - last >= time_restart) {
                NVIC_SystemReset();
            }
            break;
        }
        default: {
            break;
        }
    }

    sender->update();
    audio->update();
}


void setup_BLE() {
    sender = new BLESender();
    sender->begin();
}

void setup_PWM() {
    begin_pwm2();
}

void setup_AudioManager() {
    audio = new AudioManager();
    audio->begin();
}

void check_connection() {
    if (sender->connected()) {
        mode = 1;
        start_recording();
    }
}

void check_speaker_shutdown() {
    if (audio->almost_full()) {
        mode = 2;
        stop_pwm2();
    }
}

void check_buffer_full() {
    if (audio->full()) {
        mode = 3;
        audio->stop();
        start_sending();
    }

}

void check_sending_complete() {
    if (sender->check_sending_complete()) {
        mode = 4;
        //audio->clear();
        last = millis();
    }
}

void start_recording() {
    audio->start();
    audio->warmup();
    //Transmitting the PWM sound 
    
    start_pwm2(0);
}

void start_sending() {
    short * buff = audio->get_fullbuffer();
    int size = audio->get_fullbuffer_size();
    sender->send_buffer(buff, size);
}

int freq_modulation(unsigned long t_0, unsigned long t){

}
