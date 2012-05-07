//Copyright 2012 Usmar Padow usm@padow@gmail.com and Greg Cox
//SD memory manager and BASIC interpreter based on Spinelli's BASIC interpreter
//Uses SD memory instead of RAM
//to build your own SD card reader, use this link http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1206874649
//Use my modified library in the libraries subdirectory for Arduino Uno, for other arudinos maybe the library probided in the link may work

#ifdef __linux__
	#include "ArduinoSimulator.cpp"
	#define byte_type char
	char* freeMemory(){
		  return "This is just a dummy,so the ammount of memory wont be displayed";
	}
		
	//SDCard library simulator
	char SDmem[1000000];
	void sd_raw_write(int offset, char byteArray[],int length){
  		for(int counter=0;counter<length;counter++){
    		SDmem[counter+offset]=byteArray[counter];
  		}
	}

	void sd_raw_read(int offset, char byteArray[],int length){
  		for(int counter=0;counter<length;counter++){
    		byteArray[counter]=SDmem[counter+offset];
  		}
	}
	void sd_raw_sync(){
	}
	int sd_raw_init(){//may want to make a system to make sure that this function is called before you can read or write to the SD card, or show an error
		return 1;
	}
#include <time.h>
#include <stdlib.h>

int rand2(unsigned int max, unsigned int min)
{
  srand(time(NULL));
  int base_random = rand(); /* in [0, RAND_MAX] */
  if (RAND_MAX == base_random) rand2(max,min);
  /* now guaranteed to be in [0, RAND_MAX) */
  int range       = max - min,
      remainder   = RAND_MAX % range,
      bucket      = RAND_MAX / range;
  /* There are range buckets, plus one smaller interval
     within remainder of RAND_MAX */
  if (base_random < RAND_MAX - remainder) {
    return min + base_random/bucket;
  } else {
    return rand2(max,min);
  }
}
#else
	#include <MemoryFree.h>
	//int var[100];//this works
	#include <sd-reader_config.h>
	#include <sd_raw.h>
	#include <sd_raw_config.h>
        #include <Arduino.h>
    #define byte_type byte
#endif


//Arduino SD BASIC interpretera Copyright 2011 Usmar A Padow

#include<stdio.h>
#include<stdint.h>
#include<string.h>
//we want this only to compule for the simulator, for the real arduino, we want to use the SD card


int ProgNum=0;
char NumberOfLines=0;
#define LineLength 30//I made this small because o the limit in arudino memory
//warning this is because of the limit of the size of char which we will use to store ln, we may change it later
#define LineCount 250
#define NumberOfPrograms 10
#define ProgramSize LineLength*LineCount
#define ln0offset ProgramSize*NumberOfPrograms
//nol=number of lines
#define noloffset ln0offset+(LineCount*NumberOfPrograms)
//greg, is this the same thing? #define noloffset ln0offset+(ProgramSize*NumberOfPrograms)
//#include <arduino.h>
//byte_type tempbytes[LineLength];
byte_type tempbytes[LineCount];//this is nessesary to have a program delete command,  but it might be too much memory when I mix it with the TVout library, so I may need to change it to the commented line above

char OneLine[LineLength];//used many times, global var
int i;
int ReturnStack[5];//increase this if you think there will be more than 5 levels of return from GOSUBs
int StackCount=0;

void delete_lns() {
   for(i=1;i<LineCount;i++){
	      tempbytes[i]=0;
    }
	unsigned long offset;
	offset=(unsigned long) ln0offset+(LineCount*ProgNum);
	sd_raw_write(offset,tempbytes,LineCount+1);
    sd_raw_sync();
}

void delete_nol() {
	tempbytes[0]=0;//set teh number of lines back to 0
	unsigned long offset;
	offset=(unsigned long) noloffset+ProgNum;
	sd_raw_write(offset,tempbytes,1);
        sd_raw_sync();
}
void writeString (int offset, char str[], int len) {
  for(int counter=0;counter<len;counter++){
    tempbytes[counter]=str[counter];
    //char output[100];
    //sprintf(output,"%c",str[counter]);
    //Serial.print(output);
  }
  sd_raw_write(offset,tempbytes,len);
  sd_raw_sync();
}


