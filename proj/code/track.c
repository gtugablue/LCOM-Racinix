#include "track.h"
#include "string.h"

#define PI 					3.14159265358979323846

#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define MIN(a, b)	((a) < (b) ? (a) : (b))

static vector2D_t createCatmullRomSpline(vector2D_t P0, vector2D_t P1, vector2D_t P2, vector2D_t P3, double t);
//static double calculateCatmullCoordinate(double P0, double P1, double P2, double P3, double t);
//static double calculateCatmullDerivativeCoordinate(double P0, double P1, double P2, double P3, double t);
static void pushApart(vector2D_t hull[], unsigned hull_size);
//static vector2D_t calculateCatmullNormal(vector2D_t P0, vector2D_t P1, vector2D_t P2, vector2D_t P3, double t);
static unsigned long track_generate_random(unsigned long *seed);
static int convexHull(vector2D_t points[], unsigned n, vector2D_t hull[]);
static bool isLeft( vector2D_t P0, vector2D_t P1, vector2D_t P2);
static bool intersect(vector2D_t Pi, vector2D_t Pf, vector2D_t Qi, vector2D_t Qf);
static void swapPoints(vector2D_t *a, int i, int j);
static int partition(vector2D_t *a, int left, int right, int pivot);
static void quickSort(vector2D_t *a, int left, int right);
static unsigned modulo(int a, int b);
static int track_generate_perturb(track_t *track, unsigned long *seed);
static void track_generate_fix_angles(track_t *track);
static bool track_generate_check_intersection(track_t *track);
static void track_free_try_again(track_t *track);
static void track_initialize_arguments(track_t *track);

track_t *track_create(unsigned width, unsigned height)
{
	track_t *track;
	if ((track = (track_t *)malloc(sizeof(track_t))) == NULL)
	{
		return NULL;
	}
	track->width = width;
	track->height = height;
	track_initialize_arguments(track);
	return track;
}

int track_random_generate(track_t *track, unsigned long seed)
{
	int result;
	size_t i;
	for (i = 0; i < TRACK_GENERATION_NUM_TRIES; ++i)
	{
		if (track_generate_control_points(track, &seed))
		{
			track_free_try_again(track);
			continue; // Try again
		}
		track_generate_set_start_line(track);
#ifndef TRACK_GENERATION_ALLOW_INTERSECTIONS
		if (track_generate_check_intersection(track)) // Track intersects itself
		{
			continue; // Try again
		}
#endif
		result = track_generate_spline(track);
		if (result == -1)
		{
			track_delete(track);
			break;
		}
		else if (result == 1)
		{
			track_free_try_again(track);
			continue; // Try again;
		}
		if (track_update_track_points(track))
		{
			track_free_try_again(track);
			continue; // Try again
		}
		return 0;
	}
	track_delete(track);
	return 1;
}

int track_generate_control_points(track_t *track, unsigned long *seed)
{
	unsigned pointCount = track_generate_random(seed) % (TRACK_GENERATION_MAX_POINTS - TRACK_GENERATION_MIN_POINTS) + TRACK_GENERATION_MIN_POINTS;

	vector2D_t random_points[pointCount];
	size_t j;
	for(j = 0; j < pointCount; ++j)
	{
		random_points[j].x = track_generate_random(seed) % (int)(TRACK_GENERATION_SIZE_FACTOR * track->width) + (1 - TRACK_GENERATION_SIZE_FACTOR) * track->width / 2;
		random_points[j].y = track_generate_random(seed) % (int)(TRACK_GENERATION_SIZE_FACTOR * track->height) + (1 - TRACK_GENERATION_SIZE_FACTOR) * track->height / 2;
	}
	if ((track->control_points = malloc(pointCount * sizeof(vector2D_t))) == NULL)
	{
		return 1;
	}
	track->num_control_points = convexHull(random_points, pointCount, track->control_points) - 1;
	if ((track->control_points = realloc(track->control_points, track->num_control_points * sizeof(vector2D_t))) == NULL)
	{
		return 1;
	}
	for (j = 0; j < 10; ++j)
	{
		pushApart(track->control_points, track->num_control_points);
	}
	track_generate_perturb(track, seed);
	for (j = 0; j < 30; ++j)
	{
		pushApart(track->control_points, track->num_control_points);
		track_generate_fix_angles(track);
	}
	return 0;
}

