#include "proj.h"

#define WAIT_TIME_S 1

#define BIT(n) (0x01<<(n))

int main(int argc, char **argv) {

	/* Initialize service */

	sef_startup();

	vbe_mode_info_t vmi;

	racinix_start(&vmi);

	vg_fill(0x02);

	unsigned spline_size;
	vector2D_t spline[(unsigned)ceil(20 * (1.0 / TRACK_INTERP_PERIOD))];
	bool *track = track_generate(vmi.XResolution, vmi.YResolution, rand(), spline, &spline_size);

	vehicle_keys_t vehicle_keys;
	vehicle_t *vehicle1 = vehicle_create(20, 40, &spline[0], atan2(spline[1].y - spline[0].y, spline[1].x - spline[0].x));
	vehicle_t *vehicle2 = vehicle_create(20, 40, &spline[5], atan2(spline[6].y - spline[5].y, spline[6].x - spline[5].x));

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
					if (keyboard_int_handler())
					{
						return 1;
					}
				}
				if (msg.NOTIFY_ARG & BIT(timer_hook_bit)) {
					if (counter >= TIMER_DEFAULT_FREQ / FPS)
					{
						vg_fill(0x2);
						size_t l;
						track_draw(track, vmi.XResolution, vmi.YResolution);
						// Vehicle 1
						double drag = 0.5;
						size_t i;
						for(i = 0; i < VEHICLE_NUM_WHEELS; ++i)
						{
							if (!(*(track + (int)vehicle1->wheels[i].x + (int)vehicle1->wheels[i].y * vmi.XResolution)))
							{
								drag += 0.5;
							}
						}
						vehicle_keys.accelerate = kbd_keys[KEY_W].pressed;
						vehicle_keys.brake = kbd_keys[KEY_S].pressed;
						vehicle_keys.turn_left = kbd_keys[KEY_A].pressed;
						vehicle_keys.turn_right = kbd_keys[KEY_D].pressed;
						vehicle_tick(vehicle1, (double)counter / TIMER_DEFAULT_FREQ, drag, vehicle_keys, vmi.XResolution, vmi.YResolution);
						for (i = 1; i < 5; ++i)
						{
							vg_draw_line(vmi.XResolution / 2, vmi.YResolution - i, vmi.XResolution / 2 + vehicle1->speed, vmi.YResolution - i, 0x0);
						}

						// Vehicle 2
						drag = 0.5;
						for(i = 0; i < VEHICLE_NUM_WHEELS; ++i)
						{
							if (!(*(track + (int)vehicle2->wheels[i].x + (int)vehicle2->wheels[i].y * vmi.XResolution)))
							{
								drag += 0.5;
							}
						}
						vehicle_keys.accelerate = kbd_keys[KEY_ARR_UP].pressed;
						vehicle_keys.brake = kbd_keys[KEY_ARR_DOWN].pressed;
						vehicle_keys.turn_left = kbd_keys[KEY_ARR_LEFT].pressed;
						vehicle_keys.turn_right = kbd_keys[KEY_ARR_RIGHT].pressed;
						vehicle_tick(vehicle2, (double)counter / TIMER_DEFAULT_FREQ, drag, vehicle_keys, vmi.XResolution, vmi.YResolution);
						for (i = 5; i < 10; ++i)
						{
							vg_draw_line(vmi.XResolution / 2, vmi.YResolution - i, vmi.XResolution / 2 + vehicle2->speed, vmi.YResolution - i, 0x0);
						}
						counter = 0;

						if (vehicle_check_vehicle_collision(vehicle1, vehicle2))
						{
							vg_draw_circle(10, 10, 5, 0x0);
							vehicle_vehicle_collision_handler(vehicle1, vehicle2);
						}
						else
						{
							vg_draw_circle(10, 10, 5, 0x11);
						}
						if (vehicle_check_vehicle_collision(vehicle2, vehicle1))
						{
							vg_draw_circle(10, 10, 5, 0x0);
							vehicle_vehicle_collision_handler(vehicle2, vehicle1);
						}
					}
					++counter;
				}
			}
		}
	}

	keyboard_unsubscribe_int();

	racinix_exit();

	return 0;
}

int racinix_start(vbe_mode_info_t *vmi)
{
	srand(time(NULL));

	vg_init(0x105);
	vg_exit();

	vg_init(0x105);
	vbe_get_mode_info(0x105, vmi);
}

int racinix_exit()
{
	vg_exit();
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