void set_ln(char RealLineNumber, char SDPointer) {
	tempbytes[0]=SDPointer;
	unsigned long offset;
	offset=(unsigned long) ln0offset+(LineCount*ProgNum)+RealLineNumber;
	sd_raw_write(offset,tempbytes,1);
        sd_raw_sync();
}

char get_ln(char RealLineNumber) {
        unsigned long offset;
	offset=(unsigned long) ln0offset+(LineCount*ProgNum)+RealLineNumber;
        sd_raw_read(offset,tempbytes,1);//it seems this read fails
	return tempbytes[0];
}
void set_nol() {
	tempbytes[0]=NumberOfLines;
	unsigned long offset;
	offset=(unsigned long) noloffset+ProgNum;
	sd_raw_write(offset,tempbytes,1);
        sd_raw_sync();
}
int get_nol() {
	unsigned long offset;
	offset=(unsigned long) noloffset+ProgNum;
  	sd_raw_read(offset,tempbytes,1);
	return tempbytes[0];
}

//void bytecpy(byte_type info[]){
 void pr(unsigned long offset,int len){
  sd_raw_read(offset,tempbytes,len);
  for(int counter=0;counter<len;counter++) {
    //Serial.print((char)tempbytes[counter]);
  }
}

 void pr2(unsigned long offset,int len){
  sd_raw_read(offset,tempbytes,len);
  for(int counter=0;counter<len;counter++) {
    OneLine[counter]=(char)tempbytes[counter];
    //Serial.print((char)tempbytes[counter]);
  }
}
/*
void bytecpy(unsigned long offset,int len){
      int counter;
      Serial.print("\r\n");
      for(counter=0;counter<LineLength;counter++) {
           OneLine[counter]=(char)tempbytes[counter];
           char tmp[3];
           sprintf(tmp,"*%c",(char)tempbytes[counter]);
           Serial.print(tmp);
      }
      //p[counter]='\0';
 
       sd_raw_read(offset,tempbytes,len);
       for(int counter=0;counter<len;counter++) {
            Serial.print((char)tempbytes[counter]);
       }
}
*/
/* I want this but it uses RAM
char * string_repeat( int n, const char * s ) {
  size_t slen = strlen(s);
  char * dest = malloc(n*slen+1);
 
  int i; char * p;
  for ( i=0, p = dest; i < n; ++i, p += slen ) {
    memcpy(p, s, slen);
  }
  *p = '\0';
  return dest;
}
*/

void AddLine(char RealLineNumber, char str[]) {
        char SDPointer=get_ln(RealLineNumber);//this seems to conflict with the var[0]=0; inside loop()
        //char SDPointer=0;//this works, so it is probably something inside get_ln();
        //Serial.print("here7");
         
        //delay(1000);
	unsigned long offset;
	if(SDPointer) {//editing an existing line
		offset=ProgNum*ProgramSize+SDPointer*LineLength;
	} else {//creating a new line
		if(NumberOfLines+1>LineCount) { 
			Serial.print("Error, the maximum number of lines possible is 250");
		}
		NumberOfLines++;
                set_nol();
		set_ln(RealLineNumber,NumberOfLines);
		offset=(unsigned long) ProgNum*ProgramSize+NumberOfLines*LineLength;
	}
        writeString(offset,str,strlen(str)+1);//the +1 is so that it will also add the null character of teh string        
/*
        //make a string that takes up the whole line legnth
        strcpy(OneLine,"                              ");//30 spaces
        for(i=0;i<strlen(str)-1;i++) {//copy the string, except the null char
              OneLine[i]=str[i];
        }
        Serial.print("-->");
        Serial.print(OneLine);
        Serial.print("<--");
	writeString(offset,OneLine,LineLength);
 */      
}
void  get_Line(char *p, char RealLineNumber) {
	char SDPointer=get_ln(RealLineNumber);
	unsigned long offset;		
	if(SDPointer==0) {
		//*p='\0';
		strcpy(p,"EMPTYLINE");
		return;
	}	
	offset=(unsigned long)ProgNum*ProgramSize+SDPointer*LineLength;
	sd_raw_read(offset,tempbytes,LineLength);
        
	//bytecpy(p,tempbytes);
        //Serial.print(p);
}

