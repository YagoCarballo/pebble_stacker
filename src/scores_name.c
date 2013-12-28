#include <pebble.h>
#include "scores_name.h"
#include "scores.h"

static const char letters [26][2] = {
	"A", "B", "C", "D", "E", "F", "G", 
	"H", "I", "J", "K", "L", "M", "N", 
	"O", "P", "Q", "R", "S", "T", "U", 
	"V", "W", "X", "Y", "Z"
};

static struct ScoresNameUI {
	Window * window;
	Layer  * windowLayer;
	TextLayer * titleLayer;
	TextLayer * selectorLayer;
	TextLayer * activeLayer;
	TextLayer * letterLayers [3][6];
	ActionBarLayer * actionBarLayer;
	GBitmap * frontArrow;
	GBitmap * backArrow;
	GBitmap * nextArrow;
} ui;

static struct ScoresNameStatus {
	char * grid [6][3];
	int indexes [3];
	int pos;
	int points;
	bool cleanScores;
} status;

/**
 * Handles the Up Button Presses
 */
void scores_name_up_click_handler(ClickRecognizerRef recognizer, void *context) {
	status.indexes[status.pos]--;
	if (status.indexes[status.pos] < 0)
		status.indexes[status.pos] = 25;
	
	int c, index = 0;
	for (c=5;c>=0;c--) {
		index = status.indexes[status.pos] - (5 - c);
		if (index < 0) index = 25 + (index + 1);
		text_layer_set_text(ui.letterLayers[c][status.pos], letters[index]);
	}
}

/**
 * Handles the Middle Button Presses
 */
void scores_name_select_click_handler(ClickRecognizerRef recognizer, void *context) {
	status.pos++;
	if (status.pos >= 3) {
		char name [3] = "";
		strcat(name, letters[status.indexes[0]]);
		strcat(name, letters[status.indexes[1]]);
		strcat(name, letters[status.indexes[2]]);
		scores_name_deinit();
		scores_init_with_points_and_name(status.points, name);
		status.cleanScores = true;
		
	} else {
		text_layer_destroy(ui.activeLayer);
		GRect activeBounds = GRect(20, 110, 60, 1);
		if (status.pos == 2) activeBounds = GRect(20, 148, 60, 1);
		ui.activeLayer = text_layer_create(activeBounds);
		text_layer_set_text_color(ui.activeLayer, GColorClear);
		text_layer_set_background_color(ui.activeLayer, GColorBlack);
		text_layer_set_text(ui.activeLayer, "");
	  	text_layer_set_font(ui.activeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	  	text_layer_set_text_alignment(ui.activeLayer, GTextAlignmentCenter);
		layer_add_child(ui.windowLayer, text_layer_get_layer(ui.activeLayer));
	}
}

/**
 * Handles the Down Button Presses
 */
void scores_name_down_click_handler(ClickRecognizerRef recognizer, void *context) {
	status.indexes[status.pos]++;
	if (status.indexes[status.pos] >= 26) 
		status.indexes[status.pos] = 0;
	
	int c, index = 0;
	for (c=5;c>=0;c--) {
		index = status.indexes[status.pos] - (5 - c);
		if (index < 0) index = 25 + (index + 1);
		text_layer_set_text(ui.letterLayers[c][status.pos], letters[index]);
	}
}

/**
 * Adds listeners for the Buttons
 */
static void scores_name_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, scores_name_select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, 	scores_name_up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, 	scores_name_down_click_handler);
}

void scores_name_window_appear (Window *window) {}

