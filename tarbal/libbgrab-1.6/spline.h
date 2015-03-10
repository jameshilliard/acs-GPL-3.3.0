
struct xyspline {
		 int snumpoints;
		 float *spline_x;
		 float *spline_y;
		 float *spline_xb;
		 float *spline_xc;
		 float *spline_xd;
		 float *spline_yb;
		 float *spline_yc;
		 float *spline_yd;
		};

struct xyspline create_spline(int, float *, float *);
void spline_value (struct xyspline, float, float *); 
void delete_spline(struct xyspline); 
