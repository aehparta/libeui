/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */


/******************************************************************************/
#include "main.h"


/******************************************************************************/
#define TOUCH_IRQ_PORT_DIR       P4DIR
#define TOUCH_IRQ_PORT_IN        P4IN
#define TOUCH_IRQ_PIN            BIT3


/******************************************************************************/
eui_context_t context;

float touch_cal_x = 2064.0;
float touch_cal_ppx = -11.021;
float touch_cal_y = 1958.0;
float touch_cal_ppy = -14.948;
int touch_pressed = 0;


/******************************************************************************/
static void init_clocks()
{
    /* initialize XT1 */
    P5SEL |= BIT4 | BIT5;
    UCSCTL6 &= ~XT1OFF; /* enable XT1 */
    UCSCTL6 &= ~XCAP_3; /* internal load cap off */

    /* initialize XT2 */
    P5SEL |= BIT3 | BIT2;
    UCSCTL6 &= ~XT2OFF; /* enable XT2 */
    UCSCTL6 &= ~XCAP_2; /* internal load cap off */
    
    /* loop until XT1,XT2 & DCO fault flag is cleared */
    do
    {
        /* clear XT2, XT1 fault flags */
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
        /* clear oscillator fault flag */
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG); /* test oscillator fault flag */
    
    /* set MCLK and SMCLK to XT2 */
    UCSCTL5 = (UCSCTL5 & 0xff88) | 0b000 | 0b0000000;
    UCSCTL4 = (UCSCTL4 & 0xff88) | 0b101 | 0b1010000;
}


/******************************************************************************/
void init_timers()
{
    /* set timer A as 100 times per second maintenance timer */
    TA0CCTL0 = CCIE;                            /* CCR0 interrupt enabled */
    TA0CCR0 = (uint16_t)(MCLK_FREQ / 8 / 100);  /* 100 times per second */
    TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR;    /* SMCLK, prediv 8, upmode, clear TAR */
}


/******************************************************************************/
static uint16_t _ui_touch_read(int channel)
{
    uint8_t data[2];
    uint8_t cmd = 0b10000100 | (channel << 4);
    
    TOUCH_CS_OUT &= ~TOUCH_CS;
    
    spi_write(&cmd, 1);
    __delay_cycles(50);
    
    spi_read(data, 2);
    
    TOUCH_CS_OUT |= TOUCH_CS;
    
    return ((data[0] << 8) | data[1]) >> 3;
}


/******************************************************************************/
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR(void)
{
    float x = (float)_ui_touch_read(1);
    float y = (float)_ui_touch_read(5);
    int pressed = !(TOUCH_IRQ_PORT_IN & TOUCH_IRQ_PIN);
    
    x = ((x - touch_cal_x) / touch_cal_ppx + (context.w / 2));
    y = ((y - touch_cal_y) / touch_cal_ppy + (context.h / 2));
    
    eui_touch_update(&context, (eui_coord_t)x, (eui_coord_t)y, pressed);
}


/******************************************************************************/
int main(void)
{
    eui_view_t view;
    eui_rect_t rc;
    eui_button_t buttons[20], buttons2[10];
    eui_button_t button;
    eui_label32_t label;
    eui_box_t vbox, hbox, vbox2, vbox3;
    eui_area_t area;
    eui_image_t image;
    int i;
    
    /* basic setup */
    WDT_A_hold(WDT_A_BASE);
    PMM_setVCore(PMM_CORE_LEVEL_2);
    init_clocks();
    init_timers();
    spi_init();
    
    /* set touch detection pin as input */
    TOUCH_IRQ_PORT_DIR &= ~TOUCH_IRQ_PIN;
    
    /* initialize display driver and ui */
    SSD1289_DriverInit();
    Graphics_initContext(&context.display, &g_sSSD1289_Driver);
    eui_init(&context, Graphics_getDisplayWidth(&context.display), Graphics_getDisplayHeight(&context.display));
    eui_touch_init(&context);
    eui_view_init(&context, &view);
    
    /* setup and create view */
    eui_vbox_init(&vbox);
    //vbox.w_fixed = 300;
    vbox.child_h = 30;
    for (i = 0; i < 20; i++) {
        char str[32] = "Label";
        sprintf(str, "Long Label %d", (i + 1));
        eui_button_init(&buttons[i], str);
        eui_container_append((eui_container_t *)&vbox, (eui_object_t *)&buttons[i]);
        buttons[i].h_fixed = 80;
        buttons[i].font = &g_sFontCm16;
    }
    //buttons[3].w_fixed = 200;
    buttons[3].h_fixed = 100;
    
    eui_vbox_init(&vbox2);
    //vbox2.w_fixed = 300;
    for (i = 0; i < 10; i++) {
        char str[32] = "Test";
        sprintf(str, "Test %d", (i + 1));
        eui_button_init(&buttons2[i], str);
        eui_container_append((eui_container_t *)&vbox2, (eui_object_t *)&buttons2[i]);
        buttons2[i].h_fixed = 100;
        buttons2[i].font = &g_sFontCm16;
    }
    
    eui_hbox_init(&hbox);
    eui_container_append((eui_container_t *)&hbox, (eui_object_t *)&vbox);
    eui_container_append((eui_container_t *)&hbox, (eui_object_t *)&vbox2);
    
    eui_vbox_init(&vbox3);
    eui_container_append((eui_container_t *)&vbox3, (eui_object_t *)&hbox);
    //eui_label_init(&label, "footer label");
    //eui_container_append(&vbox3, &label);
    //label.h_fixed = 40;
    
//      eui_image_init(&image, &image_data);
//     image.fg = EUI_COLOR_WHITE;
//     image.bg = EUI_COLOR_BLACK;
//     eui_container_append(&vbox3, &image);
//     eui_button_init(&button, "Footer Button");
//     button.h_fixed = 40;
//     eui_container_append(&vbox3, &button);
    
    view.object = (eui_object_t *)&vbox3;

    view.font = &g_sFontFixed6x8;
    
    /* enable interrupts */
    __enable_interrupt();
    
    /* set backlight on */
    SSD1289_DriverBacklightSet(1);
    
    /* loop forever */
    while (1) {
        eui_touch_read(&context);
        eui_view_draw(&view);
    }
    
    return 0;
}