void track_generate_set_start_line(track_t *track)
{
	size_t i;

	// Find largest angle between control points
	unsigned best = 0;
	double best_angle = vectorAngle(vectorSubtract(track->control_points[track->num_control_points - 1], track->control_points[0]), vectorSubtract(track->control_points[1], track->control_points[0]));
	for (i = 0; i < track->num_control_points; ++i)
	{
		double current_angle = vectorAngle(vectorSubtract(track->control_points[i - 1], track->control_points[i]), vectorSubtract(track->control_points[(i + 1) % track->num_control_points], track->control_points[i]));
		if (current_angle > best_angle)
		{
			best_angle = current_angle;
			best = i;
		}
	}

	// Rotate array
	vector2D_t aux[track->num_control_points];
	for (i = 0; i < track->num_control_points; ++i)
	{
		aux[i] = track->control_points[i];
	}
	for (i = 0; i < track->num_control_points; ++i)
	{
		track->control_points[i] = aux[(i + best) % track->num_control_points];
	}
}

void track_draw(track_t *track)
{
	size_t x, y;
	for (x = 0; x < track->width; ++x)
	{
		for (y = 0; y < track->height; ++y)
		{
			if (*(track->track_points + x + y * track->width))
			{
				vg_set_pixel(x, y, RACINIX_COLOR_TRACK);
			}
		}
	}
	size_t i;
	track_draw_control_points(track);
	track_draw_finish_line(track);

	/*size_t i;
	for (i = 0; i < track->num_control_points; ++i)
	{
		vector2D_t polygon[4];
		polygon[0] = track->outside_spline[((int)(1.0 / TRACK_INTERP_PERIOD) * i) % track->spline_size];
		polygon[1] = track->outside_spline[((int)(1.0 / TRACK_INTERP_PERIOD) * (i + 1)) % track->spline_size];
		polygon[2] = track->inside_spline[((int)(1.0 / TRACK_INTERP_PERIOD) * (i + 1)) % track->spline_size];
		polygon[3] = track->inside_spline[((int)(1.0 / TRACK_INTERP_PERIOD) * i) % track->spline_size];
		vg_draw_polygon(polygon, 4, rgb(255, 255, 255));
	}*/
}

void track_draw_spline(track_t *track)
{
	if (track->num_control_points >= 3)
	{
		size_t i;
		for (i = 0; i < track->spline_size; ++i)
		{
			// DRAW CENTRAL SPLINE
			vg_draw_line(track->spline[i].x, track->spline[i].y, track->spline[(i + 1) % track->spline_size].x, track->spline[(i + 1) % track->spline_size].y, RACINIX_COLOR_TRACK);

			// DRAW INSIDE SPLINE
			vg_draw_line(track->inside_spline[i].x, track->inside_spline[i].y, track->inside_spline[(i + 1) % track->spline_size].x, track->inside_spline[(i + 1) % track->spline_size].y, RACINIX_COLOR_TRACK);

			// DRAW OUTSIDE SPLINE
			vg_draw_line(track->outside_spline[i].x, track->outside_spline[i].y, track->outside_spline[(i + 1) % track->spline_size].x, track->outside_spline[(i + 1) % track->spline_size].y, RACINIX_COLOR_TRACK);
		}
	}
}

void track_draw_control_points(track_t *track)
{
	size_t i;
	for (i = 0; i < track->num_control_points; ++i)
	{
		vg_draw_circle(track->control_points[i].x, track->control_points[i].y, 2, 0xFFFF);
	}
}

void track_draw_finish_line(track_t *track)
{
	vg_draw_line(track->inside_spline[0].x, track->inside_spline[0].y, track->outside_spline[0].x, track->outside_spline[0].y, rgb(255, 255, 255));
}

static vector2D_t createCatmullRomSpline(vector2D_t p0, vector2D_t p1, vector2D_t p2, vector2D_t p3, double t)
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

/*static double calculateCatmullCoordinate(double P0, double P1, double P2, double P3, double t)
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
}*/