void scores_name_window_load (Window *window) {
	GRect windowBounds = layer_get_frame(ui.windowLayer);
	GRect titleBounds = GRect(0, 0, 120, 30);
	ui.titleLayer = text_layer_create(titleBounds);
	text_layer_set_text_color(ui.titleLayer, GColorWhite);
	text_layer_set_background_color(ui.titleLayer, GColorBlack);
	text_layer_set_text(ui.titleLayer, "  Enter Name");
  	text_layer_set_font(ui.titleLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  	text_layer_set_text_alignment(ui.titleLayer, GTextAlignmentLeft);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.titleLayer));
	
	ui.frontArrow = gbitmap_create_with_resource(RESOURCE_ID_FRONT_ARROW);
	ui.backArrow = gbitmap_create_with_resource(RESOURCE_ID_BACK_ARROW);
	ui.nextArrow = gbitmap_create_with_resource(RESOURCE_ID_NEXT_ARROW);
	
	ui.actionBarLayer = action_bar_layer_create();
	action_bar_layer_add_to_window(ui.actionBarLayer, ui.window);
	action_bar_layer_set_click_config_provider(ui.actionBarLayer, scores_name_click_config_provider);
	action_bar_layer_set_icon(ui.actionBarLayer, BUTTON_ID_UP, ui.backArrow);
	action_bar_layer_set_icon(ui.actionBarLayer, BUTTON_ID_DOWN, ui.frontArrow);
	action_bar_layer_set_icon(ui.actionBarLayer, BUTTON_ID_SELECT, ui.nextArrow);
	
	GRect selectorBounds = GRect(100, 30, 20, (windowBounds.size.h - 30));
	ui.selectorLayer = text_layer_create(selectorBounds);
	text_layer_set_text_color(ui.selectorLayer, GColorClear);
	text_layer_set_background_color(ui.selectorLayer, GColorBlack);
	text_layer_set_text(ui.selectorLayer, "");
  	text_layer_set_font(ui.selectorLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.selectorLayer, GTextAlignmentCenter);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.selectorLayer));
	
	GRect activeBounds = GRect(20, 70, 60, 1);
	ui.activeLayer = text_layer_create(activeBounds);
	text_layer_set_text_color(ui.activeLayer, GColorClear);
	text_layer_set_background_color(ui.activeLayer, GColorBlack);
	text_layer_set_text(ui.activeLayer, "");
  	text_layer_set_font(ui.activeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.activeLayer, GTextAlignmentCenter);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.activeLayer));
	
	int r, c, x = 0, y = 0, index = 0;
	for (c=5;c>=0;c--) {
		x = (c + 1) * 5 + (c * 15);
		
		for (r=0;r<3;r++) {
			y = (r + 1) * 40;
			index = status.indexes[r] - (5 - c);
			if (index < 0) index = 25 + (index + 1);
			
			GRect letterBounds = GRect(x, y, 30, 30);
			ui.letterLayers[c][r] = text_layer_create(letterBounds);
			text_layer_set_text_color(ui.letterLayers[c][r], ((c >= 5)?GColorWhite:GColorBlack));
			text_layer_set_background_color(ui.letterLayers[c][r], GColorClear);
			text_layer_set_text(ui.letterLayers[c][r], letters[index]);
			text_layer_set_font(ui.letterLayers[c][r], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
			text_layer_set_text_alignment(ui.letterLayers[c][r], GTextAlignmentLeft);
			layer_add_child(ui.windowLayer, text_layer_get_layer(ui.letterLayers[c][r]));
		}
	}
}

void scores_name_init (int points) {
	// Creates the Window and saves the Layer for later Use
	ui.window		= window_create();
	ui.windowLayer	= window_get_root_layer(ui.window);
	
	status.points		= points;
	status.cleanScores	= false;
	status.pos			= 0;
	
	int i;
	for (i=0;i<3;i++) {
		status.indexes[i] = 0;
	}
	
	// Adds the Window Listeners
	window_set_window_handlers(ui.window, (WindowHandlers) {
    	.load = scores_name_window_load,
		.appear = scores_name_window_appear
	});
	
	// Adds the Button Listeners
	// window_set_click_config_provider(ui.window, scores_name_click_config_provider);
	
	// Makes the Window Vissible with an animation
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

void scores_name_deinit (void) {
	if (status.cleanScores) scores_deinit();
	
	window_stack_remove(ui.window, true);
	text_layer_destroy(ui.titleLayer);
	text_layer_destroy(ui.selectorLayer);
	text_layer_destroy(ui.activeLayer);
	gbitmap_destroy(ui.frontArrow);
	gbitmap_destroy(ui.backArrow);
	gbitmap_destroy(ui.nextArrow);
	
	int r, c;
	for (c=0;c<6;c++) {
		for (r=0;r<3;r++) {
			text_layer_destroy(ui.letterLayers[c][r]);
		}
	}
	
	action_bar_layer_destroy(ui.actionBarLayer);
	layer_destroy(ui.windowLayer);
	window_destroy(ui.window);
}