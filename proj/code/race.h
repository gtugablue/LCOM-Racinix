#ifndef _RACE_H
#define _RACE_H

#include "vehicle.h"
#include "track.h"
#include "video_gr.h"
#include "font.h"
#include <stdbool.h>

#define RACE_START_COUNTER_HEIGHT				120
#define RACE_START_TEXT							"GO!"
#define RACE_WAITING_TEXT						"WAITING..."
#define RACE_START_TEXT_FADE_TIME				0.6
#define RACE_SPEEDOMETER_MARGIN					30

#define RACE_SERIAL_PROTO_BASE					10
#define RACE_SERIAL_PROTO_FLOAT_MULTIPLIER		1000
#define RACE_SERIAL_PROTO_TOKEN					" "
#define RACE_SERIAL_PROTO_RACE					"RACE"
#define RACE_SERIAL_PROTO_VEHICLE_INFO			"VI" // VI <x_pos> <y_pos> <speed> <heading> <current_lap> <current_checkpoint>
#define RACE_SERIAL_PROTO_READY					"READY"

//#define RACE_SHOW_FPS							// Uncomment to show FPS

/** @defgroup race Race
 * @{
 *
 * Module that create and control everything that is related to races
 *
 */

/** @name Race structure */
/** @{
 *
 * Race structure
 */
typedef struct
{
	int state;
	track_t *track;
	unsigned num_players;
	unsigned char first; // Player in the lead

	// Serial port
	bool serial_port;
	unsigned port_number;
	long seed; // -1 if track was manually generated
	bool host; // true if the player created the match, false otherwise

	bitmap_t **vehicle_bitmaps;
	bitmap_t *bitmap_speedometer;
	vehicle_keys_t *vehicle_keys;
	uint16_t *vehicle_colors;
	vehicle_t **vehicles;
	double time; // A negative value means the race is in freeze time and about to start.
	unsigned num_laps;
	vbe_mode_info_t *vbe_mode_info;
	font_t *font;
} race_t;
/** @} end of race_t */

/** @name Race states */
/** @{ */
enum
{
	RACE_STATE_WAITING,
	RACE_STATE_FREEZETIME,
	RACE_STATE_RACING,
	RACE_STATE_END
};
/** @} end of Race states */

/**
 * @brief Race constructor
 *
 * @param track the track that the race will have
 * @param num_players the number of players that will play the game (only one or two players)
 * @param serial_port
 * @param vehicle_bitmaps the bitmaps of the vehicle (one or two vehicles upon user selection)
 * @param bitmap_speedometer the speedometer bitmap that will be loaded
 * @param vehicle_keys the keys that the user will tap to control the vehicle
 * @param vehicle_colors the color of the vehicle
 * @param freeze_time
 * @param num_laps number if laps that the race will have (after which the race will end)
 * @param vbe_mode_info
 * @param font
 *
 * @return A pointer to the race on success, NULL otherwise
 */
race_t *race_create(track_t *track, unsigned num_players, bool serial_port, bitmap_t **vehicle_bitmaps, bitmap_t* bitmap_speedometer, vehicle_keys_t *vehicle_keys, uint16_t *vehicle_colors, double freeze_time, unsigned num_laps, vbe_mode_info_t *vbe_mode_info, font_t *font);

/**
 * @brief se corrida for porta serie por modo porta serie --> passar a classe race estes parametros pa dzer qual a porta a usar
 *
 * @param race race info
 * @param port_number serial port number
 * @param seed seed of the track
 * @param host say if the user is the host or the guest
 */
void race_set_serial_port_info(race_t *race, unsigned port_number, long seed, bool host);

/**
 * @brief Starts the race
 *
 * @param race race info
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int race_start(race_t *race);

/**
 * @brief Updates the status of the race
 *
 * @param race race info
 * @param delta_time the time during which it will check for updates, that it will implement in the specific frame
 * @param fps frames per second
 *
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int race_tick(race_t *race, double delta_time, unsigned fps);

/**
 * @brief Receives the data from the serial port
 *
 * Receives and work with the data from the serial port
 *
 * @param race race info
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int race_serial_receive(race_t *race);

/**
 * @brief esta funçao host cria jogo, e funçao manda ready para o guest para começar o jogo
 *
 * @param race race info
 *
 * @return Return 0 upon success, non-zero otherwise
 */
int race_serial_transmit_ready_state(race_t *race);

/**
 * @brief Deletes the race (specified)
 *
 * @param race race info
 */
void race_delete(race_t *race);

/** @} end of race */
#endif
