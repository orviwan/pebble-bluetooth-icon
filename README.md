# pebble-bluetooth-icon

Pebble library for easily adding a bluetooth connection icon to your app.

![Screenshot of library in action](https://raw.githubusercontent.com/orviwan/pebble-bluetooth-icon/master/info/animation.gif)

## Usage

`pebble package install pebble-bluetooth-icon`

````c
// This is a simple example, demonstrating the basic usage.
#include "bluetooth-icon/bluetooth-icon.h"

static BluetoothLayer *s_bluetooth_layer;

static void window_load(Window *window) {
  s_bluetooth_layer = bluetooth_layer_create();
  layer_add_child(window_layer, s_bluetooth_layer);
}

static void window_unload(Window *window) {
  bluetooth_layer_destroy(s_bluetooth_layer);
}

````

## Additional Settings

Position the bluetooth icon on screen.

    void bluetooth_set_position(GPoint position);

Vibrate on disconnect (default: true, double pulse).

    void bluetooth_vibe_disconnect(bool vibe);

Vibrate on connect (default: true, short pulse).

    void bluetooth_vibe_connect(bool vibe);

Override the color scheme.

    void bluetooth_set_colors(GColor connected_circle, GColor connected_icon, GColor disconnected_circle, GColor disconnected_icon);
