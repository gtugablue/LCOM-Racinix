#include "track.h"

#define PI 					3.14159265358979323846

#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define MIN(a, b)	((a) < (b) ? (a) : (b))

static vector2D_t createCatmullRomSpline(vector2D_t P0, vector2D_t P1, vector2D_t P2, vector2D_t P3, double t);
static double calculateCatmullCoordinate(double P0, double P1, double P2, double P3, double t);
static double calculateCatmullDerivativeCoordinate(double P0, double P1, double P2, double P3, double t);
static void pushApart(vector2D_t hull[], unsigned hull_size);
static vector2D_t calculateCatmullNormal(vector2D_t P0, vector2D_t P1, vector2D_t P2, vector2D_t P3, double t);
static unsigned long track_generate_random(unsigned long seed);
static int orientation(vector2D_t p, vector2D_t q, vector2D_t r);
static int convexHull(vector2D_t points[], unsigned n, vector2D_t hull[]);
static bool isLeft( vector2D_t P0, vector2D_t P1, vector2D_t P2);
static void swapPoints(vector2D_t *a, int i, int j);
static int partition(vector2D_t *a, int left, int right, int pivot);
static void quickSort(vector2D_t *a, int left, int right);
static int track_generate_perturb(track_t *track);

track_t *track_generate(unsigned width, unsigned height, unsigned long seed)
{
	track_t *track;
	if ((track = malloc(sizeof(track_t))) == NULL)
	{
		return NULL;
	}

	if ((track->track_points = malloc(width * height * sizeof(bool))) == NULL)
	{
		return NULL;
	}
	printf("Bad address debug #1\n");
	unsigned pointCount = rand() % 11 + 10; //we'll have a total of 10 to 20 points
	printf("Bad address debug #1.1\n");
	vector2D_t random_points[pointCount];
	size_t i;
	printf("Bad address debug #1.2\n");
	for(i = 0; i < pointCount; ++i)
	{
		random_points[i].x = rand() % (int)(0.6 * width) + 0.2 * width;
		random_points[i].y = rand() % (int)(0.6 * height) + 0.2 * height;
	}
	printf("Bad address debug #2\n");
	track->control_points = malloc(pointCount * sizeof(vector2D_t));
	track->num_control_points = convexHull(random_points, pointCount, track->control_points) - 1;
	printf("Bad address debug #3\n");
	for (i = 0; i < 10; ++i)
	{
		pushApart(track->control_points, track->num_control_points);
	}
	printf("Bad address debug #4\n");
	track_generate_perturb(track);
	printf("Bad address debug #5\n");
	track->spline_size = 0;
	double t;
	track->spline = malloc((unsigned)ceil(20 * (1.0 / TRACK_INTERP_PERIOD)) * sizeof(vector2D_t));
	for(i = 0; i < track->num_control_points; ++i)
	{
		//vg_draw_circle(hull[i].x, hull[i].y, 10, 0x33);
		for(t = 0.0f; t <= 1.0f; t += TRACK_INTERP_PERIOD)
		{
			track->spline[track->spline_size] = createCatmullRomSpline(track->control_points[i], track->control_points[(i + 1) % track->num_control_points], track->control_points[(i + 2) % track->num_control_points], track->control_points[(i + 3) % track->num_control_points], t);
			++track->spline_size;
		}
	}
	printf("Bad address debug #6\n");
	double temp;
	vector2D_t normal;
	if ((track->inside_spline = malloc(track->spline_size * sizeof(vector2D_t))) == NULL)
	{
		return NULL;
	}
	printf("Bad address debug #6.1\n");
	if ((track->outside_spline = malloc(track->spline_size * sizeof(vector2D_t))) == NULL)
	{
		printf("RETORNOU NULL\n");
		return NULL;
	}
	printf("Bad address debug #6.2\n");
	for (i = 0; i < track->spline_size; ++i)
	{
		printf("Bad address debug #7, i = %d\n", i);
		// CALCULATE NORMAL
		normal.x = track->spline[(i + 1) % track->spline_size].x - track->spline[i].x;
		printf("Bad address debug #8\n");
		normal.y = track->spline[(i + 1) % track->spline_size].y - track->spline[i].y;
		printf("Bad address debug #9\n");
		// NORMALIZE NORMAL
		normal = vectorDivide(normal, vectorNorm(normal));
		printf("Bad address debug #10\n");
		// INSCREASE NORMAL
		normal = vectorMultiply(normal, TRACK_THICKNESS	/ 2);
		printf("Bad address debug #11\n");
		// CALCULATE PERPENDICULAR TO THE SPLINE
		temp = normal.x;
		normal.x = -normal.y;
		normal.y = temp;
		printf("Bad address debug #12\n");
		// CALCULATE INSIDE SPLINE
		track->inside_spline[i] = vectorAdd(track->spline[i], normal);
		printf("Bad address debug #13\n");
		// CALCULATE OUTSIDE SPLINE
		track->outside_spline[i] = vectorSubtract(track->spline[i], normal);
		printf("Bad address debug #14\n");
	}
	printf("Bad address debug #15\n");

	/*for (i = 0; i < spline_size; ++i)
	{
		// DRAW CENTRAL SPLINE
		vg_draw_line(spline[i].x, spline[i].y, spline[(i + 1) % spline_size].x, spline[(i + 1) % spline_size].y, 0x4);

		// DRAW INSIDE SPLINE
		vg_draw_line(inside_spline[i].x, inside_spline[i].y, inside_spline[(i + 1) % spline_size].x, y + inside_spline[(i + 1) % spline_size].y, 0xCC);

		// DRAW OUTSIDE SPLINE
		vg_draw_line(outside_spline[i].x, outside_spline[i].y, outside_spline[(i + 1) % spline_size].x, outside_spline[(i + 1) % spline_size].y, 0x4);
	}*/

	/*This loop was too slow, so it was replaced by a much faster one.
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
	size_t x, y, j;
	vector2D_t polygon[4];
	vector2D_t point;
	vector2D_t min, max;
	for (i = 0; i < track->spline_size; ++i)
	{
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
				point = vectorCreate(x, y);
				if (isPointInPolygon(polygon, 4, point))
				{
					*(track->track_points + y * width + x) = true;
				}
			}
		}
	}
	printf("Bad address debug #1\n");
	return track;
}

void track_draw(track_t *track, unsigned width, unsigned height)
{
	size_t x, y;
	for (x = 0; x < width; ++x)
	{
		for (y = 0; y < height; ++y)
		{
			if (*(track->track_points + x + y * width))
			{
				vg_set_pixel(x, y, RACINIX_COLOR_TRACK);
			}
		}
	}
	//vg_draw_line(track->inside_spline[0].x, track->inside_spline[0].y, track->outside_spline[0].x, track->outside_spline[0].y, 0x4);
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

static double calculateCatmullCoordinate(double P0, double P1, double P2, double P3, double t)
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

static double calculateCatmullDerivativeCoordinate(double P0, double P1, double P2, double P3, double t)
{
	//return t * (P0 * (2.0 - 4.5 * t) + P1 * (9.0 * t - 3.0) - 7.5 * P2 * t + 4.0 * P2 + 3.0 * P3 * t - P3);
	return 0.5 * (3 * (P3 - 3.0 * P2 + 3.0 * P1 - P0) * t * t + 2 * (-P3 + 4 * P2 + P1 + P0 - 3.0) * t + P3 - P0);
}

static void pushApart(vector2D_t hull[], unsigned hull_size)
{
    double dst = 100;
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

static vector2D_t calculateCatmullNormal(vector2D_t P0, vector2D_t P1, vector2D_t P2, vector2D_t P3, double t)
{
	vector2D_t normal;
	normal.y = -calculateCatmullDerivativeCoordinate(P0.x, P1.x, P2.x, P3.x, t);
	normal.x = calculateCatmullDerivativeCoordinate(P0.y, P1.y, P2.y, P3.y, t);
	return normal;
}

static unsigned long track_generate_random(unsigned long seed)
{
	return ((seed * 1103515245 + 12345)/65536) % 32768;
}

double track_get_point_drag(track_t *track, int x, int y, unsigned width, unsigned height)
{
	if (x >= 0 && x < width & y >= 0 && y < height)
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

// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
static int convexHull(vector2D_t points[], unsigned n, vector2D_t hull[])
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

static bool isLeft( vector2D_t p1, vector2D_t p2, vector2D_t p3 )
{
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x) <= 0;
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

static int track_generate_perturb(track_t *track)
{
	vector2D_t *perturbed_control_points;
	if ((perturbed_control_points = malloc(track->num_control_points * 2 * sizeof(vector2D_t))) == NULL)
	{
		return 1;
	}
	vector2D_t perturbation;
	double max_displacement = 40.0, perturbation_length; // Again, this may change to fit your units.
	size_t i;
	for(i = 0; i < track->num_control_points; ++i)
	{
		perturbation = vectorCreate(1, 0);
		perturbation = vectorRotate(perturbation, rand() % (int)(2 * PI));
		perturbation = vectorMultiply(perturbation, pow((rand() % 10000) / (double)10000, TRACK_CONTROL_POINT_PERTURBATION) * max_displacement);
		perturbed_control_points[2 * i] = track->control_points[i];
		perturbed_control_points[2 * i + 1] = vectorAdd(vectorDivide(vectorAdd(track->control_points[i], track->control_points[(i + 1) % track->num_control_points]), 2), perturbation);
	}
	printf("derp\n");
	free(track->control_points);
	track->control_points = perturbed_control_points;
	track->num_control_points = track->num_control_points * 2;

	for(i = 0; i < 10; ++i)
	{
		pushApart(track->control_points, track->num_control_points);
	}
	return 0;
}
