#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include<stdio.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define DISP_BUF_SIZE (480 * 1024)
void mycode();
void my_event_cb(lv_event_t *e);
void my_event_cb2(lv_event_t *e);
void my_event_cb3(lv_event_t *e);
void light();
void event_btn(void);

static lv_style_t style;
static lv_style_t style2;
static lv_style_t style3;
int flag=1;
int flag2=1;
int flag3=1;
lv_obj_t * obj;
lv_obj_t * obj2;
lv_obj_t * obj3;
lv_obj_t *btn;
lv_obj_t *btn2;
lv_obj_t *btn3;
void mycode(){//!!!
/* 
make&&scp demo 192.168.22.2:
 */

  // system("echo v1.2----------------");
  light();
  event_btn();//颜色变化按钮
  // system("./demo&");
}

void light(){
  obj=lv_btn_create(lv_scr_act());
  obj2=lv_btn_create(lv_scr_act());
  lv_obj_set_size(obj,150,150);
  lv_obj_set_size(obj2,150,150);
  lv_obj_align(obj,LV_ALIGN_RIGHT_MID,-30,-20);
  lv_obj_align(obj2,LV_ALIGN_LEFT_MID,30,-20);
  lv_style_init(&style);
  lv_style_init(&style2);
  lv_style_set_bg_color(&style,lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_bg_color(&style2,lv_palette_main(LV_PALETTE_BLUE));
  lv_obj_add_style(obj,&style,LV_STATE_DEFAULT);
  lv_obj_add_style(obj2,&style2,LV_STATE_DEFAULT);

  obj3=lv_btn_create(lv_scr_act());
  lv_obj_set_size(obj3,150,150);
  lv_obj_align(obj3,LV_ALIGN_LEFT_MID,230,-20);
  lv_style_init(&style3);
  lv_style_set_bg_color(&style3,lv_palette_main(LV_PALETTE_BLUE));
  lv_obj_add_style(obj3,&style3,LV_STATE_DEFAULT);
}
void event_btn(void){
  btn=lv_btn_create(lv_scr_act());
  btn2=lv_btn_create(lv_scr_act());
  lv_obj_set_size(btn,100,50);
  lv_obj_set_size(btn2,100,50);
  lv_obj_align(btn,LV_ALIGN_RIGHT_MID,-50,100);
  lv_obj_align(btn2,LV_ALIGN_LEFT_MID,50,100);
  lv_obj_add_event_cb(btn,my_event_cb,LV_EVENT_PRESSED,NULL);
  lv_obj_add_event_cb(btn2,my_event_cb2,LV_EVENT_PRESSED,NULL);

  btn3=lv_btn_create(lv_scr_act());
  lv_obj_set_size(btn3,100,50);
  lv_obj_align(btn3,LV_ALIGN_LEFT_MID,250,100);
  lv_obj_add_event_cb(btn3,my_event_cb3,LV_EVENT_PRESSED,NULL);
}
void my_event_cb(lv_event_t *e){
  lv_event_code_t code=lv_event_get_code(e);
  switch(code){
    case LV_EVENT_PRESSED:
      if(flag==1){
        lv_style_set_bg_color(&style,lv_palette_main(LV_PALETTE_RED));
        flag=0;
      } else{
        lv_style_set_bg_color(&style,lv_palette_main(LV_PALETTE_BLUE));
        flag=1;
      }
      lv_obj_add_style(obj,&style,LV_STATE_DEFAULT);
      break;
    default:
      break;
  }
}
void my_event_cb2(lv_event_t *e){
  lv_event_code_t code=lv_event_get_code(e);
  switch(code){
    case LV_EVENT_PRESSED:
      if(flag2==1){
        lv_style_set_bg_color(&style2,lv_palette_main(LV_PALETTE_RED));
        flag2=0;
      } else{
        lv_style_set_bg_color(&style2,lv_palette_main(LV_PALETTE_BLUE));
        flag2=1;
      }
      lv_obj_add_style(obj2,&style2,LV_STATE_DEFAULT);
      break;
    default:
      break;
  }
}
void my_event_cb3(lv_event_t *e){
  lv_event_code_t code=lv_event_get_code(e);
  switch(code){
    case LV_EVENT_PRESSED:
      if(flag3==1){
        lv_style_set_bg_color(&style3,lv_palette_main(LV_PALETTE_RED));
        flag3=0;
      } else{
        lv_style_set_bg_color(&style3,lv_palette_main(LV_PALETTE_BLUE));
        flag3=1;
      }
      lv_obj_add_style(obj3,&style3,LV_STATE_DEFAULT);
      break;
    default:
      break;
  }
}

int main(void){
  /*lvgl初始化*/
  lv_init();
  /*输出设备初始化及注册*/
  fbdev_init();
  /*A small buffer for LittlevGL to draw the screen's content*/
  static lv_color_t buf[DISP_BUF_SIZE];
  /*Initialize a descriptor for the buffer*/
  static lv_disp_draw_buf_t disp_buf;
  lv_disp_draw_buf_init(&disp_buf,buf,NULL,DISP_BUF_SIZE);
  /*Initialize and register a display driver*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf=&disp_buf;
  disp_drv.flush_cb=fbdev_flush;
  disp_drv.hor_res=800;
  disp_drv.ver_res=480;
  lv_disp_drv_register(&disp_drv);
  // 输入设备初始化及注册
  evdev_init();
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
  indev_drv_1.type=LV_INDEV_TYPE_POINTER;
  /*This function will be called periodically (by the library) to get the mouse position and state*/
  indev_drv_1.read_cb=evdev_read;
  lv_indev_t * mouse_indev=lv_indev_drv_register(&indev_drv_1);

  mycode();//!!!

  while(1){
    lv_timer_handler(); // 事务处理
    lv_tick_inc(5);     // 节拍累计
    usleep(5000);
  }
  return 0;
}














/*用户节拍获取*/
uint32_t custom_tick_get(void){
  static uint64_t start_ms=0;
  if(start_ms==0){
    struct timeval tv_start;
    gettimeofday(&tv_start,NULL);
    start_ms=(tv_start.tv_sec*1000000+tv_start.tv_usec)/1000;
  }
  struct timeval tv_now;
  gettimeofday(&tv_now,NULL);
  uint64_t now_ms;
  now_ms=(tv_now.tv_sec*1000000+tv_now.tv_usec)/1000;
  uint32_t time_ms=now_ms-start_ms;
  return time_ms;
}
