  
    /*
    Arduino UNO R4 DAC Speed Test
    --------------------------------
    This sketch is designed to test the speed of the DAC (Digital-to-Analog Converter) 
    and ADC (Analog-to-Digital Converter) on the Arduino UNO R4.
  
    Setup:
    - Connect a jumper wire from A0 to A1.
    - Connect a 4.7k resistor from A1 to ground.
  
    This sketch measures the speed and resolution of the ADC by reading analog values 
    and converting them to digital values. The results are printed to the Serial Monitor.
  
    ADC Resolution:
    - 8 bits: Useful for general, less precise measurements.
    - 10-14 bits: Common for most applications, balancing precision and speed.
    - 16 bits: High precision for detailed signal analysis.
    - 24 bits: For highly sensitive applications, usually with external ADCs.
  
    Voltage per step for different resolutions:
    - 2-bit resolution: 5V / 4 ≈ 1.25V per step.
    - 4-bit resolution: 5V / 16 ≈ 0.3125V per step.
    - 6-bit resolution: 5V / 64 ≈ 0.0781V per step.
    - 8-bit resolution: 5V / 256 ≈ 0.0195V per step.
    - 10-bit resolution: 5V / 1024 ≈ 0.0049V per step.
    - 12-bit resolution: 5V / 4096 ≈ 0.0012V per step.
    - 14-bit resolution: 5V / 16384 ≈ 0.0003V per step.
    - 16-bit resolution: 5V / 65536 ≈ 0.0001V per step.
    - 24-bit resolution: 5V / 16777216 ≈ 0.0000003V per step.
  
    For more information, visit:
    https://forum.arduino.cc/t/arduino-uno-r4-dac-speed/1177741
    https://forum.arduino.cc/t/uno-r4-analogread-code-where-is-it/1145909/4

  Example Output:
  355Reading: 1866 -> 2.2784v at Time: 16519 - 16566 = 47 microseconds
  356Reading: 1900 -> 2.3199v at Time: 16566 - 16612 = 46 microseconds
  357Reading: 1937 -> 2.3651v at Time: 16612 - 16658 = 46 microseconds
  358Reading: 1973 -> 2.4090v at Time: 16658 - 16704 = 46 microseconds
  Freq set to 60hz
  Resolution set to 12-bit
  Number of Samples 360
  Sample Delay set to 11us
  Total Time 16658us ~ 60.03 Hz


Sketch uses 64180 bytes (24%) of program storage space. Maximum is 262144 bytes.
Global variables use 9652 bytes (29%) of dynamic memory, leaving 23116 bytes for local variables. Maximum is 32768 bytes.  



1 cycle of 60hz = 16667us
16667us / 360 samples = 46.3us per sample
11us delay per sample = 11us * 360 samples = 3,960us
16,667us - 3,960us = 12,707us
12,707us / 360 samples = 35.3us per sample

60hz is 16.67ms per cycle
with a delay of 230 the frequency is ~ 10hz
with a delay of 1 the frequency is ~ 78.11hz
with a delay of 11 the frequency is ~ 60.03hz

ADC takes 23 to 24 microseconds per read with no delay
DAC12 conversion time is 30uS to reach specified accuracy... although the 'scope shows this as being faster.
https://forum.arduino.cc/t/arduino-uno-r4-dac-speed/1177741

We are using a 12-bit ADC and DAC, so the resolution is 4096 steps (0-4095)
The DAC is outputting a 60hz sine wave with 360 samples per cycle
The ADC is reading the sine wave and storing the values in an array
The time delay between each sample is 11us, which is also the delay between each analogWrite and analogRead
The total time for one cycle of the sine wave is 16658us, which is approximately 60.03hz
The ADC readings are averaging around 23 to 24 microseconds per read with no delay
The DAC12 conversion time is 30uS to reach specified accuracy... although the 'scope shows this as being faster.
46.3us - 23us = 23.3us per sample
23.3us per sample - 11us delay = 12.3us per sample for DAC
DAC analogWrite takes ~ 10uS for the code and 5uS (max) for the DAC output to change. 
12.3us per sample for DAC + 23us per sample for ADC + 11us delay = 46.3us per sample
46.3us per sample * 360 samples = 16,668us per cycle
16,668us per cycle = 60.03hz


  */