/*static double calculateCatmullDerivativeCoordinate(double P0, double P1, double P2, double P3, double t)
{
	//return t * (P0 * (2.0 - 4.5 * t) + P1 * (9.0 * t - 3.0) - 7.5 * P2 * t + 4.0 * P2 + 3.0 * P3 * t - P3);
	return 0.5 * (3 * (P3 - 3.0 * P2 + 3.0 * P1 - P0) * t * t + 2 * (-P3 + 4 * P2 + P1 + P0 - 3.0) * t + P3 - P0);
}*/

static void pushApart(vector2D_t hull[], unsigned hull_size)
{
    double dst = TRACK_GENERATION_MIN_POINT_DISTANCE;
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

/*static vector2D_t calculateCatmullNormal(vector2D_t P0, vector2D_t P1, vector2D_t P2, vector2D_t P3, double t)
{
	vector2D_t normal;
	normal.y = -calculateCatmullDerivativeCoordinate(P0.x, P1.x, P2.x, P3.x, t);
	normal.x = calculateCatmullDerivativeCoordinate(P0.y, P1.y, P2.y, P3.y, t);
	return normal;
}*/

static unsigned long track_generate_random(unsigned long *seed)
{
	return *seed = ((*seed * 1103515245 + 12345)/65536) % 32768;
}

double track_get_point_drag(track_t *track, int x, int y, unsigned width, unsigned height)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
	{
		if (track->track_points[x + y * width])
		{
			return 0;
		}
	}
	return TRACK_GRASS_DRAG;
}

void track_delete(track_t *track)
{
	free(track->track_points);
	free(track->spline);
	free(track->inside_spline);
	free(track->outside_spline);
	free(track->control_points);
	free(track);
}

static int convexHull(vector2D_t points[], unsigned n, vector2D_t hull[])
{
	int k = 0;

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

static bool isLeft( vector2D_t p1, vector2D_t p2, vector2D_t p3 )
{
	return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x) <= 0;
}

static bool intersect(vector2D_t Pi, vector2D_t Pf, vector2D_t Qi, vector2D_t Qf)
{
	return isLeft(Pi, Pf, Qi) != isLeft(Pi, Pf, Qf) && isLeft(Pi, Qi, Qf) != isLeft(Pf, Qi, Qf);
}

static void swapPoints(vector2D_t *a, int i, int j)
{
	vector2D_t t;
	t.x = a[i].x;
	t.y = a[i].y;
	a[i].x = a[j].x;
	a[i].y = a[j].y;
	a[j].x = t.x;
	a[j].y = t.y;
}

static int partition(vector2D_t *a, int left, int right, int pivot)
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

static void quickSort(vector2D_t *a, int left, int right)
{
	if (left < right)
	{
		int pivot = (left + right) / 2;
		int pos = partition(a,left,right,pivot);
		quickSort(a, left, pos - 1);
		quickSort(a, pos + 1, right);
	}
}

static unsigned modulo(int a, int b)
{
	int remainder = a % b;
	if (remainder < 0)
	{
		return remainder + b;
	}
	else
	{
		return remainder;
	}
}

static int track_generate_perturb(track_t *track, unsigned long *seed)
{
	vector2D_t *perturbed_control_points;
	if ((perturbed_control_points = malloc(track->num_control_points * 2 * sizeof(vector2D_t))) == NULL)
	{
		return 1;
	}
	vector2D_t perturbation;
	size_t i;
	for(i = 0; i < track->num_control_points; ++i)
	{
		perturbation = vectorCreate(1, 0);
		perturbation = vectorRotate(perturbation, track_generate_random(seed) % (int)(2 * PI));
		//perturbation = vectorSubtract(track->control_points[(i + 1) % track->num_control_points], track->control_points[i]);
		//perturbation = vectorRotate(perturbation, PI / 2);
		perturbation = vectorMultiply(perturbation, pow((track_generate_random(seed) % 10000) / (double)10000, TRACK_CONTROL_POINT_PERTURBATION_EXP) * TRACK_CONTROL_POINT_PERTURBATION_MAX);
		perturbed_control_points[2 * i] = track->control_points[i];
		perturbed_control_points[2 * i + 1] = vectorAdd(vectorDivide(vectorAdd(track->control_points[i], track->control_points[(i + 1) % track->num_control_points]), 2), perturbation);
	}
	free(track->control_points);
	track->control_points = perturbed_control_points;
	track->num_control_points = track->num_control_points * 2;

	for(i = 0; i < 10; ++i)
	{
		pushApart(track->control_points, track->num_control_points);
	}
	return 0;
}