void  get_Line2(char RealLineNumber) {
	char SDPointer=get_ln(RealLineNumber);
	unsigned long offset;		
	if(SDPointer==0) {
		//*p='\0';
		strcpy(OneLine,"EMPTYLINE");
                return;
	}	
	offset=(unsigned long)ProgNum*ProgramSize+SDPointer*LineLength;
	//sd_raw_read(offset,tempbytes,LineLength);
        //delay(1000);
        //Serial.print("\r\npr(offset,LineLength)=");
        //pr(offset,LineLength);
        //Serial.print("\r\nend");

        
        //char tmp[30];
        //sprintf(tmp,"\r\nRLN=%i,offset=%lu\r\n",RealLineNumber,offset);
        //Serial.print(tmp);

        //Serial.print("\r\npr(offset,LineLength)=");
        pr2(offset,LineLength);
        //Serial.print("\r\nend");

}


//Spinelli's Basic interpreter, ported to arduino bu Usmar A Padow usmpadow@gmail.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int *callp,callstk[10];
#define NumberOfNestedLoops 4
int forln[NumberOfNestedLoops],forend[NumberOfNestedLoops],for_itteration[NumberOfNestedLoops];
int var[100],CurrentLineNumber,quoted;
char buf[100],F[2];//size of buf might be reducible
//char *ln[100],*p,*q,*x,*y,*z,*s,*d,*f;
char *p,*q,*x,*y,*z,*s,*d,*f;


char *findstr(char *s,char *o) {
        for(x=s;*x;x++){
                for(y=x,z=o; *z&&*y==*z; y++)
                        z++;
                if(z>o&&!*z)
                        return x;
        }
        return 0;
}
void editln(){
        CurrentLineNumber=atoi(buf);
        //ln[l] && free(ln[l]);
        p=findstr(buf," ");
        if(findstr(buf," ")) {
              AddLine(CurrentLineNumber, p+1);
        }
}

int eqexp();
int atomexp();
int mulexp();
int addexp();
int cmpeexp();
int cmpexp();

int eqexp(){
        int o=cmpexp();
        switch(*p++){
                ;
                break;
        case '=':
                return o == eqexp();
                ;
                break;
        case '#':
                return o != eqexp();
        default:
                p--;
                return o;
        }
}
int debug(){
        char tmp[6];
        sprintf(tmp,"*p is %s,var[*p] is %d\r\n",*p,var[*p]);
        Serial.print(tmp);
        return 0;
}
    
int atomexp(){
        int o;
        return *p=='-'
                ? p++,-atomexp()
                : *p>='0' && *p<='9'
                        ? strtol(p,&p,0)
                        : *p=='('
                                ? p++,o=eqexp(),p++,o
                                : var[*p++];
}

int mulexp(){
        int o=atomexp();
        switch(*p++){
                ;
                break;
        case '*':
                return o * mulexp();
                ;
                break;
        case '/':
                return o / mulexp();
        default:
                p--;
                return o;
        }
}

int addexp(){
        int o=mulexp();
        switch(*p++){
                ;
                break;
        case '+':
                return o + addexp();
                ;
                break;
        case '-':
                return o - addexp();
        default:
                p--;
                return o;
        }
}

