
const byte ledPin = 9;
const byte ledBrightness = 10; // from 0 to 255

// These Epochs are in half/seconds
const int epochSize = 10;
const int baselineEpochSize = 10;
//const int epochSize = 30; //15 seconds
//const int initialEpochSize = 3600; //30 minutes
//const int minutesToFallAsleep = 0;
const int minutesToFallAsleep = 30;

// The variation of the distance sensor during the first phase (initialEpochSize) of the sleep
// We assume a REM phase if the variation from the past 15 seconds is larger than two times
// this baseline
int baselineVariation;

boolean REM = false;


void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(13, OUTPUT);

  doDemoMode();

  Serial.println("Waiting until you fall asleep ...");
  for(int i = 0; i < minutesToFallAsleep*2; i++) {
    delay(1000*30);
    Serial.print("Minute "); Serial.print(i/2); Serial.println(" passed half");
  }

  Serial.println("Recording baseline variance now");
  baselineVariation = recordEpochAndGetVariationAndMaybeBlink(baselineEpochSize, false);

  Serial.println("Initial Phase is over");
  Serial.print("Baseline Variance is ");
  Serial.println(baselineVariation);
}

void loop() {

  int currentVariation = recordEpochAndGetVariationAndMaybeBlink(epochSize, REM);
  Serial.print("Current Variation:");
  Serial.println(currentVariation);
  if (currentVariation > 2 * baselineVariation) { //If the current variation is two times larger than the baseline variation
    REM = true;
    digitalWrite(13, HIGH);
  }
  else {
    REM = false;
    digitalWrite(13, LOW);
  }
}

void doDemoMode() {

  Serial.println("Starting Demo Mode for 5 min");
  blink3Times();
  captureBaseline();
  binkOnce();

  Serial.println("Initial Phase is over");
  Serial.print("Baseline Variance is ");
  Serial.println(baselineVariation);

  // This is not a real probability, just a counter that is increased each time the variation
  // from the past epoch is two times the baseline variance
  boolean remProbability = 0;

  for (int i = 0; i < 10; i++) {
    int currentVariation = recordEpochAndGetVariationAndMaybeBlink(epochSize, REM);
    Serial.print("Current Variation:");
    Serial.println(currentVariation);
    if (currentVariation > 2 * baselineVariation) { //If the current variation is two times larger than the baseline variation
      REM = true;
      digitalWrite(13, HIGH);
    }
    else {
      REM = false;
      digitalWrite(13, LOW);
    }
    if (REM)
      Serial.println("NOW IS REM");
  }

  Serial.println("Demo Mode Over!");
  blink3Times();
}

void binkOnce() {
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
}

void blink3Times() {
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
}

void captureBaseline() {
  Serial.println("Recording baseline variance now");
  baselineVariation = recordEpochAndGetVariationAndMaybeBlink(baselineEpochSize, false);
}




// This will record measurements for a certain time (based on epochlen) in 0.5s intervals
// Also if blinkmeanwhile is true, it will blink the LED while doing it
int recordEpochAndGetVariationAndMaybeBlink(int epochlen, boolean blinkmeanwhile) {
  if (blinkmeanwhile) Serial.println("should blink now");
  byte readings[epochlen];
  for (int i = 0; i < epochlen; i++) {
    readings[i] = readSensor();
    delay(500);
    if (blinkmeanwhile)
      if (i % 2 == 0)
        analogWrite(ledPin, ledBrightness);
      else
        analogWrite(ledPin, 0);
  }
  return variation(readings, epochlen);
}

// Computes the variation of the values in an byte array
int variation(byte values[], int numvalues) {
  byte m = mean(values, numvalues);
  int meanSqDev = 0;
  for (int i = 0; i < numvalues; i++) {
    int diff = m - values[i];
    meanSqDev += diff * diff;
  }
  return meanSqDev / numvalues;
}

// Computes the mean value of a byte array
int mean(byte values[], byte numvalues) {
  int sum = 0;
  for (int i = 0; i < numvalues; i++) {
    sum += values[i];
  }
  return sum / numvalues;
}

// Reads the sensor returns a value 0 to 255. Change this if you use a different sensor!
byte readSensor() {
  int raw = analogRead(A0);
  return map(raw, 0, 1024, 0, 255);
}