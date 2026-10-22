//Adding libraries needed
#include <stdio.h> //standart C
#include "driver/gpio.h" //setting gpios up
#include "esp_rom_sys.h" //for brute delay (esp_rom_delay_us)
#include "freertos/FreeRTOS.h" //standart header for FreeRTOS
#include "freertos/task.h" //for soft delay (vTaskDelay())
#include "esp_timer.h" //for measuring the time
#include "esp_log.h" //for printing logs on serial monitor

//Initializing variables on GPIOs
#define PIN_TRIG GPIO_NUM_27
#define PIN_ECHO GPIO_NUM_26

//Initializing volatile variables for time operations
static volatile int64_t before = 0; //time measured(us) on raising edge 
static volatile int64_t after = 0; //time measured(us) on falling edge
static volatile int64_t diff = 0; //time(us) beetween raising and falling edges

//Setting TAG
static const char *TAG = "HCSR04";

void IRAM_ATTR measure(void *arg) {
    if (gpio_get_level(PIN_ECHO) == 1) 
	before = esp_timer_get_time();
    else {
	after = esp_timer_get_time();
	diff = after - before;
    }
}

void app_main(void) {
    //Setting GPIOs's modes
    gpio_set_direction(PIN_TRIG, GPIO_MODE_OUTPUT); // set Trig as output
    gpio_set_direction(PIN_ECHO, GPIO_MODE_INPUT); // set Echo as input
    gpio_set_intr_type(PIN_ECHO, GPIO_INTR_ANYEDGE); // set an interruption based on anyedge from echo (raising/falling)
    gpio_install_isr_service(0); //install the isr service
    gpio_isr_handler_add(PIN_ECHO, measure, NULL); // set the handler that will trigger the interruption

   while(1) {
       diff = 0;
       gpio_set_level(PIN_TRIG, 1);
       esp_rom_delay_us(10);
       gpio_set_level(PIN_TRIG, 0);
       

       //Needed delay
       vTaskDelay(pdMS_TO_TICKS(60));

       float distance = 0.01715 * diff;
       if (distance > 100)
	   distance = 0;

       ESP_LOGI(TAG, "Distância %.2fcm", distance); 
   }

}
