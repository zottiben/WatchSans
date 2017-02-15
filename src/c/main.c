#include <pebble.h>
#define KEY_TEXT_COLOR 0
#define KEY_WINDOW_BACKGROUND 1
#define KEY_MERIDIAN 2

static Window *s_main_window;
static TextLayer *s_time_hour, *s_time_minute, *s_time_meridian;
static GFont s_font_opensans_50, s_font_opensans_20;
static bool is_meridian = false;

static BitmapLayer *s_window_background;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char hour[10];
  static char min[10];
  static char mer[10];
  
  if(clock_is_24h_style() == is_meridian) {
    strftime(hour, sizeof(hour), "%I", tick_time);
    strftime(min, sizeof(min), "%M", tick_time);
    strftime(mer, sizeof(mer), "%p", tick_time);
    text_layer_set_text(s_time_meridian, mer);
  } else {
    strftime(hour, sizeof(hour), "%H", tick_time);
    strftime(min, sizeof(min), "%M", tick_time);
    strftime(mer, sizeof(mer), "%%", tick_time);
    text_layer_set_text(s_time_meridian, "");
  }
  
  text_layer_set_text(s_time_hour, hour);
  text_layer_set_text(s_time_minute, min);
}

static void set_text_color(int color) {
  text_layer_set_text_color(s_time_hour, GColorFromHEX(color));
  text_layer_set_text_color(s_time_minute, GColorFromHEX(color));
  text_layer_set_text_color(s_time_meridian, GColorFromHEX(color));
}

static void set_window_background(int color) {
  GColor background_color = GColorFromHEX(color);
  bitmap_layer_set_background_color(s_window_background, background_color);
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple *text_color_t = dict_find(iter, KEY_TEXT_COLOR);
  Tuple *window_background_t = dict_find(iter, KEY_WINDOW_BACKGROUND);
  Tuple *is_meridian_t = dict_find(iter, KEY_MERIDIAN);
  
  if(text_color_t) {
    int text_color = text_color_t->value->int32;
    persist_write_int(KEY_TEXT_COLOR, text_color);
    set_text_color(text_color);
  }
  
  if(window_background_t) {
    int background_color = window_background_t->value->int32;
    persist_write_int(KEY_WINDOW_BACKGROUND, background_color);
    set_window_background(background_color);
  }
  
  if(is_meridian_t) {
    is_meridian = is_meridian_t->value->int8;
    persist_write_int(KEY_MERIDIAN, is_meridian);
    update_time();
  }
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void appMessage() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_time_hour = text_layer_create(GRect(0,5,bounds.size.w,bounds.size.h/2));
  s_time_minute = text_layer_create(GRect(0,bounds.size.h/2 - 15,bounds.size.w,bounds.size.h/2));
  s_time_meridian = text_layer_create(GRect(110,126,30,30));
  
  s_window_background = bitmap_layer_create(GRect(0,0,bounds.size.w, bounds.size.h));
  bitmap_layer_set_background_color(s_window_background, GColorBlack);
  
  s_font_opensans_50 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OPENSANS_72));
  s_font_opensans_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OPENSANS_15));
  
  text_layer_set_text(s_time_hour, "00");
  text_layer_set_font(s_time_hour, s_font_opensans_50);
  text_layer_set_text_alignment(s_time_hour, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_time_hour,  GTextOverflowModeWordWrap);
  text_layer_set_background_color(s_time_hour, GColorClear);
  
  text_layer_set_text(s_time_minute, "00");
  text_layer_set_font(s_time_minute, s_font_opensans_50);
  text_layer_set_text_alignment(s_time_minute, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_time_minute,  GTextOverflowModeWordWrap);
  text_layer_set_background_color(s_time_minute, GColorClear);
  
  text_layer_set_text(s_time_meridian, "AM");
  text_layer_set_font(s_time_meridian, s_font_opensans_20);
  text_layer_set_text_alignment(s_time_meridian, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_time_meridian,  GTextOverflowModeWordWrap);
  text_layer_set_background_color(s_time_meridian, GColorClear);
  
  text_layer_set_text_color(s_time_hour, GColorWhite);
  text_layer_set_text_color(s_time_minute, GColorWhite);
  text_layer_set_text_color(s_time_meridian, GColorWhite);
  
  if(persist_read_int(KEY_TEXT_COLOR)) {
    int text_color = persist_read_int(KEY_TEXT_COLOR);
    set_text_color(text_color);
  }
  if(persist_read_int(KEY_WINDOW_BACKGROUND)) {
    int background_color = persist_read_int(KEY_WINDOW_BACKGROUND);
    set_window_background(background_color);
  }
  if(persist_read_int(KEY_MERIDIAN)) {
    is_meridian = persist_read_bool(KEY_MERIDIAN);
  }
  

  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_window_background));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_hour));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_minute));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_meridian));
  
}

static void main_window_unload(Window *window) {
  
  text_layer_destroy(s_time_hour);
  text_layer_destroy(s_time_minute);
  text_layer_destroy(s_time_meridian);
  
  fonts_unload_custom_font(s_font_opensans_50);
  fonts_unload_custom_font(s_font_opensans_20);
  
  
  bitmap_layer_destroy(s_window_background);
}

static void init() {
  
  appMessage();
  
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();
  
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}