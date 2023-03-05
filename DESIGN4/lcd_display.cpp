#include "lcd_display.h"

// LCD display object
LiquidCrystal_I2C lcd(0x3F, 20, 4);

// Initialize the LCD display
void init_lcd() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

// Display the caller ID and time of call
void display_call_info(char* caller_id, char* time_of_call) {
  lcd.setCursor(0, 0);
  lcd.print("Caller ID: ");
  lcd.print(caller_id);
  lcd.setCursor(0, 1);
  lcd.print("Time of call: ");
  lcd.print(time_of_call);
}

// Clear the LCD display
void clear_lcd() {
  lcd.clear();
}