int cmpeexp(){
        int o=addexp();
        switch(*p++){
                ;
                break;
        case '$':
                return o <= cmpeexp();
                ;
                break;
        case '!':
                return o >= cmpeexp();
        default:
                p--;
                return o;
        }
}
int cmpexp(){
        int o=cmpeexp();
        switch(*p++){
                ;
                break;
        case '<':
                return o < cmpexp();
                ;
                break;
        case '>':
                return o > cmpexp();
        default:
                p--;
                return o;
        }
}

void puts2(char *s) {
       //Serial.print("puts2");
       Serial.print(s);
       //return 0;
}
void gets2(char *s) {
      int inbyte_type=0;
      char tmp[2];
      //strcpy(s,"");//clear string
      s[0]='\0';//initialize string
      while(inbyte_type!=13){//13 is enter
            if(Serial.available()) {
                inbyte_type=Serial.read();
                
                sprintf(tmp,"%c",inbyte_type);
                /*
                char tmp2[50];
                sprintf(tmp2,"tmp in gets2='%s'",tmp);
                Serial.print(tmp2);
*/                
                strcat(s,tmp);//add this byte_type
                if(inbyte_type!=13) Serial.print(tmp);//echo byte_type to terminal
            }

      }
      //how to flush serials //<Ragnorok> Read bytes and toss them out until no more are available          while(inbyte_type!=13){//13 is enter
      while(Serial.available()) {
                inbyte_type=Serial.read();
       }
 
      //Serial.print(s);//print string

}



int print_disk_info();
int sample();
int readDisk();

byte_type incomingbyte_type;
void printWelcome();
long int address;


void setup()
{
  
  Serial.begin(9600);
  delay(1000);

  printWelcome();
  if(!sd_raw_init())
  {
     Serial.println("MMC/SD initialization failed");      
  }
}


