#include <SDHCI.h>
#include <NetPBM.h>
#include <DNNRT.h>
#include <Camera.h>
#include "Adafruit_ILI9341.h"

#define TFT_DC  9
#define TFT_CS  10

DNNRT dnnrt;
SDClass SD;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define OFFSET_X        48
#define OFFSET_Y         6
#define BOX_WIDTH      224
#define BOX_HEIGHT     224
#define DNN_IMG_WIDTH   28
#define DNN_IMG_HEIGHT  28



uint16_t result_buf[DNN_IMG_WIDTH * DNN_IMG_HEIGHT];

void CamCB(CamImage img)
{
  if (!img.isAvailable()) return;
  img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565); 
  tft.drawRGBBitmap(0, DNN_IMG_HEIGHT, (uint16_t *)img.getImgBuff(), 320, 240);
  
  // tft.drawRGBBitmap(0, 0, result_buf, DNN_IMG_HEIGHT, DNN_IMG_WIDTH);  

  CamImage small; 
  CamErr camErr = img.clipAndResizeImageByHW(small
            ,OFFSET_X ,OFFSET_Y 
            ,OFFSET_X+BOX_WIDTH-1 ,OFFSET_Y+BOX_HEIGHT-1 
            ,DNN_IMG_WIDTH ,DNN_IMG_HEIGHT);
  if (!small.isAvailable()) {
    Serial.println("Error Occured at making a target image");
    if (camErr) Serial.println("CamErr: " + String(camErr));
    return;
  }
  
  tft.drawRGBBitmap(0, 0, (uint16_t *)small.getImgBuff(), DNN_IMG_HEIGHT, DNN_IMG_WIDTH);  
  uint16_t* buf = (uint16_t*)small.getImgBuff();

  DNNVariable input(DNN_IMG_WIDTH * DNN_IMG_HEIGHT * 3);  
  float* fbuf_r = input.data();
  float* fbuf_g = fbuf_r + DNN_IMG_WIDTH * DNN_IMG_HEIGHT;
  float* fbuf_b = fbuf_g + DNN_IMG_WIDTH * DNN_IMG_HEIGHT;
  for (int i = 0; i < DNN_IMG_WIDTH * DNN_IMG_HEIGHT * 3; i+=3, ++buf) {
    *(fbuf_r++) = (float)(((*buf & 0xF800) >> 11) << 3) / 255.0;
    *(fbuf_g++) = (float)(((*buf & 0x07E0) >>  5) << 2) / 255.0 ; 
    *(fbuf_b++) = (float)(((*buf & 0x001F))       << 3) / 255.0 ; 
  }

  dnnrt.inputVariable(input, 0);
  dnnrt.forward();
  DNNVariable output = dnnrt.outputVariable(0);

  uint16_t value = 0;
  for (int i = 0; i < DNN_IMG_WIDTH * DNN_IMG_HEIGHT; ++i) {
    uint16_t value = output[i] * 255;
    if (value >= 256) value = 255;
    result_buf[i] = ((value >> 2) << 5) & 0x07E0;
  }
  tft.drawRGBBitmap(320-DNN_IMG_WIDTH, 0, result_buf, DNN_IMG_WIDTH, DNN_IMG_HEIGHT);
 
}

void setup() {

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  
  File nnbfile = SD.open("model.nnb");
  if (!nnbfile) {
    Serial.print("nnb not found");
    return;
  }

  int ret = dnnrt.begin(nnbfile);
  if (ret < 0) {
    Serial.print("Runtime initialization failure. ");
    Serial.println(ret);
    return;
  }

  tft.fillRect(0, 0, 320, DNN_IMG_HEIGHT, ILI9341_BLUE);
  
  theCamera.begin();
  theCamera.startStreaming(true, CamCB);
}

void loop() {
  // put your main code here, to run repeatedly:

}
