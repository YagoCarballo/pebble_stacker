#include <pebble.h>
#include "board.h"
#include "mini-printf.h"
#include "scores_name.h"

static const int TIME_INTERVAL	= 100; // Default Speed (milliseconds)

// Static Variables Used for the GUI Generation
static const int SCREEN_WIDTH	= 144;
static const int SCREEN_HEIGHT	= 168;
static const int LEFT_OFFSET	= 2;
static const int TOP_OFFSET		= 2;
static const int CELL_WIDTH		= 16;
static const int CELL_HEIGHT	= 6;
static const int CELL_PADDING	= 2;
static const int ROWS			= 15;
static const int COLS			= 7;

/**
 * Struct to Store the User Interface Info
 */
static struct GameUi {
	Window * window;			// The Window
	Layer  * windowLayer; 		// The Window Layer
	Layer  * boardLayer;		// The Game Layer
	TextLayer * debugLayer;		// The Points / Debug Layer
	TextLayer * messageLayer;	// The Game Over Layer (hidden until the end)
} ui;

/**
 * Struct to Store the Game Info
 */
static struct GameStatus {
	int pos;				// The Current Position of the Middle Stick
	int size;				// The Current Size
	int level;				// The Current Row
	int speed;				// The Current Speed
	int points;				// The Accumulated Points
	int difficulty;			// The Difficulty of the Game
	bool cleanScores;		// The Scores View Needs to be Deallocated
	bool first;				// First Game?, others...
	bool end;				// Has the Game Ended
	bool left;				// The Current Direction of the Sticks
	bool zigzag;			// Do the Sticks Change Direction on new Generation??
	bool lastLeft;			// The Last Direction When Generated
	int sizes [15];			// The Number of Sticks for Each Row
	int speeds [6];			// The Speeds for the Difficulty
	GPoint * boxes;			// The Moving Group of Sticks
	AppTimer * timer;		// The Timer that moves the Sticks
	GPoint * storage [15];	// All the Frozen Sticks
} status;

/**
 * Struct used to concatenate strings with integers
 */
static struct Debug {
	char text[100];
} debug;

/**
 * Translates a row/col into pixels (a GRect in the Screen)
 */
GRect parseCell (int _row, int _col) {
	int x = (LEFT_OFFSET + CELL_PADDING + ((_col - 1) * (CELL_PADDING + CELL_WIDTH +  CELL_PADDING)));
	int y = (TOP_OFFSET + CELL_PADDING + ((_row - 1) * (CELL_PADDING + CELL_HEIGHT +  CELL_PADDING)));
	int w = CELL_WIDTH;
	int h = CELL_HEIGHT;
	return GRect (x, y, w, h);
}

/**
 * This handles the movements of the moving Sticks
 */
void move () {
	if (!status.end) {
		if (status.left) {
			status.pos--;
			if (status.pos < 1) {
				status.left = false;
				move();
				
			} else {
				switch (status.size) {
				case 3:
					status.boxes[0].y = status.pos - 1;
					status.boxes[1].y = status.pos + 0;
					status.boxes[2].y = status.pos + 1;
					break;
				case 2:
					status.boxes[0].y = status.pos - 1;
					status.boxes[1].y = status.pos + 0;
					break;
				case 1:
					status.boxes[0].y = status.pos - 1;
					break;
				default:
					break;
				}
			}
			
		} else {
			status.pos++;
			if (status.pos > COLS) {
				status.left = true;
				move();
				
			} else {
				switch (status.size) {
				case 3:
					status.boxes[0].y = status.pos - 1;
					status.boxes[1].y = status.pos - 0;
					status.boxes[2].y = status.pos + 1;
					break;
				case 2:
					status.boxes[0].y = status.pos - 0;
					status.boxes[1].y = status.pos + 1;
					break;
				case 1:
					status.boxes[0].y = status.pos + 1;
					break;
				default:
					break;
				}
			}
		}
	}
}

/**
 * This handles the game logic (the changes of row, the difficulty, the end of game, points, etc...)
 */
