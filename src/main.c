#include <pebble.h>
  
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
  
static Window *s_main_window;
static TextLayer *s_background_layer;
static TextLayer *s_hour_layer;
static TextLayer *s_min1_layer;
static TextLayer *s_min2_layer;

static char *hour[] = {"noll", "ett", "två", "tre", "fyra", "fem", "sex", "sju", "åtta", "nio", "tio", "elva", "tolv"};
static char *min[] = {"noll", "ett", "två", "tre", "fyra", "fem", "sex", "sju", "åtta", "nio", "tio", "elva", "tolv", "tretton", "fjorton", "femton", "sexton", "sjutton", "arton", "nitton"};
static char *min2[] = {"tjugo", "trettio", "förtio", "femtio"};


static GFont s_hour_font;
static GFont s_min_font;

static void update_time() {
  int h, m;
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
    
  // Create a long-lived buffer
  static char buffer_hour[] = "    ";
  static char buffer_min1[] = "       ";
  static char buffer_min2[] = "       ";
  
  
  h = tick_time->tm_hour;
  m = tick_time->tm_min;
  
  if(h>12) {
    h = h-12;
  }
  strcpy(buffer_hour, hour[h]);  
   
  if(m > 0 && m < 20) {
    strcpy(buffer_min1, min[m]);
    strcpy(buffer_min2, " ");
    m = 0;
  } else {
    if(m >= 20 && m < 30) {
      strcpy(buffer_min1, min2[0]);
      m -= 20;
    } else if(m >= 30 && m < 40) {
      strcpy(buffer_min1, min2[1]);
      m -= 30;
    } else if(m >= 40 && m < 50) {
      strcpy(buffer_min1, min2[2]);
      m -= 40;
    } else if(m >= 50) {
      strcpy(buffer_min1, min2[3]);
      m -= 50;
    }
  }
  
  if(m == 0) {
    strcpy(buffer_min2, " ");
  } else {
    strcpy(buffer_min2, min[m]);
  }
  

  
  // Display this time on the TextLayer
  text_layer_set_text(s_hour_layer, buffer_hour);
  text_layer_set_text(s_min1_layer, buffer_min1);
  text_layer_set_text(s_min2_layer, buffer_min2);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_background_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_background_layer, GColorBlack);
  
// Create Hour TextLayer
  s_hour_layer = text_layer_create(GRect(5, 20, 144, 50));
  s_min1_layer = text_layer_create(GRect(5, 70, 144, 30));
  s_min2_layer = text_layer_create(GRect(5, 105, 144, 30));
  
  text_layer_set_background_color(s_hour_layer, GColorBlack);
  text_layer_set_background_color(s_min1_layer, GColorBlack);
  text_layer_set_background_color(s_min2_layer, GColorBlack);
  
  text_layer_set_text_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_min1_layer, GColorClear);
  text_layer_set_text_color(s_min2_layer, GColorClear);

  // Create GFont
  s_hour_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BOLD_36));
  s_min_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NORMAL_24));

  // Apply to TextLayer
  text_layer_set_font(s_hour_layer, s_hour_font);
  text_layer_set_font(s_min1_layer, s_min_font);
  text_layer_set_font(s_min2_layer, s_min_font);
  
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_min1_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_min2_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_background_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_min1_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_min2_layer));
  
}


static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_min1_layer);
  text_layer_destroy(s_min2_layer);
  text_layer_destroy(s_background_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_hour_font);
  fonts_unload_custom_font(s_min_font);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

}

static void deinit() {
// Destroy Window
    window_destroy(s_main_window);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}