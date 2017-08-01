#include <pebble.h>

static Window *window;
bool flag = false;
bool breathing=true;
int count=0;
int step=0;
static const uint32_t my_segments[] = { 100,100,100};
static GBitmap *s_bitmap;
static GBitmap *h_bitmap;
static GBitmap *x_bitmap;

static BitmapLayer *s_bitmap_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //吸
  bitmap_layer_set_bitmap(s_bitmap_layer, x_bitmap);
  count=0;
  flag=true;
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
 
  //
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HX_ICON);
  h_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_H_ICON);
  x_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_X_ICON);

  s_bitmap_layer = bitmap_layer_create(GRect(5, 5, 132, 132));
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  layer_add_child(window_layer, 
                                      bitmap_layer_get_layer(s_bitmap_layer));   
}

//
static void window_unload(Window *window) {
  gbitmap_destroy(s_bitmap);
     gbitmap_destroy(h_bitmap);
  gbitmap_destroy(x_bitmap);

  bitmap_layer_destroy(s_bitmap_layer);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
if(!flag){
   return;
}
count=count+1;
   
if(count>120){
   flag=false;
   bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);   
   return;
}
   
/*0开始震动*/
if(step==0){
   VibePattern pat = {
      .durations = my_segments,
      .num_segments = ARRAY_LENGTH(my_segments),
    };
    vibes_enqueue_custom_pattern(pat);
}

if(breathing){
   if(step==3){
      breathing=false;
      step=0;
      //呼
        bitmap_layer_set_bitmap(s_bitmap_layer, h_bitmap);

      return;
   }
}else{
   if(step==4){
      breathing=true;
      step=0;
      //吸
      bitmap_layer_set_bitmap(s_bitmap_layer, x_bitmap);
      return;
   }
}
step=step+1;
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_background_color(window,GColorBlack);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}