void nextLevel () {
	if (status.level > 0) {
		int next = 0;
		GPoint * stay = malloc(sizeof(GPoint) * status.size);
		
		if (status.level < ROWS) {
			int col, c_store;
			for (col = 0;col<status.size;col++) {
				for (c_store = 0;c_store<status.sizes[status.level];c_store++) {
					if (status.boxes[col].y == status.storage[status.level][c_store].y) {
						stay[next] = status.boxes[col];
						next++;
						break;
					}
				}
			}
			
		} else {
			int col;
			for (col=0;col<status.size;col++) {
				stay[col] = GPoint(status.boxes[col].x, status.boxes[col].y);
				next++;
			}
		}
		
		// DEBUG:
		// mini_snprintf(debug.text, 50, "Debug Mode\nStay (%d): \nRow: %d, \nCol: %d", next, stay[0].x, stay[0].y);
		// text_layer_set_text(ui.debugLayer, debug.text);
		
		if (next > 0) {
			if (status.level == 1) {
				status.end = true;
				text_layer_set_text(ui.messageLayer, "You Win!!");
				layer_set_hidden(text_layer_get_layer(ui.messageLayer), false);
				free(stay);
				return;
			}
			
			status.size = next;
			status.storage[status.level-1] = malloc(sizeof(GPoint) * next);
			status.sizes[status.level-1] = next;
			
			int col;
			for (col=0;col<next;col++) {
				status.storage[status.level-1][col] = GPoint(stay[col].x, stay[col].y);
			}
			
			status.level--;
			if (status.level < 13 && next >= 3) {
				next--;
				status.size = next;
			} else if (status.level < 6 && next >= 2) {
				next--;
				status.size = next;
			}
			
			if (status.level <= 12) {
				status.zigzag = true;
			}
			
			if (!status.zigzag) {
				status.lastLeft = true;
				for (col=1;col<=next;col++) {
					status.boxes[col-1] = GPoint(status.level, col);
				}
				
			} else {
				if (status.lastLeft) {
					status.left = false;
					status.lastLeft = false;
					int pos = 0;
					for (col=COLS;col>=(COLS-next);col--) {
						status.boxes[pos] = GPoint(status.level, col);
						pos++;
					}
				} else {
					status.left = true;
					status.lastLeft = true;
					for (col=1;col<=next;col++) {
						status.boxes[col-1] = GPoint(status.level, col);
					}
				}
			}
			
		} else {
			status.end = true;
			text_layer_set_text(ui.messageLayer, "Game Over");
			layer_set_hidden(text_layer_get_layer(ui.messageLayer), false);
		}
		
		status.points += (status.size * status.difficulty);
		free(stay);
		
	} else {
		status.end = true;
		text_layer_set_text(ui.messageLayer, "Game Over");
		layer_set_hidden(text_layer_get_layer(ui.messageLayer), false);
	}
}

/**
 * This Handles the Drawing of the Board Layer
 */
void boxDraw (Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color (ctx, GColorBlack);
	
	int i;
	for (i=0;i<status.size;i++) {
		GRect bounds = parseCell (status.boxes[i].x, status.boxes[i].y);
		graphics_fill_rect(ctx, bounds, 0, GCornerNone);
	}
	
	if (status.level < ROWS) {
		int r, c;
		for (r=(ROWS-1);r>(status.level-1);r--) {
			if (status.storage[r] != NULL) {
				for (c=0;c<status.sizes[r];c++) {
					GRect bounds = parseCell (status.storage[r][c].x, status.storage[r][c].y);
					graphics_fill_rect(ctx, bounds, 0, GCornerNone);
				}
			}
			
			// DEBUG: 
			// mini_snprintf(debug.text, 60, "Debug Mode\nLevel: %d\nSize: %d\n", status.level, status.sizes[r]);
			// text_layer_set_text(ui.debugLayer, debug.text);
		}
	}
}

/**
 * This Handles the Timer Calls
 */
void timer_callback (void *data) {
	if (status.end == false) {
		// Cancel the timer
		free(status.timer);
		status.timer = NULL;
		
		move(); // moves the sticks
		layer_mark_dirty(ui.boardLayer); // Marks the Layer as dirty so it is updated on the next ui refresh
		
		// Prints the Points into the Screen
		mini_snprintf(debug.text, 30, "Points: %d", status.points);
		text_layer_set_text(ui.debugLayer, debug.text);
		
		// Changes the Speed to increase difficulty
		if (status.level >= 11) {
			status.speed = status.speeds[5];
		} else if (status.level < 2) {
			status.speed = status.speeds[0];
		} else if (status.level < 3) {
			status.speed = status.speeds[1];
		} else if (status.level < 5) {
			status.speed = status.speeds[2];
		} else if (status.level < 7) {
			status.speed = status.speeds[3];
		} else if (status.level < 11) {
			status.speed = status.speeds[4];
		}
		
		// The timer needs to be called again, as it is setted to NULL after the first Run
		status.timer = app_timer_register(status.speed, timer_callback, NULL);
	}
}

void do_some_cleanup () {
	if (status.cleanScores) {
		scores_name_deinit();
		status.cleanScores = false;
	}
	
	if (status.timer != NULL) app_timer_cancel(status.timer); // TODO: Look why this breaks the App on the v2.0-RC
	free(status.timer);
	free(status.boxes);
	
	int r;
	for (r=0;r<ROWS;r++) {
		if (status.sizes[r] > 0) {
			free(status.storage[r]);
		}
	}
}

/**
 * Resets / Initializes all the Game Variables
 */
