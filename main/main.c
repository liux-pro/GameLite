#include <sys/cdefs.h>
#include <sys/select.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "st7789.h"
#include "st7789_faster.h"

static const char *MAIN_LOG = "MAIN_LOG";

TFT_t dev;


//Transfer GuiLite 32 bits color to your LCD color
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
#define FLIPW(h) ((((uint16_t)(h) << 8)&0xFF00) | ((uint16_t)(h) >> 8))


void gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb) {

}


//UI entry
struct DISPLAY_DRIVER {
    void (*draw_pixel)(int x, int y, unsigned int rgb);

    void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_driver;

extern void startHello3Dwave(void *phy_fb, int width, int height, int color_bytes, struct DISPLAY_DRIVER *driver);
extern void setping();

void delay_ms(unsigned int nms) {
    ESP_LOGI("GUI", "%d",nms);
    vTaskDelay(nms / portTICK_PERIOD_MS);
}


uint16_t WorkFrame[240 * 320];

//Encapsulate your LCD driver:
void gfx_draw_pixel(int x, int y, unsigned int rgb) {
//    lcdDrawPixel(&dev, x, y, GL_RGB_32_to_16(rgb));
    WorkFrame[x + y * 240] = (uint16_t) FLIPW(GL_RGB_32_to_16(rgb));

}
void taskGUI() {

};


spi_transaction_t *p_spi_transaction_pool;

void initSpiTransactionPool() {
    void *pVoid = malloc(sizeof(spi_transaction_t) * 240);
    memset(pVoid, 0, sizeof(spi_transaction_t) * 240);
    p_spi_transaction_pool = pVoid;
    for (int i = 0; i < 240; i += 1) {
        p_spi_transaction_pool[i].length = CONFIG_WIDTH * 2 * 8;
        p_spi_transaction_pool[i].tx_buffer = &WorkFrame[i * (CONFIG_WIDTH)];
    }
}

_Noreturn void taskLCD(void *param) {


    spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO,
                    CONFIG_BL_GPIO);
    lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);
    initSpiTransactionPool();

    my_driver.draw_pixel = gfx_draw_pixel;
    my_driver.fill_rect = NULL;//gfx_fill_rect;
    startHello3Dwave(NULL, 240, 320, 2, &my_driver);


    memset(WorkFrame, 0, 240 * 240 * 2);

    while (1) {
        setping();

        lcdPrepareMultiPixels(&dev);
        for (int i = 0; i < 240; i += 1) {
            spi_device_queue_trans(dev._SPIHandle, &p_spi_transaction_pool[i], portMAX_DELAY);
        }

        spi_transaction_t *r_trans;
        for (int i = 0; i < 240; i += 1) {
            spi_device_get_trans_result(dev._SPIHandle, &r_trans, portMAX_DELAY);
        }
    }
}

_Noreturn void taskSystemMonitor(void *param) {
    uint8_t CPU_RunInfo[2048]; //保存任务运行时间信息

    while (1) {
        //######### 内存 占用信息 #########
        memset(CPU_RunInfo, 0, 2048); //信息缓冲区清零

        vTaskList((char *) &CPU_RunInfo); //获取任务运行时间信息

        ESP_LOGI(MAIN_LOG, "\n---------------------------------------------");
        ESP_LOGI(MAIN_LOG, "\ntask_name\ttask_status\tpriority\tstack task_id\n%s", CPU_RunInfo);
        ESP_LOGI(MAIN_LOG, "total memort free :  %d\n", esp_get_free_heap_size());

        //######### cpu 占用信息 #########
        memset(CPU_RunInfo, 0, 2048);

        vTaskGetRunTimeStats((char *) &CPU_RunInfo); //

        ESP_LOGI(MAIN_LOG, "\ntask_name\trun_cnt\tusage_rate\n%s", CPU_RunInfo);

        vTaskDelay(10000 / portTICK_PERIOD_MS); /* 延时500个tick */
    }
}

_Noreturn void app_main(void) {

    xTaskCreate(taskSystemMonitor, "taskSystemMonitor", 4 * 1024, NULL, 5, NULL);
    xTaskCreate(taskLCD, "taskLCD", 4 * 1024, NULL, 5, NULL);

    while (1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}