/*
void setup()
{
  
  Serial.begin(9600);
  delay(1000);

}

char buf[100];
*/
void loop() {
        NumberOfLines=get_nol();//get the LineCount for program 0
        while(1) {
                puts2("\n\rOk\n\r");
               //needs to be gets on PC and gets2 on arduino
               #ifdef __linux__
                     gets(buf);
               #else
                     gets2(buf);
               #endif
                if(buf[0]=='M') {//case 'M': // MEM  shows how much free memory is left
                        Serial.print("in loop(),freeMemory()=");
                        Serial.println(freeMemory());
                }
                switch(buf[0]) {
                case 'R': // RUN
                        callp=callstk;
                        for(i=0;i<100;i++) var[i]=0;//reinitializes all variables to 0
                        for(CurrentLineNumber=0;CurrentLineNumber<LineCount && CurrentLineNumber!=-1;CurrentLineNumber++) {
			      		get_Line2(CurrentLineNumber);
                        if(strcmp(OneLine,"EMPTYLINE")!=0) {//EXECUTE this line
                                      if(OneLine[0]=='P') {//if it is PRINT
                                            if(OneLine[6]=='"') {//if there is a quote mark it is a normal print statement
                                                  //*d=0; //puts2(buf+6);
                                                  int counter=7;
                                                  char tmp[2];
                                                  while(1){
                                                  if(OneLine[counter]=='"') break;//look for the ending quotes
                                                        counter++;
                                                  }
                                                  Serial.print("\n\r");
                                                  int counter2;
                                                  for(counter2=7;counter2<=counter-1;counter2++) {
                                                        //sprintf(tmp,"%c",s[counter2]);
                                                        tmp[0]=OneLine[counter2];
                                                        tmp[1]='\0';
                                                        Serial.print(tmp);
                                                  }      
                                            } else {// if there is not quote mark, it will print the content of the variable
                                                  char tmp[20];
                                                  p=buf+5;
                                                  p[1]='\0';
                                                  sprintf(tmp,"\r\n%i",var[OneLine[6]]);
                                                  Serial.print(tmp);
                                            }
                                      }
                                      if(OneLine[0]=='I' && OneLine[1]=='N') {//if it is INPUT
                                            //Serial.print("\r\nINPUT NUMBER:\r\n");
										    #ifdef __linux__
											 	 gets(buf);
										    #else
												 gets2(buf);
										    #endif
                                            p=buf;
                                            var[OneLine[6]]=eqexp();
                                      }
                                      /*
                                      } else {//greg why is this nessesary? (all characters in double quotes including the double quotes get put into buf, if they have a tab they get put into F
                                      		s=OneLine;
                                            while(*F=OneLine[0]){
                                                    *s=='"' && quoted++;
                                                    if(quoted&1 || !findstr(" \t",F))//what in the world does this line do?
                                                            *d++=*s;
                                                    s++;
                                            }
                                            *d--=quoted=0;
                                      } 
                                      */     
                                      if(OneLine[1]=='=') {//assignment
                                            if(OneLine[2]=='R' && OneLine[3]=='N'){//var=RND, you must set X to the minimum number first then Y to the maximum number: example 10 X=10 20 Y=20 30 N=RND  now N will contain a value between 10 and 20
                                                  #ifdef __linux__
                                                  		var[OneLine[0]]=rand2(var['X'], var['Y']);
                                                  #else
                                                  		var[OneLine[0]]=random(var['X'], var['Y']);
                                                  #endif
												  continue;                                                  
                                            } else {//just assign
                                                  p=OneLine+2, var[*OneLine]=eqexp();p=OneLine+2, var[*OneLine]=eqexp();
                                                  continue;
                                            }
                                      }
                                      switch(OneLine[0]) {
                                            case 'E':        // END 
                                                    CurrentLineNumber=-2;//must be -2 cause the for loop will increase it to -1
                                                    break;
                                            case 'I':        //IF exp THEN line-number 
                                                    if(OneLine[1]=='F') {
															//char OldStr[LineLength];
															strcpy(buf,OneLine);
                                                            (*(q=findstr(OneLine,"TH"))=0,
                                                                p=OneLine+2,
                                                                eqexp() && (p=q+4, CurrentLineNumber=eqexp()-1));
                                                                //printf("%i",CurrentLineNumber);
                                                            //p=OneLine+2,
                                                            //eqexp() && (p=q+4, CurrentLineNumber=eqexp()-1));
                                                            //printf("%i",eqexp());
                                                            
                                                            //ok, eqexp() is 1 if the expression is true and 0 if the expression is false, now I just haveto get the line number into CurrentLineNumber//printf("eqexp()=%i",eqexp());
															//p=OneLine+2;
															if(eqexp()) {
																q=findstr(buf,"THEN ");
																p=q+5;
																//printf("p=%i", eqexp());//this shows that eqexp returns an integer with the line number to goto
																CurrentLineNumber=eqexp()-1;
																continue;
															}
                                                    }
                                                    break;
                                            case 'G':        // GOTO|GOSUB
                                            /*
                                             * In the C and C++ programming languages, the comma operator (represented by the token ,) is a binary operator that evaluates its first operand and discards the result, and then evaluates the second operand and returns this value (and type). The comma operator has the lowest precedence of any C operator, and acts as a sequence point.
    The use of the comma token as an operator is distinct from its use in function calls and definitions, variable declarations, enum declarations, and similar constructs, where it acts as a separator.
    In this example, the differing behavior between the second and third lines is due to the comma operator having lower precedence than assignment.
    * http://en.wikipedia.org/wiki/Comma_operator */
    												/*
                                                    p=buf+4,//this ends up putting the value agter GOTO into p      what do single commas mean in C greg?
                                                    buf[2]=='S' && (*callp++=CurrentLineNumber, p++),//if GOSUB
                                                    CurrentLineNumber=eqexp()-1;
                                                    break;
                                                    */
                                                    /*
                                                    p=OneLine+4,//this ends up putting the value after GOTO into p,I think this is pointer arithmatic      what do single commas mean in C greg?
                                                    OneLine[2]=='S' && (*callp++=CurrentLineNumber, p++),//if GOSUB
                                                    CurrentLineNumber=eqexp()-1;
                                                    break;
                                                    */
                                                    if(OneLine[2]=='S') {//this is true is it is GoSub
                                                    	StackCount++;
                                                    	ReturnStack[StackCount]=CurrentLineNumber+1;
                                                    }
                                                   	sscanf(OneLine,"%*s %i",&i);//puts line number into i
													CurrentLineNumber=i-1;//must be -1 cause ther for loop will increase it to teh right number
                                                    break;
                                            case 'R':        // REM|RETURN
                                                    //OneLine[2]!='M' && (CurrentLineNumber=*--callp);
                                                    if(OneLine[2]!='M') {//if it it not ReM it is RETURN
                                                    	CurrentLineNumber=ReturnStack[StackCount]-1;//must be -1 cause the for loop will increase this number
                                                    	StackCount--;
                                                   	}
                                                    break;

                                            case 'F':        // FOR X=10 TO 15
                                                        char varname; int start; int end;
                                                        sscanf(OneLine,"FOR %c=%i TO %i",&varname,&start,&end);
                                                        //printf("FOR %c=%i TO %i\r\n",varname,start,end);
                                                        for_itteration[varname]=start;
                                                        forend[varname]=end;
                                                        forln[varname]=CurrentLineNumber;
                                                        var[varname]=start;
                                                        break;
                                            case 'N':        // NEXT
                                                        sscanf(OneLine,"NEXT %c",&varname);
                                                        if(for_itteration[varname]<forend[varname]) {//for loop has not ended
                                                                for_itteration[varname]++;//increment itteration number;
                                                                var[varname]=for_itteration[varname];
                                                                CurrentLineNumber=forln[varname];//goto beginning of for loop
                                                        }
                                                        break;
                                      }
                                      //CurrentLineNumber++;
                              } 
                     };
                      break;

                case 'D':        // DELETE current program
                	Serial.print("DELET PROGRAM Y/N?");
					#ifdef __linux__
						 gets(buf);
					#else
						 gets2(buf);
					#endif
					if(buf[0]=='Y') {
	                	Serial.print("DELETING...");                	
						delete_lns();
						delete_nol();
						Serial.print("DONE.");
						NumberOfLines=0;
					}
					break;
			    case 'O':        // OPEN (program number)
					ProgNum=buf[5]-'0';
			        NumberOfLines=get_nol();//get the number of lines for this program
			        break;
				case 'L':        // LIST
                      //Serial.print("LISTING"); 
                      //greg, there seems to be a small bug, where somehow line 0 appears with teh same value as ProgNum, howcome?
                	//char OneLine[LineLength];
                      Serial.print("\n\r");
                      sprintf(buf,"NumberOfLines:%i\r\n",NumberOfLines);
                      Serial.print(buf);
                      for(CurrentLineNumber=0;CurrentLineNumber<LineCount;CurrentLineNumber++) {
			    get_Line2(CurrentLineNumber);
                            //strcpy(OneLine,"TEST");//this works, so the rpoblem of the eternal loop must have something to do with get_Line
                            if(strcmp(OneLine,"EMPTYLINE")!=0) {
                                  //char tmp[100];
                                  sprintf(buf,"%d %s\r\n",CurrentLineNumber,OneLine);
                                  Serial.print(buf);
                            }
			}
                      break;
                case 0:
                default:// labeled line 
                        //Serial.print("edit line");
                        editln();
                }
        }
}



void printWelcome()
{
    Serial.print("------------------------------------------------\r\n");
    Serial.print("SD Card BASIC interpreter \r\n");
    Serial.print("by Usmar A Padow usmpadow@gmail.com (amigojapan)\r\n");
    Serial.print("Contributions by Greg Cox\r\n");
    Serial.print("You must write a program (UPPERCASE) and type RUN, \r\n");    
    Serial.print("------------------------------------------------\r\n");
}


//needed for the arudino simulator
#ifdef __linux__
	int main(void) {
			initArray();
			connect();
			setup();
			while(1){
					loop();
			}
	}
#endif