void window_appear(Window *window) {
	APP_LOG(APP_LOG_LEVEL_INFO, "---> APPEARED !!!!");
	if (!status.first) do_some_cleanup();
	
	status.pos			= 0;
	status.size			= 3;
	status.points		= 0;
	status.speed		= TIME_INTERVAL;
	status.level		= ROWS;
	status.left			= true;
	status.first		= false;
	status.end			= false;
	status.zigzag		= false;
	status.lastLeft		= false;
	status.boxes		= malloc(sizeof(GPoint) * 3);
	status.boxes[0]		= GPoint(status.level, (status.pos - 1));
	status.boxes[1]		= GPoint(status.level,  status.pos);
	status.boxes[2]		= GPoint(status.level, (status.pos + 1));
	
	int i;
	status.sizes[0] = status.size;
	for (i=1;i<ROWS;i++) {
		status.sizes[i] = 0;
	}
	
	int r;
	for (r=0;r<ROWS;r++) {
		status.storage[r] = NULL;
	}
	
	if (status.difficulty == HARD) {
		status.speeds[0] = 10;
		status.speeds[1] = 20;
		status.speeds[2] = 30;
		status.speeds[3] = 40;
		status.speeds[4] = 50;
		status.speeds[5] = 60;
		
	} else if (status.difficulty == NORMAL) {
		status.speeds[0] = 30;
		status.speeds[1] = 40;
		status.speeds[2] = 50;
		status.speeds[3] = 60;
		status.speeds[4] = 70;
		status.speeds[5] = 80;
		
	} else if (status.difficulty == EASY) {
		status.speeds[0] = 50;
		status.speeds[1] = 60;
		status.speeds[2] = 70;
		status.speeds[3] = 80;
		status.speeds[4] = 90;
		status.speeds[5] = 100;
	}
	
	// Initializates the Timer
	timer_callback(NULL);
}

/**
 * Handles the GUI Initialization
 */
void window_load (Window *window) {
	GRect bounds = layer_get_frame(ui.windowLayer);
	
	ui.boardLayer = layer_create(bounds);
	layer_set_update_proc(ui.boardLayer, boxDraw);
	layer_add_child(ui.windowLayer, ui.boardLayer);
	
	//GRect debugBounds = GRect(0, 0, bounds.size.w, 60);
	// ui.debugLayer = text_layer_create(debugBounds);
	ui.debugLayer = text_layer_create(bounds);
	text_layer_set_text_color(ui.debugLayer, GColorBlack);
	text_layer_set_background_color(ui.debugLayer, GColorClear);
	text_layer_set_text(ui.debugLayer, "Points: 0");
  	text_layer_set_font(ui.debugLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.debugLayer, GTextAlignmentCenter);
	layer_add_child(ui.boardLayer, text_layer_get_layer(ui.debugLayer));
	
	GRect messageBounds = GRect(0, 69, bounds.size.w, 30);
	ui.messageLayer = text_layer_create(messageBounds);
	text_layer_set_text_color(ui.messageLayer, GColorWhite);
	text_layer_set_background_color(ui.messageLayer, GColorBlack);
	text_layer_set_text(ui.messageLayer, "");
  	text_layer_set_font(ui.messageLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(ui.messageLayer, GTextAlignmentCenter);
	layer_add_child(ui.boardLayer, text_layer_get_layer(ui.messageLayer));
	layer_set_hidden(text_layer_get_layer(ui.messageLayer), true);
}

/**
 * Handles the Middle Button Presses
 */
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	// If the Game didn't finish yet, handle the Logic, otherwise restart the game
	if (!status.end) nextLevel();
	else {
		status.end = true;
		free(status.timer);
		status.timer = NULL;
		
		layer_set_hidden(text_layer_get_layer(ui.messageLayer), true);
		window_stack_pop(true);
		scores_name_init(status.points);
		status.cleanScores	= true;
	}
}

/**
 * Handles the Back Button Presses
 */
static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
	status.end = true;
	free(status.timer);
	status.timer = NULL;
	
	window_stack_pop(true);
}

/**
 * Adds listeners for the Buttons
 */
static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_BACK , back_click_handler);
}

/**
 * Handles the Initialization of the Game
 */
void game_init(int difficulty) {
	// Creates the Window and saves the root layer for later use
	ui.window = window_create();
	ui.windowLayer = window_get_root_layer(ui.window);
	
	// sets the variable first to true (as it is it's first run)
	status.first		= true;
	status.difficulty	= difficulty;
	status.end			= false;
	
	// Adds the Button Listeners
	window_set_click_config_provider(ui.window, click_config_provider);
	
	// Adds the Window Listeners
	window_set_window_handlers(ui.window, (WindowHandlers) {
    	.load = window_load,
    	.appear = window_appear
	});
	
	// Loads the Window with an animation
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

/**
 * Deallocates Memory when Destroyed
 */
void game_deinit(void) {
	status.end = true;
	
	status.timer = NULL;
	if (status.timer != NULL) app_timer_cancel(status.timer);
	free(status.timer);
	
	// Ressets Variables
	window_appear(ui.window);
	do_some_cleanup();
	
	// Destroys Layers
	layer_destroy(ui.boardLayer);
	free(ui.boardLayer);
	
	text_layer_destroy(ui.debugLayer);
	free(ui.debugLayer);
	
	text_layer_destroy(ui.messageLayer);
	free(ui.messageLayer);
	
	layer_destroy(ui.windowLayer);
	free(ui.windowLayer);
	
	window_destroy(ui.window);
}
