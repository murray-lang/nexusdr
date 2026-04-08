/*
 * lvgl_port_lcd.h
 *
 *  Updated for LVGL v9.x
 *  Original: Ahmet Alperen Bulut / github.com/ahmetalperenbulut
 */

#ifndef INC_LVGL_PORT_LCD_H_
#define INC_LVGL_PORT_LCD_H_

#ifdef __cplusplus
 extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

#define LCD_INSTANCE    (0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize LCD display for LVGL v9
 */
void LCD_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
 }
#endif


#endif /* INC_LVGL_PORT_LCD_H_ */
