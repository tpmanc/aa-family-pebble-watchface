#include <pebble.h>

Window *my_window;
TextLayer *text_layer, *date_text;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static GFont time_font, date_font;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char date_buffer[12];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  strftime(date_buffer, sizeof(date_buffer), "%d.%m.%Y", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(text_layer, s_buffer);
  text_layer_set_text(date_text, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

void handle_init(void) {
  my_window = window_create();
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect img = GRect(0, 30, bounds.size.w, 120);
  
  // set background
  window_set_background_color(my_window, GColorBlack);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG);
  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(img);
  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_alignment(s_background_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Time
  // Create the TextLayer with specific bounds
  text_layer = text_layer_create(GRect(0, 5, bounds.size.w, 34));
  // Improve the layout to be more like a watchface
  time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_28));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, time_font);
  text_layer_set_text(text_layer, "00:00");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  // Date
  date_text = text_layer_create(GRect(0, bounds.size.h - 40, bounds.size.w, 34));
  date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_20));
  text_layer_set_background_color(date_text, GColorBlack);
  text_layer_set_text_color(date_text, GColorWhite);
  text_layer_set_font(date_text, date_font);
  text_layer_set_text(date_text, "25.12.2015");
  text_layer_set_text_alignment(date_text, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(date_text));
  
  window_stack_push(my_window, true);
  
  // subscribe to time
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  text_layer_destroy(date_text);
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  window_destroy(my_window);
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
