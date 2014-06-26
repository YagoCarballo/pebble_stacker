#include <pebble.h>
#include "about.h"

/**
 * Struct to Handle the GUI
 */
static struct InstructionsUI {
	Window * window;
	Layer  * windowLayer;
	TextLayer * titleLayer;
	TextLayer * textLayer;
	ScrollLayer * scroll;
} ui;

/**
 * Initializates the GUI Elements
 */
void about_window_load (Window *window) {
	GRect windowBounds = layer_get_frame(ui.windowLayer);
	GRect titleBounds = GRect(0, 0, windowBounds.size.w, 30);
	ui.titleLayer = text_layer_create(titleBounds);
	text_layer_set_text_color(ui.titleLayer, GColorWhite);
	text_layer_set_background_color(ui.titleLayer, GColorBlack);
	text_layer_set_text(ui.titleLayer, "About");
  	text_layer_set_font(ui.titleLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.titleLayer, GTextAlignmentCenter);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.titleLayer));
	
	// Creates the Scroll
	GRect scrollBounds = GRect(0, 30, windowBounds.size.w, (windowBounds.size.h - 30));
	ui.scroll = scroll_layer_create(scrollBounds);
	scroll_layer_set_click_config_onto_window(ui.scroll, ui.window);
	layer_add_child(ui.windowLayer, scroll_layer_get_layer(ui.scroll));
	
	GRect textBounds = GRect(0, 0, scrollBounds.size.w, 2000);
	ui.textLayer = text_layer_create(textBounds);
	text_layer_set_text_color(ui.textLayer, GColorBlack);
	text_layer_set_background_color(ui.textLayer, GColorClear);
	text_layer_set_text(ui.textLayer, "Developer:\nYago Carballo (yagocarballo@gmail.com)\n\nFrameworks:\nmini-printf (https://github.com/mludvig/mini-printf)\n\nGame By...\nameiga 2014\n\n");
  	text_layer_set_font(ui.textLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  	text_layer_set_text_alignment(ui.textLayer, GTextAlignmentCenter);
	
	GSize max_size = text_layer_get_content_size(ui.textLayer);
	text_layer_set_size(ui.textLayer, max_size);
	scroll_layer_set_content_size(ui.scroll, GSize(windowBounds.size.w, max_size.h + 5));
	scroll_layer_add_child(ui.scroll, text_layer_get_layer(ui.textLayer));
}

/**
 * Handles the About Initialization
 */
void about_init (void) {
	ui.window = window_create();
	ui.windowLayer = window_get_root_layer(ui.window);
	
	window_set_window_handlers(ui.window, (WindowHandlers) {
    	.load = about_window_load
	});
	
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

/**
 * Deallocates the About on Destruction
 */
void about_deinit (void) {
	text_layer_destroy(ui.titleLayer);
	text_layer_destroy(ui.textLayer);
	scroll_layer_destroy(ui.scroll);
	layer_destroy(ui.windowLayer);
}