//https://deepbluembedded.com/sine-lookup-table-generator-calculator/
//const uint16_t sineLookupTable[] = { 0x800, 0x824, 0x847, 0x86b, 0x88f, 0x8b2, 0x8d6, 0x8fa, 0x91d, 0x940, 0x964, 0x987, 0x9aa, 0x9cd, 0x9ef, 0xa12, 0xa35, 0xa57, 0xa79, 0xa9b, 0xabc, 0xade, 0xaff, 0xb20, 0xb41, 0xb62, 0xb82, 0xba2, 0xbc1, 0xbe1, 0xc00, 0xc1f, 0xc3d, 0xc5b, 0xc79, 0xc97, 0xcb4, 0xcd1, 0xced, 0xd09, 0xd24, 0xd40, 0xd5a, 0xd75, 0xd8f, 0xda8, 0xdc1, 0xdda, 0xdf2, 0xe0a, 0xe21, 0xe38, 0xe4e, 0xe64, 0xe79, 0xe8e, 0xea2, 0xeb6, 0xec9, 0xedb, 0xeee, 0xeff, 0xf10, 0xf21, 0xf31, 0xf40, 0xf4f, 0xf5d, 0xf6b, 0xf78, 0xf84, 0xf90, 0xf9c, 0xfa7, 0xfb1, 0xfba, 0xfc3, 0xfcc, 0xfd3, 0xfda, 0xfe1, 0xfe7, 0xfec, 0xff1, 0xff5, 0xff8, 0xffb, 0xffd, 0xfff, 0x1000, 0x1000, 0x1000, 0xfff, 0xffd, 0xffb, 0xff8, 0xff5, 0xff1, 0xfec, 0xfe7, 0xfe1, 0xfda, 0xfd3, 0xfcc, 0xfc3, 0xfba, 0xfb1, 0xfa7, 0xf9c, 0xf90, 0xf84, 0xf78, 0xf6b, 0xf5d, 0xf4f, 0xf40, 0xf31, 0xf21, 0xf10, 0xeff, 0xeee, 0xedb, 0xec9, 0xeb6, 0xea2, 0xe8e, 0xe79, 0xe64, 0xe4e, 0xe38, 0xe21, 0xe0a, 0xdf2, 0xdda, 0xdc1, 0xda8, 0xd8f, 0xd75, 0xd5a, 0xd40, 0xd24, 0xd09, 0xced, 0xcd1, 0xcb4, 0xc97, 0xc79, 0xc5b, 0xc3d, 0xc1f, 0xc00, 0xbe1, 0xbc1, 0xba2, 0xb82, 0xb62, 0xb41, 0xb20, 0xaff, 0xade, 0xabc, 0xa9b, 0xa79, 0xa57, 0xa35, 0xa12, 0x9ef, 0x9cd, 0x9aa, 0x987, 0x964, 0x940, 0x91d, 0x8fa, 0x8d6, 0x8b2, 0x88f, 0x86b, 0x847, 0x824, 0x800, 0x7dc, 0x7b9, 0x795, 0x771, 0x74e, 0x72a, 0x706, 0x6e3, 0x6c0, 0x69c, 0x679, 0x656, 0x633, 0x611, 0x5ee, 0x5cb, 0x5a9, 0x587, 0x565, 0x544, 0x522, 0x501, 0x4e0, 0x4bf, 0x49e, 0x47e, 0x45e, 0x43f, 0x41f, 0x400, 0x3e1, 0x3c3, 0x3a5, 0x387, 0x369, 0x34c, 0x32f, 0x313, 0x2f7, 0x2dc, 0x2c0, 0x2a6, 0x28b, 0x271, 0x258, 0x23f, 0x226, 0x20e, 0x1f6, 0x1df, 0x1c8, 0x1b2, 0x19c, 0x187, 0x172, 0x15e, 0x14a, 0x137, 0x125, 0x112, 0x101, 0xf0, 0xdf, 0xcf, 0xc0, 0xb1, 0xa3, 0x95, 0x88, 0x7c, 0x70, 0x64, 0x59, 0x4f, 0x46, 0x3d, 0x34, 0x2d, 0x26, 0x1f, 0x19, 0x14, 0x0f, 0x0b, 0x08, 0x05, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x05, 0x08, 0x0b, 0x0f, 0x14, 0x19, 0x1f, 0x26, 0x2d, 0x34, 0x3d, 0x46, 0x4f, 0x59, 0x64, 0x70, 0x7c, 0x88, 0x95, 0xa3, 0xb1, 0xc0, 0xcf, 0xdf, 0xf0, 0x101, 0x112, 0x125, 0x137, 0x14a, 0x15e, 0x172, 0x187, 0x19c, 0x1b2, 0x1c8, 0x1df, 0x1f6, 0x20e, 0x226, 0x23f, 0x258, 0x271, 0x28b, 0x2a6, 0x2c0, 0x2dc, 0x2f7, 0x313, 0x32f, 0x34c, 0x369, 0x387, 0x3a5, 0x3c3, 0x3e1, 0x400, 0x41f, 0x43f, 0x45e, 0x47e, 0x49e, 0x4bf, 0x4e0, 0x501, 0x522, 0x544, 0x565, 0x587, 0x5a9, 0x5cb, 0x5ee, 0x611, 0x633, 0x656, 0x679, 0x69c, 0x6c0, 0x6e3, 0x706, 0x72a, 0x74e, 0x771, 0x795, 0x7b9, 0x7dc};
const uint16_t sineLookupTable[] = {2048, 2084, 2119, 2155, 2191, 2226, 2262, 2298,2333, 2368, 2404, 2439, 2474, 2509, 2543, 2578,2613, 2647, 2681, 2715, 2748, 2782, 2815, 2848,2881, 2914, 2946, 2978, 3009, 3041, 3072, 3103,3133, 3163, 3193, 3223, 3252, 3281, 3309, 3337,3364, 3392, 3418, 3445, 3471, 3496, 3521, 3546,3570, 3594, 3617, 3640, 3662, 3684, 3705, 3726,3746, 3766, 3785, 3803, 3822, 3839, 3856, 3873,3889, 3904, 3919, 3933, 3947, 3960, 3972, 3984,3996, 4007, 4017, 4026, 4035, 4044, 4051, 4058,4065, 4071, 4076, 4081, 4085, 4088, 4091, 4093,4095, 4096, 4096, 4096, 4095, 4093, 4091, 4088,4085, 4081, 4076, 4071, 4065, 4058, 4051, 4044,4035, 4026, 4017, 4007, 3996, 3984, 3972, 3960,3947, 3933, 3919, 3904, 3889, 3873, 3856, 3839,3822, 3803, 3785, 3766, 3746, 3726, 3705, 3684,3662, 3640, 3617, 3594, 3570, 3546, 3521, 3496,3471, 3445, 3418, 3392, 3364, 3337, 3309, 3281,3252, 3223, 3193, 3163, 3133, 3103, 3072, 3041,3009, 2978, 2946, 2914, 2881, 2848, 2815, 2782,2748, 2715, 2681, 2647, 2613, 2578, 2543, 2509,2474, 2439, 2404, 2368, 2333, 2298, 2262, 2226,2191, 2155, 2119, 2084, 2048, 2012, 1977, 1941,1905, 1870, 1834, 1798, 1763, 1728, 1692, 1657,1622, 1587, 1553, 1518, 1483, 1449, 1415, 1381,1348, 1314, 1281, 1248, 1215, 1182, 1150, 1118,1087, 1055, 1024, 993, 963, 933, 903, 873,844, 815, 787, 759, 732, 704, 678, 651,625, 600, 575, 550, 526, 502, 479, 456,434, 412, 391, 370, 350, 330, 311, 293,274, 257, 240, 223, 207, 192, 177, 163,149, 136, 124, 112, 100, 89, 79, 70,61, 52, 45, 38, 31, 25, 20, 15,11, 8, 5, 3, 1, 0, 0, 0,1, 3, 5, 8, 11, 15, 20, 25,31, 38, 45, 52, 61, 70, 79, 89,100, 112, 124, 136, 149, 163, 177, 192,207, 223, 240, 257, 274, 293, 311, 330,350, 370, 391, 412, 434, 456, 479, 502,526, 550, 575, 600, 625, 651, 678, 704,732, 759, 787, 815, 844, 873, 903, 933,963, 993, 1024, 1055, 1087, 1118, 1150, 1182,1215, 1248, 1281, 1314, 1348, 1381, 1415, 1449,1483, 1518, 1553, 1587, 1622, 1657, 1692, 1728,1763, 1798, 1834, 1870, 1905, 1941, 1977, 2012};
#define NUM_SAMPLES 360
//const float amplitude = 2048; // Half of the full scale for a 12-bit DAC (0-4095) 
int freq = 60; // Frequency in Hz 
int readings[NUM_SAMPLES];
unsigned long timestamps[NUM_SAMPLES];

