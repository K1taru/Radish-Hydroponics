Skip to content

About Apps
Measure pH with a low-cost Arduino pH sensor board
2019-11-30in Arduino, Hydroponics, Sensor2778 Words
share tweet whatsapp

pH Sensor Board
I recently bought an Arduino pH sensor kit for measuring pH value of my hydroponic setup, it cheap but has very little information/document on how to use it, so I decided to figure it out myself on how it works and how to use it.

Popular pH measurement kits for Arduino
If you search for pH sensor with Arduino on Internet, you are likely see 3 major commercially available or mass-produced solutions:

Atlas Scientific offers high quality and well-designed sensor kit for pH measurement. It Gravity analog pH Kit consists of a consumer-grade pH sensor and interface board, plus 3 package of calibration buffer solutions will cost $65.00. Atlas Scientific hardware is high quality but doesn't seems to be open-sourced.

DFRobot also has a solution with the same name Gravity (why?) as Atlas Scientific. its version 1 Gravity: Analog pH Sensor Kit consists of pH probe plus the sensor board and is priced at $29.50. There is a version 2 of Gravity: Analog pH Sensor Kit which comes with the board with enhanced design at $39.50 by including buffer solutions and mounting screws for the board. DFRobot published its schematic, PCB layout and Arduino code for version 1 on its website and github under GPL2 license. But it only publish the PCB layout for version 2 without schematic, so I don't know what exactly was enhanced in the design for the version 2.

The third commonly available pH sensor kit for Arduino that you see almost in every e-commerce marketplaces such as Taobao, AliExpress and Amazon is this "mystery" pH sensor kit that I bought. You can find it at as low as $17.00 for a pH probe with the sensor board. It is "mystery" because it seems that there are multiple Chinese manufacturers producing the same board but I can't really find out which company actually own the design. I bought it anyway with the thinking that if I could understand how the pH probe works and with a little bit of "reverse-engineering" of the circuit design to help me to have better understanding of the circuitry, then I should be able to figure out on how to make it work. This fits my tinker spirit well...

Other than those three commonly available pH sensor kits, there are others available in the market, but they are relatively niche with limited distribution.

If you are interested on pH measurement or pH sensor board, you might to read further on A review on Seeed Studio pH and eC sensor kits - Part 1".

How pH probe work electronically?
A pH probe consists of two main parts: a glass electrode and a reference electrode as shown in the picture below. I'm not very good at chemistry, so I won't try to explain it that way, this pH theory guide provides very comprehensive explanation about the theory behind. In the nutshell, pH is determined essentially by measuring the voltage difference between these two electrodes.


Typical pH probe and characteristics
The pH probe is a passive sensor, which means no excitation voltage or current is required. It produces a voltage output that is linearly dependent upon the pH of the solution being measured. An ideal pH probe produces 0v output when pH value is at 7, and it produces a positive voltage (a few hundred mili-volts) when pH value go down, and a negative voltage level when pH value go up, causing by the hydrogen irons forming at the outside (and inside) of the membrane glass tip of the pH probe when the membrane comes into contact with solution. The source impedance of a pH probe is very high because the thin glass bulb has a large resistance that is typically in the range of 10 MΩ to 1000 MΩ. Whatever measurement circuit connect to the probe requires to be high-impedance in order to minimise the loading effect of the circuit.

Hardware - The pH sensor board explained
The pH sensor board that I bought came without any user guide, schematic or example code. I asked the small Chinese vendor for information but in vain. I decided to "reverse-engineering" the schematic diagram but eventually I find the schematic diagram at the attachment of this Arduino forum discussion. The pH sensor board can be divided into 3 different sections based on its functionality. I colour the three key sections with different color for discussion here.

ph sensor board functional highlights
pH sensor board functional highlights
pH Measurement Circuit
The light green section with the TLC4502 high-impedance operation amplifier basically consists of a voltage divider and a unity-gain amplifier. The pH output(Po) provided an analog output for pH measurement. As pH probe swing between positive and negative voltage, and since TLC4502 is operate with single power source, half of the TLC4502 is used as a voltage divider to provide a reference voltage of 2.5v to "float" the pH probe input so that the output of Po will be +/-2.5v based on pH value. A potentiometer RV1 is used for calibration purpose that I will further discuss later. This part of the circuit is well-designed and it is all it needed for measuring the pH value. The other parts of the board in my opinion are not well designed and sort of in the category of "nice-to-have" and not essential.

