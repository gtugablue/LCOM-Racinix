#include "proj.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

#define WAIT_TIME_S 1

#define BIT(n) (0x01<<(n))

int main(int argc, char **argv) {

	/* Initialize service */

	sef_startup();

	vbe_mode_info_t vmi;

	racinix_start(&vmi);

	vg_fill(0x02);
	//generate_track(0, 0, vmi.XResolution, vmi.YResolution);

	vector2D_t position;
	position.x = 200;
	position.y = 100;
	vehicle_t *vehicle = vehicle_create(5, 3, &position, 0);

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
	unsigned long counter;
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
					vg_draw_circle(vehicle->position.x, vehicle->position.y, 7, 0x4);
					vehicle_tick(vehicle, (double)1/60, 0);
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

void generate_track(unsigned x, unsigned y, unsigned width, unsigned height)
{
	 unsigned pointCount = rand() % 11 + 10; //we'll have a total of 10 to 20 points
	 vector2D_t random_points[pointCount];
	 size_t i;

	 for(i = 0; i < pointCount; ++i)
	 {
		 random_points[i].x = rand() % (int)(0.8 * width) + 0.1 * width;
		 random_points[i].y = rand() % (int)(0.8 * height) + 0.1 * height;
	 }

	 vector2D_t hull[pointCount];
	 vector2D_t spline[(unsigned)ceil(pointCount * (1.0 / INTERP_PERIOD))];
	 size_t hull_size = convexHull(random_points, pointCount, hull) - 1;

	 pushApart(hull, hull_size);
	 pushApart(hull, hull_size);
	 pushApart(hull, hull_size);

	 size_t spline_size = 0;
	 double t;
	 for(i = 0; i < hull_size; ++i)
	 {
		 vg_draw_circle(x + hull[i].x, y + hull[i].y, 10, 0x33);
		 for(t = 0.0f; t <= 1.0f; t += INTERP_PERIOD)
		 {
			 spline[spline_size] = createCatmullRomSpline(hull[i], hull[(i + 1) % hull_size], hull[(i + 2) % hull_size], hull[(i + 3) % hull_size], t);
			 ++spline_size;
		 }
	 }
	 double temp;
	 vector2D_t normal, outside_spline[spline_size], inside_spline[spline_size];
	 for (i = 0; i < spline_size; ++i)
	 {
		 // CALCULATE NORMAL
		 normal.x = spline[(i + 1) % spline_size].x - spline[i].x;
		 normal.y = spline[(i + 1) % spline_size].y - spline[i].y;

		 // NORMALIZE NORMAL
		 normal = vectorDivide(normal, vectorNorm(normal));

		 // INSCREASE NORMAL
		 normal = vectorMultiply(normal, 25.0);

		 // CALCULATE PERPENDICULAR TO THE SPLINE
		 temp = normal.x;
		 normal.x = -normal.y;
		 normal.y = temp;

		 // CALCULATE INSIDE SPLINE
		 inside_spline[i] = vectorAdd(spline[i], normal);

		 // CALCULATE OUTSIDE SPLINE
		 outside_spline[i] = vectorSubtract(spline[i], normal);
	 }

	 /*for (i = 0; i < spline_size; ++i)
	 {
		 // DRAW CENTRAL SPLINE
		 vg_draw_line(x + spline[i].x, y + spline[i].y, x + spline[(i + 1) % spline_size].x, y + spline[(i + 1) % spline_size].y, 0xCC);

		 // DRAW INSIDE SPLINE
		 vg_draw_line(x + inside_spline[i].x, y + inside_spline[i].y, x + inside_spline[(i + 1) % spline_size].x, y + inside_spline[(i + 1) % spline_size].y, 0xCC);

		 // DRAW OUTSIDE SPLINE
		 vg_draw_line(x + outside_spline[i].x, y + outside_spline[i].y, x + outside_spline[(i + 1) % spline_size].x, y + outside_spline[(i + 1) % spline_size].y, 0xCC);
	 }*/
	 size_t j;
	 vector2D_t point;
	 for (i = 0; i < width; ++i)
	 {
		 point.x = i;
		 for (j = 0; j < height; ++j)
		 {
			 point.y = j;
			 if (isPointInPolygon(outside_spline, spline_size, &point))
			 {
				vg_set_pixel(x + i, y + j, 0xCC);
			 }
		 }
	 }
	 for (i = 0; i < width; ++i)
	 	 {
	 		 point.x = i;
	 		 for (j = 0; j < height; ++j)
	 		 {
	 			 point.y = j;
	 			 if (isPointInPolygon(inside_spline, spline_size, &point))
	 			 {
	 				vg_set_pixel(x + i, y + j, 0x02);
	 			 }
	 		 }
	 	 }
	 return;
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

vector2D_t createCatmullRomSpline(vector2D_t p0, vector2D_t p1, vector2D_t p2, vector2D_t p3, double t)
{
    double t2 = t * t;
    double t3 = t2 * t;

    vector2D_t result;

    /*double t0 = ((-t + (double)2) * t - (double)1) * t * (double)0.5;
	double t1 = ((((double)3 * t - (double)5) * t) * t + (double)2) * (double)0.5;
	double t2 = (((double)-3 * t + (double)4) * t + (double)1) * t * (double)0.5;
	double t3 = ((t - (double)1) * t * t) * (double)0.5;

	result.x = p0.x * t0 + p1.x * t1 + p2.x * t2 + p3.x * t3;
	result.y = p0.y * t0 + p1.y * t1 + p2.y * t2 + p3.y * t3;
*/
    result.x = 0.5f * ((2.0f * p1.x) +
    	    (-p0.x + p2.x) * t +
    	    (2.0f * p0.x - 5.0f * p1.x + 4 * p2.x - p3.x) * t2 +
    	    (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
    result.y = 0.5f * ((2.0f * p1.y) +
    	    (-p0.y + p2.y) * t +
    	    (2.0f * p0.y - 5.0f * p1.y + 4 * p2.y - p3.y) * t2 +
    	    (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
/*
    result.x = calculateCatmullCoordinate(p0.x, p1.x, p2.x, p3.x, t);
    result.y = calculateCatmullCoordinate(p0.y, p1.y, p2.y, p3.y, t);
*/
    return result;
}

double calculateCatmullCoordinate(double P0, double P1, double P2, double P3, double t)
{
	double t0 = t + 1;
	double t1 = t0 + 1;
	double t2 = t1 + 1;
	double t3 = t2 + 1;

	double L01, L12, L23;
	double L012, L123;
	double C12;

	L01 = 	P0 * ((t1 - t)/(t1 - t0)) 	+ 	P1 * ((t - t0)/(t1 - t0));
	L12 = 	P1 * ((t2 - t)/(t2 - t1)) 	+ 	P2 * ((t - t1)/(t2 - t1));
	L23 = 	P2 * ((t3 - t)/(t3 - t2)) 	+ 	P3 * ((t - t2)/(t3 - t2));
	L012 = 	L01 * ((t2 - t)/(t2 - t0)) 	+ 	L12 * ((t - t0)/(t2 - t0));
	L123 = 	L12 * ((t3 - t)/(t3 - t1)) 	+ 	L23 * ((t - t1)/(t3 - t1));
	C12 = 	L012 * ((t2 - t)/(t2 - t1)) + 	L123 * ((t - t1)/(t2 - t1));

	return C12;
}

double calculateCatmullDerivativeCoordinate(double P0, double P1, double P2, double P3, double t)
{
	//return t * (P0 * (2.0 - 4.5 * t) + P1 * (9.0 * t - 3.0) - 7.5 * P2 * t + 4.0 * P2 + 3.0 * P3 * t - P3);
	return 0.5 * (3 * (P3 - 3.0 * P2 + 3.0 * P1 - P0) * t * t + 2 * (-P3 + 4 * P2 + P1 + P0 - 3.0) * t + P3 - P0);
}

void pushApart(vector2D_t hull[], unsigned hull_size)
{
    double dst = 120;
    double dst2 = dst*dst;
    int i, j;
    for(i = 0; i < hull_size; ++i)
    {
        for(j = i+1; j < hull_size; ++j)
        {
            if(pow(vectorDistance(hull[i], hull[j]), 2) < dst2)
            {
                double hx = hull[j].x - hull[i].x;
                double hy = hull[j].y - hull[i].y;
                double hl = (double)sqrt(hx*hx + hy*hy);
                hx /= hl;
                hy /= hl;
                double dif = dst - hl;
                hx *= dif;
                hy *= dif;
                hull[j].x += hx;
                hull[j].y += hy;
                hull[i].x -= hx;
                hull[i].y -= hy;
            }
        }
    }
}

vector2D_t calculateCatmullNormal(vector2D_t P0, vector2D_t P1, vector2D_t P2, vector2D_t P3, double t)
{
	vector2D_t normal;
	normal.y = -calculateCatmullDerivativeCoordinate(P0.x, P1.x, P2.x, P3.x, t);
	normal.x = calculateCatmullDerivativeCoordinate(P0.y, P1.y, P2.y, P3.y, t);
	return normal;
}
