//C++ arduino simulator alpha Copyright 2011 Usmar A Padow usmpadow@gmail.com
//Parallel communication between 2 arduinos
//to see output of this program do:
//g++ -g -o arduinosimulator_Parallel_protocol_dual_arduino arduinosimulator_Parallel_protocol_dual_arduino.cpp 
//a@carrydesktop:~/arduino$ ./arduinosimulator_Parallel_protocol_dual_arduino |less

#define HIGH 1
#define LOW 0
#define INPUT 1
#define OUTPUT 0
#define UNSET 2
#include <stdio.h>
#include <unistd.h> //for the usleep function and fork
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */

class Pin {
 private:
 public:
   int *ppinval;
   int pinmode;
 Pin(){
         this->pinmode=UNSET;
 }
   
   /*
   void connect(Pin _pin){
         int test=1;
     this->ppinval=_pin.ppinval;
   }
   */
};

class Serial_ {
 private:
 public:
   void print(char str[]){
         printf("%s",str);
   }
   void println(char str[]){
         printf("%s\n",str);
   }
   
   char read(){//these functions need to be fixed so they do not require return to be pressed, perhaps using SDL or curses
         return getchar();
   }
   int available(){
         return 1;//getchar() != '';
   }
   int begin(int speed){
         return speed;//getchar() != '';
   }

}Serial;

int digitalRead(Pin _pin){
  return *_pin.ppinval;
}
int digitalWrite(Pin _pin, int val){
  *_pin.ppinval = val;
}

Pin PinNull;//this pin is so that I can initialize the array with something to avoid segmentation faults
Pin *AssociativeArray[2][1000];//this array is to keep track of which pin is connected to which to check for short circuits                
void initArray(void) {
        int x;
        for(x=0;x<=1000-1;x++) {
                AssociativeArray[0][x]=&PinNull;
                AssociativeArray[1][x]=&PinNull;
        }
}

void pinMode(Pin *_pin, int mode){//I think I should add a way to warn which pin is short circuiting which pin
        if(mode==OUTPUT) {
                int x;
                for(x=0;x<=1000-1;x++) {
                        //printf("AssociativeArray[0][x]=%x _pin=%x &PinNull=%x\n",AssociativeArray[0][x],_pin,&PinNull);
                        if(AssociativeArray[0][x]==_pin) {
                                Pin tmp=*AssociativeArray[1][x];
                                if(tmp.pinmode==OUTPUT) {
                                        printf("ERROR! possible short circuit detected between two pins connected together and are both set to OUTPUT, this can fry a port on your arduino!\n");
                                        exit(1);
                                }
                        }
                }
        }
        //Pin tmp=*_pin;
        //tmp.pinmode=mode;
 //here is the next problem, I cannot seem to set this property
 //when I have a pointer to an object how can I assing a value to a variable inside the object?
  //*_pin.pinmode=mode;
  _pin->pinmode=mode;
}

void delayMicroseconds(int microseconds) {
        usleep(microseconds);//microseconds are a millionth of a second
}
void delay(int milliseconds) {
        usleep(milliseconds*1000);//microseconds are a millionth of a second
}

//make pin obects 
Pin reciever_recieve_ready;//acknowledgement pin
Pin reciever_send_ready;//acknowledgement pin
Pin reciever_dataPin0;
Pin reciever_dataPin1;
Pin reciever_dataPin2;
Pin reciever_dataPin3;
Pin reciever_dataPin4;
Pin reciever_dataPin5;
Pin reciever_dataPin6;
Pin reciever_dataPin7;

Pin sender_recieve_ready;//acknowledgement pin
Pin sender_send_ready;//acknowledgement pin
Pin sender_dataPin0;
Pin sender_dataPin1;
Pin sender_dataPin2;
Pin sender_dataPin3;
Pin sender_dataPin4;
Pin sender_dataPin5;
Pin sender_dataPin6;
Pin sender_dataPin7;

//create wires
int wire0=LOW;
int wire1=LOW;
int wire2=LOW;
int wire3=LOW;
int wire4=LOW;
int wire5=LOW;
int wire6=LOW;
int wire7=LOW;
int wire8=LOW;
int wire9=LOW;
//define how often the circuit should be tested the smaller the faster it will be updated
//int CircuitUpdateDelay=200;//wait 200 milliseconds before updating circuit

//define the behaviour of your circuit here, I will be pretending I have an LED connected to a wire(lets ignore the needed ressistor for now)
void circuit(void) {
        //in this circuit the wires just transfer power(in real life we will need resistors). we dont need to define circuit behaviour
}

void connect(){
        //set initial values of a pin or you will get a segmentation fault
    reciever_recieve_ready.ppinval=&wire0;//acknowledgement pin
    reciever_send_ready.ppinval=&wire1;//acknowledgement pin

        reciever_dataPin0.ppinval=&wire2;
        reciever_dataPin1.ppinval=&wire3;
        reciever_dataPin2.ppinval=&wire4;
        reciever_dataPin3.ppinval=&wire5;
        reciever_dataPin4.ppinval=&wire6;
        reciever_dataPin5.ppinval=&wire7;
        reciever_dataPin6.ppinval=&wire8;
        reciever_dataPin7.ppinval=&wire9;

    sender_recieve_ready.ppinval=&wire0;//crossing wires
    sender_send_ready.ppinval=&wire1;//crossing wires

        sender_dataPin0.ppinval=&wire2;
        sender_dataPin1.ppinval=&wire3;
        sender_dataPin2.ppinval=&wire4;
        sender_dataPin3.ppinval=&wire5;
        sender_dataPin4.ppinval=&wire6;
        sender_dataPin5.ppinval=&wire7;
        sender_dataPin6.ppinval=&wire8;
        sender_dataPin7.ppinval=&wire9;
        
        //populare array of pins that are connected to each other
        //I wond do this here because I wont be cchanging the pinMode at runtime
}