pH Threshold Detection Circuit
The yellow section provides a pH threshold detection/notification circuit. For example, you could adjust the potentiometer RV2 so that when pH level reach a threshold level (e.g. say 7.5), the RED LED D1 will be turned on (Digital output Do changed from high to low). Alternatively, you could use it to detect the lower pH level threshold, say, when pH value is below 5.5, the RED LED will be turned off and Do changes from low to high. But you can't set both lower and upper thresholds with this circuit. In my opinion, it will be easier to just use software solution than this hardware solution for threshold detection.

Temperature Reading Circuit
The light blue/cyan section of the board consists of 1 and a half LM358 OpAmp, and provides an analog reading at To. U2B of LM358 acts as a not so accurate voltage divider and provides a voltage reference of 2.5v to a Wheatstone bridge that consists of R13 - R15 and a thermistor TH1. The U3A behave as an differential OpAmp, the output is then pass through a low-pass filter and further amplified by a non-inverting OpAmp U3B. This entire circuit has nothing to do with pH measurement, at least not directly. I will talk about this toward the end of this article.

The sole reason for measuring temperature in the context of measuring pH value is because that pH curve slope changes when temperature change between 0 to 100 degree Celsius. It is therefore important to measure the temperature of the solution, and add temperature compensation factor into the pH calculation.

One thing interesting is that all the manufacturers for this board design that I saw in the market had the thermistor solder on the board instead of having a water-proof thermistor probe like the one that I described in my my previous post. By soldering thermistor on-board, that means the thermistor is measuring ambience temperature near the board instead of the temperature of the solution where pH was measured, this simply doesn't make sense. This makes me think that all those Chinese manufacturers are simply copying the design from a circuit diagram or reverse-engineering without understanding the purpose of having the thermistor for temperature measurement in the context of pH measurement application.

Now I studied and understand the circuit diagram, it is time to calibrate the pH sensor and write some code for measuring the pH value!

How to calibrate the pH sensor?
As discussed previously that by design the pH probe oscillates between negative and positive values. When the pH reading is at 7.0, the pH output is offset by 2.5v so that both negative and positive values generated by the pH probe can be represented as positive values in full range, this means that when pH is at 0, the Po would be at 0v, and when pH is at 14, the Po would be at 5v.

In order to make sure that when pH is at 7.0, we can calibrate the reading to make sure that Po will be at 2.5v by disconnecting the probe from the circuit and short-circuiting the inner pin of the BNC connector with the outer BNC ring. With a multimeter measure the value of Po pin and adjust the potentiometer to be 2.5V. Don't worry if you don't have a multimeter, you can write an Arduino sketch to read the analog input by connecting the Po to analog input A0 of the Arduino.

ph_calibrate.ino

#include <Arduino.h>
const int adcPin = A0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int adcValue = analogRead(adcPin);
  float phVoltage = (float)adcValue * 5.0 / 1024;
  Serial.print("ADC = "); Serial.print(adcValue);
  Serial.print(";  Po  = "); Serial.println(phVoltage, 3);
  delay(1000);
}
Connect Po to Aanalog input A0 on Arduino, and G to Arduino GND. Run the Arduino sketch, and open the Serial Monitor of Arduino IDE to observe the reading, slowly adjust the potentiometer RV1 (the one near the BNC connector on the board) until the Po reading equal to 2.50v.

This is assuming that all the pH probe are equal and will produce exactly 0v at pH reading of 7.0, but in reality all probes are slightly different from each other, especially for consumer-grade pH probe. Temperature also affect the reading of pH sensor slightly, so the better way is to use a pH buffer solution of pH=7.0 to calibrate the probe. All the buffer solution will have the temperature compensation information on its package that you could factor-in for your calibration.

ph buffer solution for calibration
pH buffer solution for calibration
pH buffer packages for calibration purpose available in liquid form or in powders form, liquid pack is easy to use but powders pack is good for storage. These solutions are sold in different values but the most common are pH 4.01, pH 6.86 and pH 9.18.

pH values are relatively linear over a certain range (between ph 2 to ph 10), we need two calibration points to determine the linear line, and then derives the slope of the line so that we could calculate any pH value with a given voltage output (see Figure 2 chart above). What value of pH buffer to use for this second calibration depends on your application, if your application is for measuring acidic solution, use buffer solution for ph=4.01 for the second calibration; buf if your application is mostly for measuing basic/alkanine solution, use buffer solution of ph=9.18 for the second calibration. In my case, as hydroponic for vegetable grow tends to be slightly acidic with ph ranging between 5.5 - 6.5, I use ph=4.01 buffer solution for my calibration.

