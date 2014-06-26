#include <pebble.h>
#include "scores.h"
#include "mini-printf.h"

static const uint32_t SCORES_NAME_1 = 10;
static const uint32_t SCORES_NAME_2 = 20;
static const uint32_t SCORES_NAME_3 = 30;
static const uint32_t SCORES_POINTS_1 = 11;
static const uint32_t SCORES_POINTS_2 = 22;
static const uint32_t SCORES_POINTS_3 = 33;

static struct ScoresUI {
	Window * window;
	Layer  * windowLayer;
	TextLayer * titleLayer;
	TextLayer * scoresLayer;
} ui;

static struct ScoresStatus {
	bool exist [3];
	int points [3];
	int lastPoints;
	
	char name [3][5];
	char lastName [5];
	
	char scores [50];
	bool update;
	bool new_data;
} status;

void scores_window_appear (Window *window) {
	status.exist[0] = false;
	status.exist[1] = false;
	status.exist[2] = false;
	status.update   = false;
	strcpy(status.name[0], "---");
	strcpy(status.name[1], "---");
	strcpy(status.name[2], "---");
	status.points[0] = 0;
	status.points[1] = 0;
	status.points[2] = 0;
	
	if (persist_exists(SCORES_NAME_1) && persist_exists(SCORES_POINTS_1)) {
		persist_read_string(SCORES_NAME_1, status.name[0], (sizeof(char) * 5));
		status.points[0] = persist_read_int(SCORES_POINTS_1);
		status.exist[0] = true;
	}
	
	if (persist_exists(SCORES_NAME_2) && persist_exists(SCORES_POINTS_2)) {
		persist_read_string(SCORES_NAME_2, status.name[1], (sizeof(char) * 5));
		status.points[1] = persist_read_int(SCORES_POINTS_2);
		status.exist[1] = true;
	}
	
	if (persist_exists(SCORES_NAME_3) && persist_exists(SCORES_POINTS_3)) {
		persist_read_string(SCORES_NAME_3, status.name[2], (sizeof(char) * 5));
		status.points[2] = persist_read_int(SCORES_POINTS_3);
		status.exist[2] = true;
	}
	
	if (status.new_data) {
		if (status.exist[0] && status.exist[1] && status.exist[2]) {
			if (status.lastPoints >= status.points[0]) {
				status.points[2] = status.points[1];
				status.points[1] = status.points[0];
				status.points[0] = status.lastPoints;
				strcpy(status.name[2], status.name[1]);
				strcpy(status.name[1], status.name[0]);
				strcpy(status.name[0], status.lastName);
				status.update    = true;
			
			} else if (status.lastPoints >= status.points[1]) {
				status.points[2] = status.points[1];
				status.points[1] = status.lastPoints;
				strcpy(status.name[2], status.name[1]);
				strcpy(status.name[1], status.lastName);
				status.update    = true;
			
			} else if (status.lastPoints >= status.points[2]) {
				status.points[2] = status.lastPoints;
				strcpy(status.name[2], status.lastName);
				status.update    = true;
			}
		} else {
			status.update = true;
			if (!status.exist[0]) {
				status.points[0] = status.lastPoints;
				strcpy(status.name[0], status.lastName);
			
			} else if (!status.exist[1]) {
				status.points[1] = status.lastPoints;
				strcpy(status.name[1], status.lastName);
			
			} else if (!status.exist[2]) {
				status.points[2] = status.lastPoints;
				strcpy(status.name[2], status.lastName);
			}
		}
	}
	
	if (status.update) {
		persist_write_string(SCORES_NAME_1, status.name[0]);
		persist_write_string(SCORES_NAME_2, status.name[1]);
		persist_write_string(SCORES_NAME_3, status.name[2]);
		persist_write_int(SCORES_POINTS_1, status.points[0]);
		persist_write_int(SCORES_POINTS_2, status.points[1]);
		persist_write_int(SCORES_POINTS_3, status.points[2]);
	}
	
	strcpy(status.scores, "");
	mini_snprintf(status.scores, 50, 
		"%s      %d \n%s      %d \n%s      %d \n", 
		status.name[0], status.points[0], 
		status.name[1], status.points[1], 
		status.name[2], status.points[2]
	);
	text_layer_set_text(ui.scoresLayer, status.scores);
}

void scores_window_load (Window *window) {
	GRect windowBounds = layer_get_frame(ui.windowLayer);
	GRect titleBounds = GRect(0, 0, windowBounds.size.w, 30);
	ui.titleLayer = text_layer_create(titleBounds);
	text_layer_set_text_color(ui.titleLayer, GColorWhite);
	text_layer_set_background_color(ui.titleLayer, GColorBlack);
	text_layer_set_text(ui.titleLayer, "Scores");
  	text_layer_set_font(ui.titleLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.titleLayer, GTextAlignmentCenter);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.titleLayer));
	
	GRect scoresBounds = GRect(0, 30, windowBounds.size.w, (windowBounds.size.h - 30));
	ui.scoresLayer = text_layer_create(scoresBounds);
	text_layer_set_text_color(ui.scoresLayer, GColorBlack);
	text_layer_set_background_color(ui.scoresLayer, GColorClear);
	text_layer_set_text(ui.scoresLayer, "");
  	text_layer_set_font(ui.scoresLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.scoresLayer, GTextAlignmentCenter);
	layer_add_child(ui.windowLayer, text_layer_get_layer(ui.scoresLayer));
}

/**
 * Handles the Middle Button Presses
 */
static void scores_select_click_handler (ClickRecognizerRef recognizer, void *context) {
	window_stack_pop(true);
}

/**
 * Adds listeners for the Buttons
 */
static void scores_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, scores_select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, scores_select_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, scores_select_click_handler);
}

void scores_init_with_points_and_name (int points, char * name) {
	// Creates the Window and saves the Layer for later Use
	ui.window		= window_create();
	ui.windowLayer	= window_get_root_layer(ui.window);
	
	status.lastPoints	= points;
	strcpy(status.lastName, name);
	
	if (strcmp(status.lastName, "---") == 0) {
		status.new_data = false;
	} else {
		status.new_data = true;
	}
	
	// Adds the Window Listeners
	window_set_window_handlers(ui.window, (WindowHandlers) {
    	.load = scores_window_load,
		.appear = scores_window_appear
	});
	
	// Adds the Button Listeners
	window_set_click_config_provider(ui.window, scores_click_config_provider);
	
	// Makes the Window Vissible with an animation
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

void scores_init (void) {
	scores_init_with_points_and_name(0, "---");
}

void scores_deinit (void) {
	text_layer_destroy(ui.titleLayer);
	text_layer_destroy(ui.scoresLayer);
}