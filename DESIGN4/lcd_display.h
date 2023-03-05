#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal.h>

void init_lcd_display();
void display_caller_info(char* caller_name, char* caller_number, int call_duration);
void display_network_info(char* ssid, char* ip_address);
void display_message(char* message);
void clear_lcd_display();

#endif