//Set the ADC & DAC resolution
int adcResolution = 12;
float bitresolution = pow(2, adcResolution) - 1;
int plotter = false;



//DAC timing before ADC read
 // Approximate delay for sampling at (1/Hz s =  µs)
 //unsigned long delayTime = 10 / freq; // Calculate delay for sampling at desired frequency
 //unsigned long samplePeriod = 1000000 / (freq * NUM_SAMPLES); // Calculate delay for 10Hz signal with NUM_SAMPLES samples per period
 //unsigned long samplePeriod = (1.000000000/freq)/NUM_SAMPLES+25;
 //unsigned long samplePeriod = (1.000000000/freq)*1000000; // Approximate delay for sampling at 600 Hz (1/600 s ≈ 1667 µs) } 
 //DAC analogWrite takes ~ 10uS for the code and 5uS (max) for the DAC output to change. 
 unsigned long samplePeriod = 11;// ~25us for an analog read with no delay, so give it another 25us to rest before next sample attempt?
// samplePeriod = 230 ~ 10hz
// samplePeriod = 1 ! 78.11hz

 



// Process incoming serial commands
// note: this function is blocking and will delay the loop
void processCommand(String command) { 
  command.trim(); // Remove any leading or trailing whitespace 
  Serial.print("received command:");
  Serial.println(command);
  delay(1000);
  // Example commands: 
  if (command.startsWith("/f ")) { 
    freq = command.substring(3).toInt(); 
    Serial.print("Frequency set to: "); 
    Serial.println(freq); 
    delay(5000);
  } else if (command.startsWith("/r")) { 
    adcResolution = command.substring(3).toInt(); 
    bitresolution = pow(2, adcResolution) - 1; 
    analogReadResolution(adcResolution); 
    analogWriteResolution(adcResolution); 
    Serial.print("ADC Resolution set to: "); 
    Serial.println(adcResolution); 
  } else if (command.startsWith("/d ")) { 
    samplePeriod = command.substring(3).toInt(); 
    Serial.print("Sample delay set to: "); 
    Serial.println(samplePeriod); 
  } else if (command.startsWith("/z")) { 
    plotter = !plotter;
    Serial.println(plotter); 
    delay(2000);
  } else if (command.startsWith("/pm ")) {   
    int pin = command.substring(9, command.indexOf(' ', 3)).toInt(); 
    String mode = command.substring(command.indexOf(' ', 3) + 1); 
    if (mode == "OUTPUT") { 
      pinMode(pin, OUTPUT); 
      Serial.print("Pin "); 
      Serial.print(pin); 
      Serial.println(" set to OUTPUT"); 
    } else if (mode == "INPUT") { 
      pinMode(pin, INPUT); 
      Serial.print("Pin "); 
      Serial.print(pin); 
      Serial.println(" set to INPUT"); 
    } 
  } else { 
      Serial.println("Unknown command. Available commands: /freq, /resolution, /delay, /pinMode"); 
  } 
}

