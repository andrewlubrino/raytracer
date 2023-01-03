///g++ window.cpp -o window -L usr/lib/x86_64-linux-gnu/perl5/5.32/auto/Tk/Xlib -l Xlib.so

#include <X11/Xlib.h>
#include <stdio.h>
#include <cmath>
#include <random>

class Vec3 {

	public:
		Vec3(){}

		Vec3 (float x, float y, float z): x(x), y(y), z(z){}

		Vec3 add(Vec3 const & in){
			return Vec3(x + in.x,
						y + in.y,
						z + in.z);
		}

		Vec3 add(double in){
			return Vec3(x + in,
						y + in,
						z + in);
		}

		Vec3 operator + (Vec3 const & in){ 

			return (*this).add(in);

		}

		Vec3 operator + (double in){ 

			return (*this).add(in);

		}

		void operator += (Vec3 const & in) {

			x += in.x;
			y += in.y;
			z += in.z;

		}

		Vec3 minus(Vec3 const & in) const{
			return Vec3(x - in.x,
						y - in.y,
						z - in.z);
		}

		Vec3 operator - (Vec3 const & in) const {

			return (*this).minus(in);

		}

		Vec3 mult(float in){

			return Vec3(x * in, y * in, z * in);
		}

		Vec3 operator * (float in) {

			return (*this).mult(in);

		}

		Vec3 div(float in){

			return (*this) * (1 / in);
		}

		Vec3 operator / (float in){

			return (*this).div(in);

		}

		void operator /= (float in){

			x /= in;
			y /= in;
			z /= in;

		}

		static float dot(Vec3 const & in1, Vec3 const & in2){

			return in1.x * in2.x + in1.y * in2.y + in1.z * in2.z;

		}

		float length(){

			if(len == nullptr){

				len = new float;

				*len = std::sqrt(Vec3::dot(*this, *this));

				return *len;

			} else {
				
				return *len;

			}
		}

		Vec3 normalize(){

			return (*this) / (*this).length();

		}

		Vec3 clamp(double min, double max){

			float clamp_x = x;
			float clamp_y = y;
			float clamp_z = z;

			if (x < min) clamp_x = min;
			if (x > max) clamp_x = max;

			if (y < min) clamp_y = min;
			if (y > max) clamp_y = max;

			if (z < min) clamp_z = min;
			if (z > max) clamp_z = max;

			return Vec3(clamp_x, clamp_y, clamp_z);

		}

		void print(){
			printf("%.1f %.1f %.1f\n", x, y, z);
		}

		float x { 0.0 };
		float y { 0.0 };
		float z { 0.0 };
		float * len {nullptr};
};

class Ray {

	public:

		Vec3 origin {Vec3()};

		Vec3 direction {Vec3()};

		Ray(){}

		Ray(Vec3 const & origin, Vec3 const & direction): origin(origin), direction(direction) {}

		Vec3 getPoint(double t){

			return (*this).origin + ((*this).direction * t);

		}

};

class Sphere {

	public:

		Vec3 center {Vec3()};

		Vec3 color {Vec3(255, 255, 255)};

		float radius {1.0};

		Sphere(){}

		Sphere(Vec3 const & center, float radius, Vec3 const & color): center(center), radius(radius), color(color) {}

		double wasHit(Ray const & ray) {

			Vec3 aMinC = ray.origin - (this -> center);
			
			double a = Vec3::dot(ray.direction, ray.direction);

			double b = 2 * Vec3::dot(ray.direction, aMinC);

			double c = Vec3::dot(aMinC, aMinC) - std::pow(this -> radius, 2);

			double discriminant = std::pow(b, 2) - 4 * a * c;

			if (discriminant < 0) {

				return -1;

			} else {

				return (-b - std::sqrt(discriminant)) / (2 * a);
			}
		}
};

inline double randomDouble() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

void raytracer(unsigned char *data, int width, int height){

	float viewportHeight = 2.0;

	float viewportWidth = 2.0;

	float focal_length = 1.0;

	Vec3 origin(0,0,0); 
	Vec3 horizontal(viewportWidth,0,0); 
	Vec3 vertical(0,viewportHeight,0);
	Vec3 depth(0,0,focal_length);

	Vec3 lowerLeft = origin - horizontal / 2 - vertical / 2 - depth;

	int maxColor = 255;

	Sphere test_rt(Vec3(0,0,-1), 0.5, Vec3(255,153,255));

	int samplesPerPixel = 100;

	for(int i = 0; i < height; i++){

		for(int j = 0; j < width; j++) {

			Vec3 resultVec = Vec3();

			for(int s = 0; s < samplesPerPixel; s++){

				double u = (double(j) + randomDouble()) / double(width - 1);

				double v = (double(i) + randomDouble()) / double(height - 1);

				Ray ray(origin, lowerLeft + (horizontal * u) + (vertical * v) - origin);

				double t = test_rt.wasHit(ray);

				if (t == -1) {

					resultVec += Vec3(255.0, 255.0, 255.0);

				} else {

					Vec3 intersection = ray.getPoint(t);

					Vec3 normal = (intersection - test_rt.center).normalize();

					Vec3 color = ((normal + 1.0) / 2) * maxColor;

					resultVec += color;

				}
			}

			resultVec /= samplesPerPixel;

			Vec3 output = resultVec.clamp(0, 255);

      int offset = width * i * 4 + 4 * j;

      data[offset] = output.z;

      data[offset + 1] = output.y;

      data[offset + 2] = output.x;

      data[offset  + 3] = 0;			
		}
	}
}

int main(void) {
  Display *d;
  Window w;
  XEvent e;
  int s;

  d = XOpenDisplay(NULL);
  if (d == NULL) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 500, 500, 1,
                          BlackPixel(d, s), WhitePixel(d, s));
  
  GC gc = XCreateGC(d, w, 0, NULL);

  XSelectInput(d, w, ExposureMask | KeyPressMask);
  XMapWindow(d, w);

  int width = 500;
  int height = 500;

  unsigned char data[width * height * 4];

  raytracer(data, width, height);

  XImage *image  = XCreateImage(d, DefaultVisual(d, 0), 24, ZPixmap, 0, (char*) data, height, width, 32, 0); 

  Pixmap pm = XCreatePixmap(d, w, height, width, 24);

  XPutImage(d, pm, gc, image, 0,0,0,0, height, width);

  while (1) {
    XNextEvent(d, &e);
    if (e.type == Expose) {
        XCopyArea(d, pm, w, gc, 0, 0, width, height, 10, 10);
    }
    if (e.type == KeyPress)
        break;
  }

  XCloseDisplay(d);
  return 0;
}