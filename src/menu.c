#include <pebble.h>
#include "menu.h"
#include "menu_level.h"
#include "instructions.h"
#include "scores.h"
#include "about.h"

/**
 * Struct to Handle the GUI
 */
static struct MenuUI {
	Window * window;
	Layer  * windowLayer;
	TextLayer * titleLayer;
	SimpleMenuLayer * menuLayer;
	SimpleMenuSection menu_sections [1];
	SimpleMenuItem menu_items [4];
} ui;

/**
 * Struct to Indicate if the Screens need to be Deallocated
 */
static struct Status {
	bool cleanMenuLevel;
	bool cleanInstructions;
	bool cleanScores;
	bool cleanAbout;
	bool difficulty;
} status;

/**
 * Cleans the Screens from memory if they need to be Cleaned
 */
void menu_window_appear(Window *window) {
	if (status.cleanMenuLevel) menu_level_deinit();
	if (status.cleanInstructions) instructions_deinit();
	if (status.cleanScores) scores_deinit();
	if (status.cleanAbout) about_deinit();
	
	status.cleanMenuLevel		= false;
	status.cleanInstructions	= false;
	status.cleanScores			= false;
	status.cleanAbout			= false;
}

void menu_select_callback(int index, void *ctx) {
	// ui.menu_items[index];
	if (index == 0) {
		instructions_init();
		status.cleanInstructions = true;
		
	} else if (index == 1) {
		status.cleanMenuLevel = true;
		menu_level_init();
		
	} else if (index == 2) {
		scores_init();
		status.cleanScores = true;
		
	} else if (index == 3) {
		about_init();
		status.cleanAbout = true;
	}
}

/**
 * Initializates the GUI Elements
 */
void menu_window_load (Window *window) {
	GRect windowBounds = layer_get_frame(ui.windowLayer);
	GRect titleBounds = GRect(0, 0, windowBounds.size.w, 30);
	ui.titleLayer = text_layer_create(titleBounds);
	text_layer_set_text_color(ui.titleLayer, GColorWhite);
	text_layer_set_background_color(ui.titleLayer, GColorBlack);
	text_layer_set_text(ui.titleLayer, "The Stacker");
  	text_layer_set_font(ui.titleLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.titleLayer, GTextAlignmentCenter);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.titleLayer));
	
	// Creates the Menu Items
	ui.menu_items[0] = (SimpleMenuItem){
    	.title = "Instructions",
    	.callback = menu_select_callback,
  	};
	
	ui.menu_items[1] = (SimpleMenuItem){
    	.title = "New Game",
    	.callback = menu_select_callback,
  	};
	
	ui.menu_items[2] = (SimpleMenuItem){
    	.title = "Scores",
    	.callback = menu_select_callback,
  	};
	
	ui.menu_items[3] = (SimpleMenuItem){
    	.title = "About",
    	.callback = menu_select_callback,
  	};
	
	// Creates the Main Section
	ui.menu_sections[0] = (SimpleMenuSection){
    	.num_items = 4,
    	.items = ui.menu_items,
  	};
	
	// Creates the Menu Layer
	GRect menuBounds = GRect(0, 30, windowBounds.size.w, (windowBounds.size.h - 30));
	ui.menuLayer = simple_menu_layer_create(menuBounds, ui.window, ui.menu_sections, 1, NULL);
	simple_menu_layer_set_selected_index(ui.menuLayer, 1, false);
	layer_add_child(ui.windowLayer, simple_menu_layer_get_layer(ui.menuLayer));
	
	status.cleanMenuLevel		= false;
	status.cleanInstructions	= false;
	status.cleanScores			= false;
	status.cleanAbout			= false;
}

/**
 * Handles the Menu Initialization
 */
void menu_init (void) {
	// Creates the Window and saves the Layer for later Use
	ui.window = window_create();
	ui.windowLayer = window_get_root_layer(ui.window);
	
	// Adds the Window Listeners
	window_set_window_handlers(ui.window, (WindowHandlers) {
    	.load = menu_window_load,
    	.appear = menu_window_appear
	});
	
	// Makes the Window Vissible with an animation
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

// Deallocates Memory When Destroyed
void menu_deinit (void) {
	// Cleans the Views if they need to be cleaned
	if (status.cleanMenuLevel) menu_level_deinit();
	if (status.cleanInstructions) instructions_deinit();
	if (status.cleanScores) scores_deinit();
	if (status.cleanAbout) about_deinit();
	
	status.cleanMenuLevel		= false;
	status.cleanInstructions	= false;
	status.cleanScores			= false;
	status.cleanAbout			= false;
	status.difficulty			= false;
	
	// Destroy Used Layers
	text_layer_destroy(ui.titleLayer);
	simple_menu_layer_destroy(ui.menuLayer);
	layer_destroy(ui.windowLayer);
}
