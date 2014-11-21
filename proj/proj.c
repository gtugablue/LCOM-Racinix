#include "proj.h"

#define WAIT_TIME_S 	1
#define MOUSE_NUM_TRIES		10
#define MOUSE_HOOK_BIT	12

#define BIT(n) (0x01<<(n))

static vector2D_t mouse_position;
static vbe_mode_info_t vmi;
static int state;

int main(int argc, char **argv) {

	/* Initialize service */

	sef_startup();

	racinix_start();

	racinix_dispatcher();

	racinix_exit();

	return 0;
}

int racinix_start()
{
	srand(time(NULL));

	vg_init(0x105);
	vbe_get_mode_info(0x105, &vmi);

	mouse_position = vectorCreate(vmi.XResolution / 2, vmi.YResolution / 2);
}

int racinix_exit()
{
	vg_exit();
}

int racinix_dispatcher()
{
	track_t *track = track_generate(vmi.XResolution, vmi.YResolution, rand());

	vehicle_t *vehicle1 = vehicle_create(20, 40, &track->spline[0], atan2(track->spline[1].y - track->spline[0].y, track->spline[1].x - track->spline[0].x));
	vehicle_t *vehicle2 = vehicle_create(20, 40, &track->spline[5], atan2(track->spline[6].y - track->spline[5].y, track->spline[6].x - track->spline[5].x));

	unsigned mouse_hook_id = MOUSE_HOOK_BIT;
	if (mouse_subscribe_int(&mouse_hook_id) == -1)
	{
		return 1;
	}
	if (mouse_set_stream_mode(MOUSE_NUM_TRIES))
	{
		return 1;
	}
	if (mouse_enable_stream_mode(MOUSE_NUM_TRIES))
	{
		return 1;
	}
	mouse_discard_interrupts(MOUSE_NUM_TRIES, MOUSE_HOOK_BIT);

	if (keyboard_subscribe_int() == -1)
	{
		return 1;
	}

	unsigned char timer_hook_bit;
	if ((timer_hook_bit = timer_subscribe_int()) < 0)
	{
		return 1;
	}


	int r, ipc_status;
	message msg;
	unsigned counter = 0;
	while(!kbd_keys[KEY_ESC].pressed)
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			// Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) /* hardware interrupt notification */
			{
				if (msg.NOTIFY_ARG & BIT(KEYBOARD_HOOK_BIT)) {
					if (racinix_keyboard_int_handler())
					{
						return 1;
					}
				}
				if (msg.NOTIFY_ARG & BIT(timer_hook_bit)) {
					if (racinix_timer_int_handler(&vmi, track, vehicle1, vehicle2))
					{
						return 1;
					}
				}
				if (msg.NOTIFY_ARG & BIT(MOUSE_HOOK_BIT)) {
					racinix_mouse_int_handler(vmi.XResolution, vmi.YResolution);
				}
			}
		}
	}

	timer_unsubscribe_int();
	keyboard_unsubscribe_int();
	mouse_disable_stream_mode(MOUSE_NUM_TRIES);
	mouse_unsubscribe_int(mouse_hook_id);
}

int racinix_event_handler(int event, ...)
{
	static int state = RACINIX_STATE_MAIN_MENU;
	va_list var_args;
	va_start(var_args, event);
	switch(state)
	{
	case RACINIX_STATE_MAIN_MENU:
	{
		racinix_main_menu_event_handler(var_args);
	}
	case RACINIX_STATE_PICK_TRACK:
	case RACINIX_STATE_DESIGN_TRACK:
	case RACINIX_STATE_RACE_FREEZE_TIME:
	case RACINIX_STATE_RACE:
	case RACINIX_STATE_RACE_END:
	case RACINIX_STATE_END:
	break;
	}
	va_end(var_args);
}

int racinix_main_menu_event_handler(va_list var_args)
{
	if (va_arg(var_args, int) == RACINIX_EVENT_MOUSE_LEFT_BTN) // event
	{
		// TODO
	}
	else
	{
		if (!va_arg(var_args, int)) // pressed
		{
			char *buttons[RACINIX_MAIN_MENU_NUM_BTN];
			buttons[0] = "Pick track", 0;
			buttons[1] = "Design track", 0;
			buttons[2] = "Multiplayer", 0;
			buttons[3] = "Credits", 0;
			buttons[4] = "Exit", 0;

			size_t i;
			for (i = 0; i < RACINIX_MAIN_MENU_NUM_BTN; ++i)
			{
				// TODO Write text
				vg_draw_rectangle(
						(vmi.XResolution - RACINIX_MAIN_MENU_CHAR_WIDTH * sizeof(buttons[i])) / 2,
						i * vmi.YResolution / RACINIX_MAIN_MENU_NUM_BTN,
						RACINIX_MAIN_MENU_CHAR_WIDTH * sizeof(buttons[i]),
						RACINIX_MAIN_MENU_BTN_HEIGHT,
						0x11
				);
			}
		}
	}
	return 0;
}

int racinix_keyboard_int_handler()
{
	return keyboard_int_handler();
}