int track_generate_spline(track_t *track)
{
	track->spline_size = 0;
	if ((track->spline = realloc(track->spline, track->num_control_points * ceil(1.0 / TRACK_INTERP_PERIOD) * sizeof(vector2D_t))) == NULL)
	{
		return -1;
	}
	int i;
	double t;
	for(i = 0; i < track->num_control_points; ++i)
	{
		for(t = 0.0f; t <= 1.0f; t += TRACK_INTERP_PERIOD)
		{
			track->spline[track->spline_size] = createCatmullRomSpline(track->control_points[modulo(i - 1, track->num_control_points)], track->control_points[i % track->num_control_points], track->control_points[(i + 1) % track->num_control_points], track->control_points[(i + 2) % track->num_control_points], t);
			++track->spline_size;
		}
	}
	double temp;
	vector2D_t normal;
	if ((track->inside_spline = realloc(track->inside_spline, track->spline_size * sizeof(vector2D_t))) == NULL)
	{
		return -1;
	}
	if ((track->outside_spline = realloc(track->outside_spline, track->spline_size * sizeof(vector2D_t))) == NULL)
	{
		return -1;
	}
	for (i = 0; i < track->spline_size; ++i)
	{
		// CALCULATE NORMAL
		normal.x = track->spline[(i + 1) % track->spline_size].x - track->spline[i].x;
		normal.y = track->spline[(i + 1) % track->spline_size].y - track->spline[i].y;

		// NORMALIZE NORMAL
		normal = vectorDivide(normal, vectorNorm(normal));

		// INSCREASE NORMAL
		normal = vectorMultiply(normal, TRACK_THICKNESS	/ 2);

		// CALCULATE PERPENDICULAR TO THE SPLINE
		temp = normal.x;
		normal.x = -normal.y;
		normal.y = temp;

		// CALCULATE INSIDE SPLINE
		track->inside_spline[i] = vectorAdd(track->spline[i], normal);
		if (track->inside_spline[i].x < 0 || track->inside_spline[i].y < 0 || track->inside_spline[i].x > track->width || track->inside_spline[i].y > track->height)
		{
			return 1;
		}

		// CALCULATE OUTSIDE SPLINE
		track->outside_spline[i] = vectorSubtract(track->spline[i], normal);
		if (track->outside_spline[i].x < 0 || track->outside_spline[i].y < 0 || track->outside_spline[i].x > track->width || track->outside_spline[i].y > track->height)
		{
			return 1;
		}
	}
	return 0;
}