To avoid cross contamination, dip the probe in distill water for a couple of minites before dipping it in different buffer solutions. For increase the accuracy, let the probe stay in the buffer solution for a couple of minutes before taking the reading as the result.

Use the same Arduino sketch to get the voltage reading for pH=4.01, and write down the voltage value, in my case, the voltage is 3.06 @ pH=4.01. The voltage readings at ph of 4.01 Vph4 and at pH of 7.0 Vph7 allows us to draw a straight line, and we can get the Voltage change per pH value m as:

m = (ph7 - ph4) / (Vph7 - Vph4) /
m = (7 - 4.01) / (2.5 - 3.05)
m = -5.436
So the pH value at any voltage reading at Po can be derived with this formula:

pH = pH7 - (Vph7 - Po) * m

i.e.

pH = 7 - (2.5 - Po) * m
Measure pH value
With the formula, we can create the Arduino sketch to measure the pH value based on the voltage reading at the Po.

#include <Arduino.h>
const int adcPin = A0;

// calculate your own m using ph_calibrate.ino
// When using the buffer solution of pH4 for calibration, m can be derived as:
// m = (pH7 - pH4) / (Vph7 - Vph4)
const float m = -5.436; 

void setup() {
  Serial.begin(115200);
}

void loop() {
  float Po = analogRead(adcPin) * 5.0 / 1024;
  float phValue = 7 - (2.5 - Po) * m;
  Serial.print("ph value = "); Serial.println(phValue);
  delay(5000);
}
How about Temperature Measurement?
As I mentioned before it doesn't make sense to measure the ambience temperature near the PCB, so the first thing I did is de-solder the on-board thermistor and replace it with one of those water-prove thermistors.

A Wheatstone Bridge circuit is nothing more than two simple series-parallel arrangements of resistances connected between a reference voltage supply and ground producing zero voltage difference between the two parallel branches when balanced. When one of the arm of the resistance arrangements consists of a thermistor, its resistance changes as temperature changed, causing the imbalance of two resistance arms and a voltage difference developed between the two parallel branches in according to the change of the thermistor resistance which is directly related to the change of the temperature.

