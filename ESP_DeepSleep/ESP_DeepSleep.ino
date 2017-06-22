/* Deep Sleep - Blink
 *
 * Blinks the onboard LED, sleeps for 10 seconds and repeats
 *
 * Connections:
 * D0 -- RST
 *
 * If you cant reprogram as the ESP is sleeping, disconnect D0 - RST and try again
 */

// sleep time in seconds
const int sleepSeconds = 10;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nWake up");

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW); //LED ON
  
  // Connect D0 to RST to wake up
  pinMode(D0, WAKEUP_PULLUP);

  /* SOME CODE HERE */

  Serial.printf("Sleep for %d seconds\n\n", sleepSeconds);
  ESP.deepSleep(sleepSeconds * 1000000);
}

void loop() {
/* no use of the loop funciotn in this kind of setup */
}