int track_update_track_points(track_t *track)
{
	if ((track->track_points = realloc(track->track_points, track->width * track->height * sizeof(bool))) == NULL)
	{
		return 1;
	}

	/* This loop was too slow, so it was replaced by a much faster one...
				 size_t j;
				 size_t counter = 0;
				 vector2D_t point;
				 for (i = 0; i < width; ++i)
				 {
					 point.x = i;
					 for (j = 0; j < height; ++j)
					 {
						 point.y = j;
						 counter++;
						 if (isPointInPolygon(outside_spline, spline_size, &point) && !isPointInPolygon(spline, spline_size, &point))
						 {
							vg_set_pixel(x + i, y + j, 0x00);
						 }
					 }
				 }
				 printf("Method 1 iterations: %d\n", counter);*/

	/* This loop was much faster, however we came up with a better idea...
			size_t x, y;
			vector2D_t polygon[4];
			vector2D_t point;
			bool found;
			for (i = 0; i < track->spline_size; ++i)
			{
				polygon[0] = track->inside_spline[i];
				polygon[1] = track->outside_spline[i];
				polygon[2] = track->outside_spline[(i + 1) % track->spline_size];
				polygon[3] = track->inside_spline[(i + 1) % track->spline_size];

				for (x = MAX(track->spline[i].x - 71, 0); x < MIN(track->spline[i].x + 71, width); ++x)
				{
					point.x = x;
					found = false;
					for (y = MAX(track->spline[i].y - 71, 0); y < MIN(track->spline[i].y + 71, height); y++)
					{
						point.y = y;
						if (!*(track->track_points + y * width + x))
						{
							if (isPointInPolygon(polygon, sizeof(polygon) / sizeof(vector2D_t), &point))
							{
	 *(track->track_points + y * width + x) = true;
							}
						}
					}
				}
			}*/

	/* This loop is really fast. It loops through all spline points and creates a 4-side polygon. Then it checks all points that are in the range (min.x, min.y) to (max.x, max.y). */
	memset(track->track_points, false, track->width * track->height); // Initialize track_points
	size_t x, y, i, j;
	vector2D_t polygon[4];
	vector2D_t point;
	vector2D_t min, max;
	bool *current_point; // For efficiency purposes
	for (i = 0; i < track->spline_size; ++i)
	{
		*(track->track_points + (unsigned)track->spline[i].y * track->width + (unsigned)track->spline[i].x) = true;
		polygon[0] = track->inside_spline[i];
		polygon[1] = track->outside_spline[i];
		polygon[2] = track->outside_spline[(i + 1) % track->spline_size];
		polygon[3] = track->inside_spline[(i + 1) % track->spline_size];
		min = max = polygon[0];
		for (j = 0; j < 4; ++j)
		{
			if (polygon[j].x < min.x)
			{
				min.x = polygon[j].x;
			}
			else if (polygon[j].x > max.x)
			{
				max.x = polygon[j].x;
			}
			if (polygon[j].y < min.y)
			{
				min.y = polygon[j].y;
			}
			else if (polygon[j].y > max.y)
			{
				max.y = polygon[j].y;
			}
		}
		for (x = (int)min.x; x <= (int)max.x; ++x)
		{
			for (y = (int)min.y; y <= (int)max.y; ++y)
			{
				if ((unsigned)x >= track->width || (unsigned)y >= track->height)
				{
					return 1;
				}
				point = vectorCreate((unsigned)x, (unsigned)y);
				current_point = track->track_points + (unsigned)y * track->width + (unsigned)x;
				if (!*current_point && isPointInPolygon(polygon, 4, point))
				{
					*current_point = true;
				}
			}
		}
	}
	return 0;
}

static void track_generate_fix_angles(track_t *track)
{
	vector2D_t vector1, vector2;
	size_t i;
	for(i = 0; i < track->num_control_points; ++i)
	{
		// TODO
		/*vector1 = vectorSubtract(track->control_points[modulo(i - 1, track->num_control_points)], track->control_points[i]);
		vector2 = vectorSubtract(track->control_points[(i + 1) % track->num_control_points], track->control_points[i]);

		// To calculate the angle, we need to use the atan instead of simply calculating it, because we need to know wether the rotation was clockwise or counter-clockwise
		double angle = atan2(vectorPerpendicularDotProduct(vector1, vector2), vectorScalarProduct(vector1, vector2));
		printf("angle: %d\n", (int)angle);
		if(abs(angle) <= TRACK_GENERATION_MAX_ANGLE * PI / 180)
		{
			// Angle ok
			continue;
		}

		float nA = (TRACK_GENERATION_MAX_ANGLE * PI / 180) * ((angle > 0) - (angle < 0));
		float diff = nA - angle;

		track->control_points[(i + 1) % track->num_control_points] = vectorAdd(track->control_points[i], vectorRotate(vector2, diff));
		 *///I got the difference between the current angle and 100degrees, and built a new vector that puts the next point at 100 degrees.


		int previous = modulo(i - 1, track->num_control_points);
		int next = (i + 1) % track->num_control_points;
		float px = track->control_points[i].x - track->control_points[previous].x;
		float py = track->control_points[i].y - track->control_points[previous].y;
		float pl = sqrt(px * px + py * py);
		px /= pl;
		py /= pl;

		float nx = track->control_points[i].x - track->control_points[next].x;
		float ny = track->control_points[i].y - track->control_points[next].y;
		nx = -nx;
		ny = -ny;
		float nl = sqrt(nx * nx + ny * ny);
		nx /= nl;
		ny /= nl;

		float angle = atan2(px * ny - py * nx, px * nx + py * ny);
		if (abs(angle) <= TRACK_GENERATION_MAX_ANGLE * PI / 180)
		{
			continue;
		}

		float nA = (TRACK_GENERATION_MAX_ANGLE * PI / 180) * ((angle > 0) - (angle < 0));
		float diff = nA - angle;
		float new_cos = cos(diff);
		float new_sin = sin(diff);
		float newX = nx * new_cos - ny * new_sin;
		float newY = nx * new_sin + ny * new_cos;
		newX *= nl;
		newY *= nl;
		track->control_points[next].x = track->control_points[i].x + newX;
		track->control_points[next].y = track->control_points[i].y + newY;
	}
}

