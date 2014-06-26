#include <pebble.h>
#include "board.h"

/**
 * Struct to Handle the GUI
 */
static struct MenuLevelUI {
	Window * window;
	Layer  * windowLayer;
	TextLayer * titleLayer;
	SimpleMenuLayer * menuLayer;
	SimpleMenuSection menu_sections [1];
	SimpleMenuItem menu_items [3];
} ui;

/**
 * Struct to Indicate if the Screens need to be Deallocated
 */
static struct Status {
	bool cleanGame;
	bool difficulty;
} status;

/**
 * Cleans the Screens from memory if they need to be Cleaned
 */
void menu_level_window_appear(Window *window) {
	if (status.cleanGame) game_deinit();
	status.cleanGame = false;
}

void menu_level_select_callback(int index, void *ctx) {
	window_stack_remove(ui.window, true);
	
	// ui.menu_items[index];
	if (index == 0) {
		status.cleanGame = true;
		game_init(HARD);
		
	} else if (index == 1) {
		status.cleanGame = true;
		game_init(NORMAL);
		
	} else if (index == 2) {
		status.cleanGame = true;
		game_init(EASY);
	}
}

/**
 * Initializates the GUI Elements
 */
void menu_level_window_load (Window *window) {
	GRect windowBounds = layer_get_frame(ui.windowLayer);
	GRect titleBounds = GRect(0, 0, windowBounds.size.w, 30);
	ui.titleLayer = text_layer_create(titleBounds);
	text_layer_set_text_color(ui.titleLayer, GColorWhite);
	text_layer_set_background_color(ui.titleLayer, GColorBlack);
	text_layer_set_text(ui.titleLayer, "Level");
  	text_layer_set_font(ui.titleLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.titleLayer, GTextAlignmentCenter);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.titleLayer));
	
	// Creates the Menu Items
	ui.menu_items[0] = (SimpleMenuItem){
    	.title = "Hard",
    	.callback = menu_level_select_callback,
  	};
	
	ui.menu_items[1] = (SimpleMenuItem){
    	.title = "Normal",
    	.callback = menu_level_select_callback,
  	};
	
	ui.menu_items[2] = (SimpleMenuItem){
    	.title = "Easy",
    	.callback = menu_level_select_callback,
  	};
	
	// Creates the Main Section
	ui.menu_sections[0] = (SimpleMenuSection){
    	.num_items = 3,
    	.items = ui.menu_items,
  	};
	
	// Creates the Menu Layer
	GRect menuBounds = GRect(0, 30, windowBounds.size.w, (windowBounds.size.h - 30));
	ui.menuLayer = simple_menu_layer_create(menuBounds, ui.window, ui.menu_sections, 1, NULL);
	simple_menu_layer_set_selected_index(ui.menuLayer, 1, false);
	layer_add_child(ui.windowLayer, simple_menu_layer_get_layer(ui.menuLayer));
	
	status.cleanGame = false;
}

/**
 * Handles the Menu Initialization
 */
void menu_level_init (void) {
	// Creates the Window and saves the Layer for later Use
	ui.window = window_create();
	ui.windowLayer = window_get_root_layer(ui.window);
	
	// Adds the Window Listeners
	window_set_window_handlers(ui.window, (WindowHandlers) {
    	.load = menu_level_window_load,
    	.appear = menu_level_window_appear
	});
	
	// Makes the Window Vissible with an animation
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

// Deallocates Memory When Destroyed
void menu_level_deinit (void) {
	// Cleans the Views if they need to be cleaned
	if (status.cleanGame) game_deinit();
	
	status.cleanGame	= false;
	status.difficulty	= false;
	
	// Destroy Used Layers
	text_layer_destroy(ui.titleLayer);
	simple_menu_layer_destroy(ui.menuLayer);
	layer_destroy(ui.windowLayer);
}