Specifically to this circuit, the voltage reference is provided by U2B which formed a voltage divider and produce a reference voltage (let's called it Vref) of 2.5V at pin 7 of U2B. According to the characteristics of thermistor, the thermistor will have a resistance of 10k-ohms at temperature of 25 degree Celsius. The Wheatstone Bridge will be balanced and the output voltage Vd of the Wheatstone Bridge at the terminals of resistors R16 and R18 will be zero and will swing above and below 0 volt when temperature changes. The Vd is then amplified by U3A which seems to be a differential amplifier, the U3B is a typical non-inverting amplifier. As I'm not quite sure about the gain of U3A so I decided to ask at Electrical Engineering StackExchange, and I got my questions answered within an hour. The circuit has a total gain of 14.33 when the thermistor is at 10k (i.e. when temperature is at 25 degree Celsius). However, the gain of U3A will change when the thermistor resistance change, obviously this is not a very good design.

I also got confirmed my suspicion that there is a missing 20k resistor between between pin 3 of U3A and ground on the circuit diagram, interestingly the circuit board is designed to have this resistor, but where the resistor is supposed to be is left empty (why?). Further inspect the circuit I noticed that the R12 on the board is actually having a value of 51.1k instead of 100k as shown in the circuit diagram. So the over gain will be 1.33+5.11+1=7.44.

We can derive the Vd based on the measured voltage of To, and further derive the value of resistance of TH1 at the temperature where To is measured:

    Vd = To / 7.44

    Vd = Vref * (R14 / (R14 + R15)) - Vref * (R13 / (R13 + TH1))

Absolute temperature T based on Steinhart–Hart equation for thermistor calculation can then be derived from:

    T = 1 / (1/To + 1/B * ln(TH1/Ro))
Where:
T is the absolute temperature to be measured in Kelvin;
To is the reference temperature in Kelvin at 25 degree Celsius;
Ro is the thermistor resistance at To;
B is Beta or B parameter = 3950, provided by manufacturer in their specification.

In theory, the primary benefit of Wheatstone Bridge circuit is its ability to provide extremely accurate measurements in contrast with a simple voltage divider, as a voltage divider is often affected by the loading impedance of the measuring circuit. In actual application, the accuracy of the Wheatstone Bridge is highly depend on the precision of the resistors used to form the Wheatstone Bridge, the precision of voltage reference as well as the circuit that connected to the Wheatstone Bridge. Although I figured out the formula on how to measure the temperature, I did not write the code to calculate the temperature, as the gain of U3A will vary as the value of the thermistor varies in according to the temperature. This make the reading result almost unpredictable and I will probably not use this circuit for measuring the water temperature without further modifying the design.

In Summary
Overall, this pH sensor board has a good pH measurement circuit design, the rest parts of the circuit are quite useless and a little bit over-engineered. By eliminating the bad part of the circuit design and kept the good part, it could be simpler and maybe slightly cheaper than current design for a pH sensor board.

Related topic:
A review on Seeed Studio pH and EC sensor kits - Part 1(PH)".
A review on Seeed Studio pH and EC sensor kits - Part 2(EC).

Post navigation
Build an ESP32 Web Server and IR RemoteGetting started with STM32 and things you need to be aware of
29 comments by readers
Huysonyoung Akihabara
2020-02-20 at 11:30 pm
Dear Sir!
It’s really helpful topic. Thank you so much for your sharing. I have been making 1 pH meter kit since sep-2019 up to now, so I have 1 problem;

I tested with pH7.01 buffer solution, it show me pH = 7.04 at 2.51V; and with pH4.01 buffer solution I got pH = 3.98 at 3.02V.

Now I take 1 liter waste water sample from the collection pit (Wastewater system) then tested, it displayed pH = 7.62 at 2.49V ….. but I droped the pH probe to test directly with waste water treatment system then it showed pH = – 6.68 at 4.83V.

Can you explain for me why the “Po” voltage out so high up from 2.49V to 4.83V with the same water sample?

Thank you so much,

Henry Cheung
2020-02-21 at 2:42 pm
I don’t know what caused it, but ph=-6.68 at 4.83v is basically out of the linear range that the probe could accurately measured, or your probe is not connected. Also, don’t measure pH on running flow as the value will not be accurate, and if you have a TDS probe, put it away from pH probe.

John Smith
2020-06-01 at 11:05 am
Thanks you for the nice article. I would like to add the following.
When the pH sensor module is used with a micro controller like Arduino, calculations and calibrations can be simplified by eliminating the voltage calculation. When calibrating with 7.0 pH solution, float phVoltage = (float)adcValue * 5.0 / 1024 = 2.5 only if adcValue = 512 so RV1 can be tuned to that.
The slope

m = (ph7 - ph4) / (adc7Value - adc4Value)
can be used in equation

pH = pH7 - (adc7Value - adcValue) * m = 7 - (512 - adcValue) * m
I think voltage calculation is useful when someone designs a circuit without using software and in which case, the signal from ‘Do’ can be used to trigger a device like a relay to turn something on/off. Depending on the circuit, active-high or active-low relay can be used.

Henry Cheung
2020-06-02 at 11:03 am
Thanks for the comment.

On ‘Do’, it has Hysteresis effect, meaning if you expect it to trigger something when a value is reach to a certain point, it does not necessary will trigger at the same point when the the value falling back. So personally, I prefer to use software solution than the ‘Do’.

John Smith
2020-06-03 at 2:09 pm
I too prefer the software approach.
Many of these boards are based on expired patents, that’s why they are ‘low-cost’. Which means they are old and back in the days, software were not widely used. Most circuits were designed and run based on hardware. I could think of a few applications that might utilize the function. Of course, accuracy can be achieved, not at the speed we are accustomed to but enough to satisfy the needs.

dan
2020-06-18 at 6:16 am
Hi,
great article.
The ph changes with temperature. how would we apply temp compensation to the ph readings.

Thanks

Henry Cheung
2020-06-21 at 12:40 pm
The PH variation due to temperature is less significant than EC measurement, for example, ph of 4.00 at 25 degree C will be around 4.01 @30 C and increase by 0.01 for approx every 5 degrees temperature change, but it remains at about 4.00 in the range between 10 – 25 C. So I will simply ignore the temperature factor in ph measurement unless you measuring ph about 30 degree C or below 10 degree C. I won’t trust the temperature measurement circuit for this particular board for the reason that I mentioned in my article. If you really want to have temperature measurement, get a water-proof DS18B20.

Hunter Lu
2020-07-26 at 7:26 pm
Hi, there

Check out below linked pdf file that gives a formular on how to apply temp compensation.
https://www.hach.com/quick.search-download.search.jsa?keywords=pH

Chan
2020-06-22 at 3:18 am
Hi Henry,
I am not an expert but I want to build a smart PH and EC sensor which helps me regulate the water nutrients and PH levels for an indoor hydroponics system.
Can you guide me on an economical way to achieve this?
Regards
Chan

Hunter Lu
2020-07-26 at 7:22 pm
Hi, There

Thanks for the post and discussion, I too duplicated the whole circuit for PH measurement, as well as adding water-proofing DS18B20 and EC sensor on PCB, in which I planned to build an eco measuring system for in door plant growing measurement. Everything worked fine but I got two problems:
1. Big variations of PH value output measured alone without DS18B20 and EC sensor, not sure why, since I don’t have oscillator, couldn’t identify where the variation coming from, maybe you could help with it.
2. All 3 sensors putting together, DS18B20 and EC sensors working fine, but PH sensor got significant interference by DS18B20 and EC sensor, which seems to be reasonable(each of them emitted potential into solution with impact the micro-voltage level PH Sensor. I will have to isolate/disconnect DS18B20 and EC sensor through Arduino software and hardware switch circuit.

The calibration of PH sensor went well, which I used standard buffer resolution, output the data exactly as expected, but when I put it into a glass with tap water, it gave big variation, as I mentioned above…

Would appreciate if anyone could help me out.

Henry Cheung
2020-07-26 at 8:29 pm
The things that I could think of 1) PH sensor doesn’t work well with running water or in a current stream. 2) Which MCU you are using? Arduino? I would suggest that you add a 10uF and 100nF capacitors in parallel at PH sensor board’s Vcc and Ground. 3) If you are using a PH sensor board other than this design, make sure the output impedance of the board that feed into ADC of Arduino is less than 10k ohm, you can read about my recent experience on evaluation Seeed Studio’s pH sensor board for the problem that I encountered.