/*

Setup the serial monitor and ADC resolution

*/
void setup() {
  Serial.begin(115200);
  analogReadResolution(adcResolution); //change to ??-bit resolution  
  analogWriteResolution(adcResolution);
  
  //wave.sine(freq);       // Generate a sine wave with the initial frequency  

  Serial.println();
  Serial.println("Type commands to adjust settings:");
  delay(5000);   
}

/*

Loop through the sine wave and read the ADC

*/
void loop() {  
  // Start the timer
  unsigned long startTime = micros(); // Start time in microseconds 

  // Loop through the sine wave
  for (int i = 0; i < NUM_SAMPLES; i++) {
    // analogWrite takes c. 10uS for the code and 5uS (max) for the DAC output to change.
    analogWrite(A0, sineLookupTable[i]); // Write to DAC using mod operator
    //delay in Microseconds to create/adjust for 60hz sine wave
    delayMicroseconds(samplePeriod); 
    //Read the ADC
    readings[i] = analogRead(A1);
    // Store the elapsed time
    timestamps[i] = micros() - startTime; // Store the elapsed time    
    //ADC readings are averaging around 23 to 24 microseconds per read with no delay
    //According to the RA4M1 data-sheet the DAC12 conversion time is 30uS to reach specified accuracy... although the 'scope shows this as being faster.
    //https://forum.arduino.cc/t/arduino-uno-r4-dac-speed/1177741    
  }//end for

  // Calculate the total time and frequency
  unsigned long totalTime = timestamps[NUM_SAMPLES - 1] - timestamps[0]; 
  float frequency = 1000000.0 / totalTime; // Calculate frequency

  

  // Output the results (optional, for debugging)
  for (int i = 0; i < NUM_SAMPLES-1; i++) {   
    //12-bit (0-4096) and 14-bit (0-65355) resolution or etc
    float voltage = readings[i] * (5.0 / bitresolution); // Convert ADC reading to voltage    

  if(plotter){
    Serial.println(voltage,4);
    delay(10);
  }else{    
      Serial.print(i);
      Serial.print("Reading: ");
      Serial.print(readings[i]);
      Serial.print(" -> ");
      Serial.print(voltage,4);
      Serial.print("v at Time: ");
      Serial.print(timestamps[i]);
      Serial.print(" - ");
      Serial.print(timestamps[i+1]);
      Serial.print(" = ");
      if (timestamps[i+1] > timestamps[i]) { 
        Serial.print(timestamps[i+1] - timestamps[i]); // Calculate and print the time difference 
      } else { 
        Serial.print((4294967295 - timestamps[i]) + timestamps[i+1] + 1); // Handle overflow case 
      } Serial.println(" microseconds");
    
    }//end if plotter
  }//end for 

  if(!plotter){
    Serial.print("Freq set to "); Serial.print(freq); Serial.println("hz");
    Serial.print("Resolution set to "); Serial.print(adcResolution); Serial.println("-bit");
    Serial.print("Number of Samples "); Serial.print(NUM_SAMPLES); Serial.println("");
    Serial.print("Sample Delay set to "); Serial.print(samplePeriod); Serial.println("us");
    //Total Time for 1 cycle of 60hz = 16667us
    //11us delay per sample = 11us * 360 samples = 3960us
    Serial.print("Total Time "); Serial.print(totalTime); Serial.print("us ~ "); 
    Serial.print(frequency, 2); Serial.println(" Hz"); 
    
    //Serial.print( 1 / ( (timestamps[2] - timestamps[1]) * NUM_SAMPLES) );Serial.println("hz");
    delay(5000); // Delay for one second before the next batch
  }

  // Check for incoming serial commands
  if (Serial.available()) { 
      String command = Serial.readStringUntil('\n'); 
      processCommand(command);
    }

}//end loop