static bool track_generate_check_intersection(track_t *track)
{
	size_t i, j;
	for (i = 0; i < track->num_control_points; ++i)
	{
		for (j = 0; j < track->num_control_points; ++j)
		{
			if (MIN(modulo(i - j, track->num_control_points), modulo(j - i, track->num_control_points)) > 2 && intersect(track->control_points[i], track->control_points[(i + 1) % track->num_control_points], track->control_points[j], track->control_points[(j + 1) % track->num_control_points]))
			{
				return true;
			}
		}
	}
	return false;
}

unsigned track_get_closest_control_point(track_t *track, vector2D_t point)
{
	size_t i;
	double min_distance = vectorDistance(point, track->control_points[0]);
	double curr_distance;
	unsigned closest_point = 0;
	for (i = 1; i < track->num_control_points; ++i)
	{
		curr_distance = vectorDistance(point, track->control_points[i]);
		if (curr_distance < min_distance)
		{
			min_distance = curr_distance;
			closest_point = i;
		}
	}
	return closest_point;
}

unsigned track_get_closest_spline_point(track_t *track, vector2D_t point)
{
	size_t i;
	double min_distance = vectorDistance(point, track->spline[0]);
	double curr_distance;
	unsigned closest_point = 0;
	for (i = 1; i < track->spline_size; ++i)
	{
		curr_distance = vectorDistance(point, track->spline[i]);
		if (curr_distance < min_distance)
		{
			min_distance = curr_distance;
			closest_point = i;
		}
	}
	return closest_point;
}

int track_add_control_point(track_t *track, unsigned after_point_ID)
{
	if ((track->control_points = realloc(track->control_points, ++track->num_control_points * sizeof(vector2D_t))) == NULL)
	{
		return 1;
	}
	size_t i;
	for (i = track->num_control_points - 1; i > after_point_ID + 1; --i)
	{
		track->control_points[i] = track->control_points[i - 1];
	}
	return 0;
}

int track_erase_control_point(track_t *track, unsigned point_ID)
{
	size_t i;
	--track->num_control_points;
	for (i = point_ID; i < track->num_control_points; ++i)
	{
		track->control_points[i] = track->control_points[i + 1];
	}
	if ((track->control_points = realloc(track->control_points, track->num_control_points * sizeof(vector2D_t))) == NULL) // Will probably not fail, but since there's nothing stating that in the C standard, better safe than sorry
	{
		return 1;
	}
	return 0;
}

unsigned track_spline_to_control_point(track_t *track, unsigned spline_point_ID)
{
	return spline_point_ID / (ceil(1.0 / TRACK_INTERP_PERIOD));
}

static void track_free_try_again(track_t *track)
{
	free(track->track_points);
	free(track->spline);
	free(track->inside_spline);
	free(track->outside_spline);
	free(track->control_points);
	track_initialize_arguments(track);
}

static void track_initialize_arguments(track_t *track)
{
	track->track_points = NULL;
	track->control_points = NULL;
	track->num_control_points = 0;
	track->spline_size = 0;
	track->spline = NULL;
	track->inside_spline = NULL;
	track->outside_spline = NULL;
}