I don’t have any issue of having DS18B20 temperature sensor together with PH sensor, actually in my setup, my PH sensor is right next to the DS18B20 with less than 1cm separation, DS18B20 should stay away from EC sensor as EC sensor generated a magnetic/electrode field around its tip.

Hunter Lu
2020-07-26 at 10:59 pm
Thanks for the quick response, Henry.

In my view, DS18B20 and EC sensor once power on having direct impact to PH sensor. As confirmed by your comment, I can understand the EC sensor having impact to PH sensor, DS18B20 shouldn’t have impact to PH sensor, I don’t understand this part, need to figure out. Another alternative is to use water-proof thermal resistor B 3950 with screwnut for easy installation.

I am actually making a board with all components on including PH, EC sensor circuits, with ATMEGA328P MCU simulating as Arduino Nano. To add 10uF and 100nF to VCC of the Amplifier CD4052 is agood idea, somehow I missed it. The layout needs to be improve to have the CD4052 closer to PH BNC slot.

I haven’t experienced the ADC issue with impedence problem, will check out your blog later for reference.

Great comments, thanks Henry.

mohamad
2020-10-17 at 1:46 pm
I mentioned in my research that the temperature sensor is being removed and I installed another sensor
Can the sensor be left in place and the measured temperature adjusted?
What is the effect of temperature on the pH value?

Henry Cheung
2020-10-18 at 8:34 am
On pH dependency on temperature, you should consult your probe manufacturer’s user guide. In general, for instance at 0 °C the pH of pure water is about 7.47. At 25 °C it is 7.00, and at 100 °C it is 6.14. For practical application, it depend on where is your region, I lives in tropical region, and see my answer to question 3 above.

Gennadiy
2021-04-11 at 4:52 am
Hi Henry!
First of all, i want to say that its very very useful information – really apreciate it.
I making final year project – automatic control system for hydroponics.
As you wrote at the beginning of the post, there is very little – almost none information about how to calibrate properly these sensors. Your post is very informative, but still i couldn’t figure out the point about temperature compensation.
Can you please clearify it? From what i gave understood, is that you didn’t wrote any code for temp. compensation due to the fact that the built in thermistor is useless- beacuase it measures ambient temp. (not in water).
I do want to make precise calibration on my EC Sensor, and i got submersible DS18B20 sensor.
Should i left the T1 (Temperature out) pin on the EC sensor unconnected? Or if i don’t want to use it – connect to GND?
Any help is appreciated.

