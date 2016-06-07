#include <pebble.h>
#include <pebble-bluetooth-icon.h>
#include <pebble-events/pebble-events.h>

typedef struct BluetoothIconSettings {
  bool vibe_disconnect;
  bool vibe_connect;
  bool is_connected;
  GRect circle_layer_rect;
  GRect icon_layer_rect;
  GColor color_connected_icon;
  GColor color_connected_circle;
  GColor color_disconnected_icon;
  GColor color_disconnected_circle;
} BluetoothIconSettings;

static Layer *s_bluetooth_layer, *s_bluetooth_icon_layer, *s_bluetooth_circle_layer;
static EventHandle s_handle;

static GPath *s_bluetooth_path_ptr = NULL;
static GPathInfo BT_PATH_INFO = {
  .num_points = 18,
  .points = (GPoint []) {{0, 1}, {2, 3}, {3, 3}, {3, 0}, {4, 0}, {6, 2}, {4, 4}, {6, 6}, {4, 8}, {3, 8}, {3, 5}, {2, 5}, {0, 7}, {2, 5}, {3, 5}, {3, 3}, {2, 3}, {0, 1}}
};

// Draw the circle
static void bluetooth_circle_layer_update_callback(Layer *layer, GContext* ctx) {
  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);
  graphics_context_set_fill_color(ctx, (data->is_connected ? data->color_connected_circle : data->color_disconnected_circle));
  graphics_fill_circle(ctx, GPoint(5, 5), 5);
}

// Draw the icon
static void bluetooth_icon_layer_update_callback(Layer *layer, GContext* ctx) {
  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);
  graphics_context_set_stroke_color(ctx, (data->is_connected ? data->color_connected_icon : data->color_disconnected_icon));
  gpath_draw_outline(ctx, s_bluetooth_path_ptr);
}

// Vibrate if required and update the icon
static void bluetooth_connection_callback(bool connected) {
  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);

  if(data->vibe_disconnect && (data->is_connected && !connected)) {
    // Just disconnected
    vibes_double_pulse();
  } else if (data->vibe_connect && (!data->is_connected && connected)) {
    // Just connected
    vibes_short_pulse();
  }

  data->is_connected = connected;
  layer_mark_dirty(s_bluetooth_circle_layer);
  layer_mark_dirty(s_bluetooth_icon_layer);
}

// Force the icon to refresh
static void bluetooth_refresh() {
  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);
  bluetooth_connection_callback(data->is_connected);
}

// Create a set of default settings.
static void bluetooth_set_defaults(BluetoothIconSettings *data) {
  data->is_connected = true;
  data->circle_layer_rect = GRect(0, 0, 13, 13);
  data->icon_layer_rect = GRect(2, 1, 7, 9);
  data->vibe_disconnect = true;
  data->vibe_connect = true;
  data->color_connected_icon = GColorWhite;
  data->color_connected_circle = GColorBlue;
  data->color_disconnected_icon = GColorRed;
  data->color_disconnected_circle = GColorWhite;
}

// Create a new bluetooth layer and start monitoring.
BluetoothLayer* bluetooth_layer_create() {
  s_bluetooth_layer = layer_create_with_data(GRect(0, 0, 13, 13), sizeof(BluetoothIconSettings));

  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);
  bluetooth_set_defaults(data);

  s_bluetooth_circle_layer = layer_create(data->circle_layer_rect);
  layer_set_update_proc(s_bluetooth_circle_layer, bluetooth_circle_layer_update_callback);
  layer_add_child(s_bluetooth_layer, s_bluetooth_circle_layer);

  s_bluetooth_path_ptr = gpath_create(&BT_PATH_INFO);

  s_bluetooth_icon_layer = layer_create(data->icon_layer_rect);
  layer_set_update_proc(s_bluetooth_icon_layer, bluetooth_icon_layer_update_callback);
  layer_add_child(s_bluetooth_layer, s_bluetooth_icon_layer);

  bluetooth_connection_callback(connection_service_peek_pebble_app_connection());
  s_handle = events_connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_connection_callback
  });

  return s_bluetooth_layer;
}

// Destroy the layer and its contents.
void bluetooth_layer_destroy(BluetoothLayer *bluetooth_layer) {
  events_connection_service_unsubscribe(s_handle);
  gpath_destroy(s_bluetooth_path_ptr);
  s_bluetooth_path_ptr = NULL;

  layer_remove_from_parent(s_bluetooth_circle_layer);
  layer_remove_from_parent(s_bluetooth_icon_layer);
  layer_remove_from_parent(s_bluetooth_layer);

  layer_destroy(s_bluetooth_circle_layer);
  layer_destroy(s_bluetooth_icon_layer);
  layer_destroy(s_bluetooth_layer);
}

// Disable vibrate on disconnect
void bluetooth_vibe_disconnect(bool vibe) {
  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);
  data->vibe_disconnect = vibe;
}

// Disable vibrate on disconnect
void bluetooth_vibe_connect(bool vibe) {
  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);
  data->vibe_connect = vibe;
}

// Adjust the colors of the bluetooth icon.
void bluetooth_set_colors(GColor connected_circle, GColor connected_icon, GColor disconnected_circle, GColor disconnected_icon) {
  BluetoothIconSettings *data = layer_get_data(s_bluetooth_layer);
  data->color_connected_circle = connected_circle;
  data->color_connected_icon = connected_icon;
  data->color_disconnected_circle = disconnected_circle;
  data->color_disconnected_icon = disconnected_icon;
  bluetooth_refresh();
}

// Move the bluetooth icon position on screen (x, y).
void bluetooth_set_position(GPoint position) {
  GRect frame = layer_get_frame(s_bluetooth_layer);
  frame.origin = position;
  layer_set_frame(s_bluetooth_layer, frame);
}
