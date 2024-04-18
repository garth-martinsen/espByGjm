/*********
  Rui Santos wrote the original code.
  Garth Martinsen modified it with a global variable to control the Core1 actions. ie: Core0 sets global variable to make Core1 actions conditional.
  GJM assumes that either core can set the global var to control the other.
  Complete project details at http://randomnerdtutorials.com
*********/
#include <Arduino.h>
TaskHandle_t Task1;
TaskHandle_t Task2;
void Task1code(void *pvParameters);
void Task2code(void *pvParameters);
// LED pins
const int led1 = 27;
const int led2 = 26;
volatile bool holdupRed = false;

void setup()
{
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
      Task1code, /* Task function. */
      "Task1",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &Task1,    /* Task handle to keep track of created task */
      0);        /* pin task to core 0 */
  delay(500);

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
      Task2code, /* Task function. */
      "Task2",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &Task2,    /* Task handle to keep track of created task */
      1);        /* pin task to core 1 */
  delay(500);
}

// Task1code: blinks a green LED every 2000 ms
void Task1code(void *pvParameters)
{
  Serial.print("Task1 running on core ");
  int core0 = (xPortGetCoreID());
  Serial.println(core0);
  int cnt = 0;
  for (;;)
  {
    cnt++;
    if (cnt % 10 == 0)
    {
      holdupRed = !holdupRed;
    }
    digitalWrite(led1, HIGH);
    Serial.printf(" \ng:%d\n", core0);
    delay(2000);
    digitalWrite(led1, LOW);
    delay(2000);
  }
}

// Task2code: blinks a red LED every 200 ms
void Task2code(void *pvParameters)
{
  Serial.print("Task2 running on core ");
  int core1 = xPortGetCoreID();
  Serial.println(core1);
  int once = 0;
  int rcnt = 0;
  for (;;)
  {
    rcnt++;
    if (holdupRed)
    {
      switch (once)
      {
      case 0:
        Serial.println("Holding up Red Blinking");
        once = 1;
        break;
      case 1:
        break;
      }
    }
    else
    {
      once=0;
      digitalWrite(led2, HIGH);
      Serial.printf("r:%d", core1);
      delay(200);
      digitalWrite(led2, LOW);
      delay(200);
    }
  }
}

void loop()
{
}