int racinix_timer_int_handler(vbe_mode_info_t *vmi, track_t *track, vehicle_t *vehicle1, vehicle_t *vehicle2)
{
	racinix_event_handler(RACINIX_EVENT_TIMER);
	static vehicle_keys_t vehicle_keys;
	static unsigned counter = 0;
	if (counter >= TIMER_DEFAULT_FREQ / FPS)
	{
		vg_swap_mouse_buffer();
		vg_fill(0x2);
		size_t l;
		track_draw(track, vmi->XResolution, vmi->YResolution);
		// Vehicle 1
		double drag = 0.5;
		size_t i;
		for(i = 0; i < VEHICLE_NUM_WHEELS; ++i)
		{
			drag += track_get_point_drag(track, (int)vehicle1->wheels[i].x, (int)vehicle1->wheels[i].y, vmi->XResolution, vmi->YResolution);
		}
		vehicle_keys.accelerate = kbd_keys[KEY_W].pressed;
		vehicle_keys.brake = kbd_keys[KEY_S].pressed;
		vehicle_keys.turn_left = kbd_keys[KEY_A].pressed;
		vehicle_keys.turn_right = kbd_keys[KEY_D].pressed;
		vehicle_tick(vehicle1, vmi, (double)counter / TIMER_DEFAULT_FREQ, drag, vehicle_keys);
		for (i = 1; i < 5; ++i)
		{
			vg_draw_line(vmi->XResolution / 2, vmi->YResolution - i, vmi->XResolution / 2 + vehicle1->speed, vmi->YResolution - i, 0x0);
		}
		// Vehicle 2
		drag = 0.5;
		for(i = 0; i < VEHICLE_NUM_WHEELS; ++i)
		{
			drag += track_get_point_drag(track, (int)vehicle2->wheels[i].x, (int)vehicle2->wheels[i].y, vmi->XResolution, vmi->YResolution);
		}
		vehicle_keys.accelerate = kbd_keys[KEY_ARR_UP].pressed;
		vehicle_keys.brake = kbd_keys[KEY_ARR_DOWN].pressed;
		vehicle_keys.turn_left = kbd_keys[KEY_ARR_LEFT].pressed;
		vehicle_keys.turn_right = kbd_keys[KEY_ARR_RIGHT].pressed;
		vehicle_tick(vehicle2, vmi, (double)counter / TIMER_DEFAULT_FREQ, drag, vehicle_keys);
		for (i = 5; i < 10; ++i)
		{
			vg_draw_line(vmi->XResolution / 2, vmi->YResolution - i, vmi->XResolution / 2 + vehicle2->speed, vmi->YResolution - i, 0x0);
		}
		counter = 0;

		if (vehicle_check_vehicle_collision(vehicle1, vehicle2))
		{
			vehicle_vehicle_collision_handler(vehicle1, vehicle2);
		}
		if (vehicle_check_vehicle_collision(vehicle2, vehicle1))
		{
			vehicle_vehicle_collision_handler(vehicle2, vehicle1);
		}
		draw_mouse(vmi->XResolution, vmi->YResolution);
		vg_swap_buffer();
	}
	++counter;
	return 0;
}

int racinix_mouse_int_handler(unsigned width, unsigned height)
{
	if(mouse_int_handler(MOUSE_NUM_TRIES))
	{
		return 1;
	}
	mouse_data_packet_t mouse_data_packet;
	if (mouse_get_packet(&mouse_data_packet))
	{
		mouse_position = vectorAdd(mouse_position, vectorCreate(mouse_data_packet.x_delta, -mouse_data_packet.y_delta));
		if (mouse_position.x < 0)
		{
			mouse_position.x = 0;
		}
		else if (mouse_position.x >= width)
		{
			mouse_position.x = width - 1;
		}
		if (mouse_position.y < 0)
		{
			mouse_position.y = 0;
		}
		else if (mouse_position.y >= height)
		{
			mouse_position.y = height - 1;
		}

		draw_mouse(width, height);

		return 0;
	}
	else
	{
		return 1;
	}
}

void draw_mouse(unsigned width, unsigned height)
{
	vg_swap_buffer();
	int xpm_width, xpm_height;
	char *xpm = read_xpm(pixmap_get(5), &xpm_width, &xpm_height, width, height);
	vg_draw_mouse((int)mouse_position.x, (int)mouse_position.y, xpm, (unsigned short)xpm_width, (unsigned short)xpm_height);
	free(xpm);
	vg_swap_mouse_buffer();
}

// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
int convexHull(vector2D_t points[], unsigned n, vector2D_t hull[])
{
	int k = 0;

	// Sort points lexicographically
	quickSort(points, 0, n - 1);
	int i;
	// Build lower hull
	for (i = 0; i < n; ++i) {
		while (k >= 2 && isLeft(hull[k-2], hull[k-1], points[i])) k--;
		hull[k++] = points[i];
	}

	// Build upper hull
	int t;
	for (i = n-2, t = k+1; i >= 0; i--) {
		while (k >= t && isLeft(hull[k-2], hull[k-1], points[i])) k--;
		hull[k++] = points[i];
	}
	return k;
}

bool isLeft( vector2D_t p1, vector2D_t p2, vector2D_t p3 )
{
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x) <= 0;
}

void swapPoints(vector2D_t *a, int i, int j)
{
    vector2D_t t;
    t.x = a[i].x;
    t.y = a[i].y;
    a[i].x = a[j].x;
    a[i].y = a[j].y;
    a[j].x = t.x;
    a[j].y = t.y;
}

int partition(vector2D_t *a, int left, int right, int pivot)
{
    int pos, i;
    swapPoints(a, pivot, right);
    pos = left;
    for(i = left; i < right; i++)
    {
        if (a[i].x < a[right].x || (a[i].x == a[right].x && a[i].y < a[right].y))
        {
            swapPoints(a, i, pos);
            pos++;
        }
    }
    swapPoints(a, right, pos);
    return pos;
}

void quickSort(vector2D_t *a, int left, int right)
{
    if (left < right)
    {
    	int pivot = (left + right) / 2;
        int pos = partition(a,left,right,pivot);
        quickSort(a, left, pos - 1);
        quickSort(a, pos + 1, right);
    }
}
