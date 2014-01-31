#include <pebble.h>
#include "menu.h"

static const int SPLASH_TIME = 2000;

bool menuStarted = false;
Window      * window;
AppTimer    * timer;
GBitmap     * splashImage;
BitmapLayer * splashLayer;

void splash_deinit () {
	if (timer != NULL) app_timer_cancel (timer); // TODO: Look why this breaks the App on the v2.0-RC
	window_stack_remove(window, true);
	gbitmap_destroy(splashImage);
	bitmap_layer_destroy(splashLayer);
	window_destroy(window);
	free(timer);
}

void splash_timer_callback (void * data) {
	timer = NULL;
	splash_deinit();
	
	menuStarted = true;
	menu_init();
}

void init_splash () {
	window = window_create();
	Layer * windowLayer = window_get_root_layer(window);
	
	splashImage = gbitmap_create_with_resource(RESOURCE_ID_AMEIGA_SPLASH);
	GRect bounds = layer_get_frame(windowLayer);
	splashLayer = bitmap_layer_create(bounds);
	
	bitmap_layer_set_bitmap(splashLayer, splashImage);
	bitmap_layer_set_alignment(splashLayer, GAlignCenter);
	bitmap_layer_set_background_color(splashLayer, GColorBlack);
	layer_add_child(windowLayer, bitmap_layer_get_layer(splashLayer));
	
	const bool animated = true;
	window_stack_push(window, animated);
	
	timer = app_timer_register(SPLASH_TIME, splash_timer_callback, NULL);
}

/**
 * Entry Point of the PebbleApp
 */
int main(void) {
	init_splash();
	app_event_loop();
	
	if (menuStarted) menu_deinit();
	else splash_deinit();
	return 0;
}
