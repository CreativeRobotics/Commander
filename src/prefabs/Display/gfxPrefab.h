//GFX prefab

void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

drawLine x0 y0 xl yl col

drawFastVLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);

drawFastVLine x0 y0 len col


drawFastHLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);

drawFastHLine x0 y0 len col

void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

drawRect x0 y0 w h col

void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

fillRect x0 y0 w h col

void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

drawCircle

void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

fillCircle

void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);

drawRoundRect

void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);

fillRoundRect

void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);

drawChar

void setCursor(uint16_t x0, uint16_t y0);

setCursor

void setTextColor(uint16_t color);

setTextColor

void setTextColor(uint16_t color, uint16_t backgroundcolor);

setTextColor

void setTextSize(uint8_t size);

setTextSize

void setTextWrap(boolean w);

setTextWrap

void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

drawBitmap

//The bitmap data must be located in program memory using the PROGMEM directive. 

void fillScreen(uint16_t color);

fillScreen

void setRotation(uint8_t rotation);

setRotation