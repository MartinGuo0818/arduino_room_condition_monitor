#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_SSD1306.h>
#include <afstandssensor.h>
#include <Adafruit_Si7021.h>
double duration, distance_mm,t,parameter,humi,light_converted;
const uint8_t photoresistor=15,micpin=14,buzzer=25,led=32,CHANNEL_BUZZER =
1,CHANNEL_LED = 2;
uint16_t interval = 1000;
int Presis,Lsound;
double avg_temp=0,avg_humi=0,avg_sound=0,avg_light=0,avg_gas=0;
int n_times=0;
double mark_sleeping,mark_working;
double
light_normalized_s=0,noise_normalized_s=0,humid_normalized_s=0,temp_normaliz
ed_s=0;
double
light_normalized_w=0,noise_normalized_w=0,humid_normalized_w=0,temp_normal
ized_w=0;
Adafruit_SSD1306 display(128, 64, &Wire, -1); //128x64 OLED Display - Using
default I2C - No reset pin (-1)
Adafruit_ADS1015 ads; //4-Channel Analog to Digital
Converter 10-bit resolution
AfstandsSensor hcsr04(13, 27); //HC-SR04 Ultrasonic Distance
25 / 32
Sensor - Trigger pin connected to IO13 - Echo pin connected to IO27
Adafruit_Si7021 si7021 = Adafruit_Si7021();
void setup() {
 Serial.begin(115200);
 pinMode(photoresistor,INPUT);
 pinMode(buzzer, OUTPUT);
 pinMode(led, OUTPUT);
 pinMode(micpin,INPUT);
 ledcSetup(CHANNEL_BUZZER,1600, 8);
 ledcSetup(CHANNEL_LED, 3200, 8);
 ledcAttachPin(buzzer, CHANNEL_BUZZER);
 ledcAttachPin(led, CHANNEL_LED);
 ledcWrite(CHANNEL_BUZZER, 0);
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Initialize OLED - Use
internal power source - I2C address = 0x3C
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
}
void loop() {
 for (int i=0; i < 10; i++) {
 avg_temp+=si7021.readTemperature();
 avg_humi+=humi=si7021.readHumidity();
 avg_light+=analogRead(photoresistor);
 avg_sound+=analogRead(micpin);
 }
 //average filtering takes ten reading from the sensors and take the average value
 avg_temp=avg_temp/10;
 avg_humi=avg_humi/10;
 avg_light=avg_light/10;
 avg_sound=avg_sound/10;
26 / 32
 //calibration calculation
 avg_temp=0.9054*avg_temp-0.0379;
 avg_humi=0.9221*avg_humi+3.9258;
 light_converted=(8.59*(double)pow(10,-5))*sq(avg_light)+26.6;
 light_normalized_w=1.0-sq((light_converted-1000.0)/500);
 light_normalized_s=1.0-sq(light_converted/500.0);
 noise_normalized_w=1.0-sq(avg_sound/49.0);
 noise_normalized_s=1.0-sq(avg_sound/42.0);
 //humid_normalized_w=1.0-sq((avg_humi-55.0)/15.0);
 if (avg_humi<40.0){
 humid_normalized_w=(avg_humi/40.0);
 }else if(40>=avg_humi<70.0){
 humid_normalized_w=1.0;
 }else{
 humid_normalized_w=-(avg_humi/30.0)+100.0/30.0;
 }
 if(avg_humi<64.0){
 humid_normalized_s=(avg_humi/64.0);
 }else if(avg_humi>=64.0){
 humid_normalized_s=-(avg_humi/36.0)+100.0/36.0;
 }
 //humid_normalized_s=1.0-sq((avg_humi-64.0)/15.0);
 temp_normalized_w=1.0-sq((avg_temp-23.0)/3.0);
 temp_normalized_s=1.0-sq((avg_temp-24.8)/3.0);

mark_sleeping=light_normalized_s+noise_normalized_s+temp_normalized_s+humi
d_normalized_s;

mark_working=light_normalized_w+noise_normalized_w+temp_normalized_w+hum
27 / 32
id_normalized_w;
 n_times+=1;
 if (n_times==10) {
 //display.clearDisplay();
 //display.setCursor(0,0);
 //display.print("T(degC):"); display.print(avg_temp,0);display.print(",");
 //display.println("Humidity:"); display.print(avg_humi,0);
 //display.println("Light
(LUX):");display.print(light_converted,0);display.print(",");
 //display.println("Sound:");display.print(avg_sound,0);

Serial.println("==========================================");
 Serial.print("T(degC):"); Serial.println(avg_temp);
 Serial.print("T_normalized_working:");Serial.println(temp_normalized_w);
 Serial.print("T_normalized_sleeping:");Serial.println(temp_normalized_s);
 Serial.print("Humidity(%):"); Serial.println(avg_humi);

Serial.print("Humidity_normalized_working:");Serial.println(humid_normalized_w);

Serial.print("Humidity_normalized_sleeping:");Serial.println(humid_normalized_s);
 Serial.print("Light (LUX):");Serial.println(light_converted);
 Serial.print("Light_normalized_working:");Serial.println(light_normalized_w);
 Serial.print("Light_normalized_sleeping:");Serial.println(light_normalized_s);
 Serial.print("Sound:");Serial.println(avg_sound);
 Serial.print("Sound_normalized_working:");Serial.println(noise_normalized_w);
 Serial.print("Sound_normalized_sleeping:");Serial.println(noise_normalized_s);
 Serial.print("Working Mark:");Serial.println((double)mark_working);
 Serial.print("Sleeping Mark:");Serial.println((double)mark_sleeping);
 //ledcWrite(CHANNEL_BUZZER, 64);
 ledcWrite(CHANNEL_LED, 128);
28 / 32
 delay(500); //delay(ms)
 ledcWrite(CHANNEL_BUZZER, 0);
 ledcWrite(CHANNEL_LED, 0);
 display.clearDisplay();
 display.setCursor(0,0);
 display.println();
 display.print("T:"); display.print(avg_temp);display.print(',');
 //display.println();
 display.print("RH:"); display.print(avg_humi); display.print('%');
 display.println();
 display.print("Illuminance:");display.print(light_converted);
 display.println();
 display.print("Working Mark:");display.print((double)mark_working);
 display.println();
 display.print("Sleeping Mark:");display.print((double)mark_sleeping);
 display.println();
 display.print("Current envirnoment");
 display.println();
 display.print("is more suitable for:");
 display.println();
 if (mark_working>mark_sleeping & mark_working>-4){
 display.print("working");
 }
 else if (mark_sleeping>mark_working & mark_sleeping>-4){
 display.print("sleeping");
 }else{
 display.print("None");
 }
 display.display();
 n_times=0;
29 / 32
 }
 avg_temp=0;temp_normalized_s=0;temp_normalized_w=0;
 avg_humi=0;humid_normalized_s=0;humid_normalized_w=0;
 avg_light=0;light_normalized_s=0;light_normalized_w=0;
 avg_sound=0;noise_normalized_s=0;noise_normalized_w=0;
 light_converted=0;
 mark_sleeping=0;mark_working=0;
 delay(1000);
}