Henry Cheung
2021-04-11 at 8:14 pm
I gave a few reasons on why I didn’t do temperature compensation on pH measurements in the comments, you can scroll up and read it. For the EC, I’m not sure what EC sensor you are using, so I can’t really comment on how it should be connected, in general, if you don’t use it, leave it open should be fine. You can also read my another post which uses a water-proof DS18B20 with an EC sensor.

Steve
2021-04-26 at 2:40 pm
Hi Henry,
I found your article really very interesting.
I followed all the steps in your article, I realized that when I short the BNC signal to make the 2.5 volt adjustment, the maximum range I can get by adjusting the trimmer is 2.4- 4.99 Volts. My board does not go below 2.4 volts.
Do you think it is defective?

Jean-Philippe
2021-05-19 at 4:38 am
Hello,
I have the same issue while calibrating the minimum I can get it 2.6 volts, I can’t event reach 2.5…
For me the range is 2.6-4.99

Dave
2021-10-23 at 9:33 pm
You can do what i did, while calibrating the minimun I only was able to get 2.59V, to be able to get get proper readings of pH value i just get left the bnc potentiometer at the minimum possible and got the readings of the powder solutions: 6.86 and 4.01.
And with this using the equation y = ax+b i could get the correct values of pH.
Ex:
pH = a*xV + b
4.01 = a*(3.04)+b
6.86 = a*(2.54)+b
Then i get the result y = -5.7*21.338 and thats it.

Jon Wong
2021-10-04 at 9:14 am
Hi,
I have the same problem! When I shortened the BNC to make the 2.5 volt adjustment, the ADC value only goes down to 831 and not 512? I can’t adjust RV1 further to get to 2.5 volts! Do you think the board is defective? Thanks for your help!

Saitama
2021-08-26 at 9:51 am
Hi, when trying to calibrate the sensor I get an incorrect voltage from PO when trying to adjust it with the (BOATER 3296) potentiometer. I try to read it from the PO pin but it just reads ADC = 872; Po = 4.262V. The actual readings can be measured at the external part of the BNC probe or at the potentiometer. They show the correct voltage that I’ve tuned to 2.5V for the 7 Ph baseline. Is this a fault with the BNC interface?

I am running this code to measure the voltage at PO, the potentiometer and the BNC connector

#include
const int adcPin = A0;

void setup() {
Serial.begin(115200);
}

void loop() {
int adcValue = analogRead(adcPin);
float phVoltage = (float)adcValue * 5.0 / 1023;
Serial.print(“ADC = “); Serial.print(adcValue);
Serial.print(“; Po = “); Serial.println(phVoltage);
delay(1000);

}

JM
2022-10-15 at 3:11 am
Conseguiste que te funcionara? Que hiciste al respecto? Conclusion?

dekip
2021-11-05 at 7:05 pm
As far as I can see, there are no data of C values of the schematic. Did any of you know what the values are?

vehan
2022-10-17 at 9:08 pm
Did you ever get a response to this? I am also looking for this

MikeK
2022-09-28 at 1:27 am
You made the post i was looking for. Its perfect. Thank you!!!

Falah hasbi
2023-10-29 at 6:59 pm
Great article,
I just followed your tutorial, but i have a problem with my adc value not change (in my serial monitor 1024). By the way, i’m using esp8266. Anyone can help me to fix this problem?

Henry Cheung
2023-11-01 at 5:25 pm
ESP8266 ADC by default has input range from 0-1.0v, depend on the module or board you are using, if it doesn’t have the voltage divider setup to allow higher voltage input, it will read as 1023 for any voltage higher than 1.0v. get to know your board better by checking the schematic, or apply a 0.5v input to see what value you get (without voltage divider, it will read as 512@0.5v).

Max
2024-10-18 at 10:14 am
For my NodeMcu the Voltage was 3.3V to build the divider

umer ahmed
2023-12-10 at 3:30 pm
i have connected the wire then connect the with laptop, i am getting 0.0 and 0.24 and values like that repeatedly after the gap with 0.00 so what would be the probable error, and one more thing by mistake i change the second screw then what i can do to correct it.

Comments are closed.

E-tinkers.com | Copyright © 2026