#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "*******"
#define WIFI_PASSWORD "******"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "*******************************"

/* 3. Define the RTDB URL */
#define DATABASE_URL "************************************" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* Sensor pin definitions */
#define MoistureSensor A0
#define relayPin D7

DHT dht = DHT(D4, DHT11); // DHT(uint8_t pin, uint8_t type);

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

bool pump_activate = false;

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

void setup() {
  Serial.begin(115200);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;


Firebase.reconnectWiFi(true);


/* Sign up */
    if (Firebase.signUp(&config, &auth, "", ""))
    {
        Serial.println("ok");
    }
    else
        Serial.printf("%s\n", config.signer.signupError.message.c_str());

  
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  
  Firebase.setDoubleDigits(5);

  Serial.println("Light Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Setup the sensor gain and integration time */
  configureSensor();
  
  pinMode(D4, INPUT);
  dht.begin();

  pinMode(relayPin, OUTPUT);

  /* We're ready to go! */
  Serial.println("GOOD TO GO!");
}

void loop() {
  /* Get a new light sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);

  float humidity;
  float temperature;
  float moisture;
  float ideal_moisture;

  /* Read from sensors */
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  moisture = analogRead(MoistureSensor);

  moisture = moisture / 9.0; /* Converts moisture values to percentage by dividing value by 900(max range of sensor)*/

  Serial.printf("\t");
  Serial.printf("Humidity: %.1f", humidity);
  Serial.printf("\t");
  Serial.printf("Temperature: %.1f", temperature);
  Serial.printf("\t");
  Serial.printf("Soil moisture: %.3f", moisture);
  Serial.printf("\t");

 
  if (event.light)
  {
    Serial.print(event.light); Serial.println(" lux");
    Serial.printf("\n");
    Firebase.setFloat(fbdo, "/luminosity", event.light);
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
    Serial.printf("\n");
  }

  /* Send data to Firabase database */
  
  Firebase.setFloat(fbdo, "/humidity", humidity);
  Firebase.setFloat(fbdo, "/temperature", temperature);
  Firebase.setFloat(fbdo, "/moisture", moisture);

  Firebase.getFloat(fbdo, "/ideal_moisture", &ideal_moisture);
  
  /* Water pump control */
  
  Firebase.getBool(fbdo, "/pump", &pump_activate);
  
  if(moisture < ideal_moisture || pump_activate == true){
    digitalWrite(relayPin, LOW); // turn relay on
    Firebase.setBool(fbdo, "/pump", false);
  }
  else{
    digitalWrite(relayPin, HIGH); // turn relay off
  }
  
  delay(10000);

}
