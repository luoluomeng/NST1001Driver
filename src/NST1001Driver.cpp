#include "Arduino.h"
#include "driver/mcpwm.h"

static uint32_t last_paulse = 0;
static uint32_t pulse_count = 0;

static QueueHandle_t cap_queue;
static uint32_t tt = 0;
static bool nst1001_isr_handler(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_sig, const cap_event_data_t *edata,
                                void *arg)
{
    BaseType_t high_task_wakeup = pdFALSE;
    tt = millis();
    if (last_paulse != 0)
    {
        uint32_t ticketElapsed = (edata->cap_value - last_paulse);
        if (ticketElapsed > 1000000)
        {

            xQueueSendFromISR(cap_queue, &pulse_count, &high_task_wakeup);
            pulse_count = 0;
        }
        else
        {
            pulse_count++;
        }
    }
    last_paulse = edata->cap_value;
    return high_task_wakeup == pdTRUE;
}

void initNST1001(int pin)
{
    cap_queue = xQueueCreate(1, sizeof(uint32_t));
    if (cap_queue == NULL)
    {
        return;
    }
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, pin));

    mcpwm_capture_config_t conf = {
        .cap_edge = MCPWM_POS_EDGE,
        .cap_prescale = 1,
        .capture_cb = nst1001_isr_handler,
        .user_data = NULL};
    ESP_ERROR_CHECK(mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &conf));
}

float getNST1001Temp()
{
    uint32_t pulseCount;
    xQueueReceive(cap_queue, &pulseCount, portMAX_DELAY);
    float temp = pulseCount * 0.0625 - 50.0625;
    return temp;
}
