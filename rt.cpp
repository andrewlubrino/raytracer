///g++ rt.cpp -o rt -L usr/lib/x86_64-linux-gnu/perl5/5.32/auto/Tk/Xlib -l X11

#include <X11/Xlib.h>
#include <stdio.h>
#include <cmath>
#include <random>
#include <vector>
#include <tuple>
#include "rt.h"

class Util {

	public:

		static inline double randomDouble() {
			static std::uniform_real_distribution<double> distribution(0.0, 1.0);
			static std::mt19937 generator;
			return distribution(generator);
		}	

};

Vec3::Vec3(){}

Vec3::Vec3 (float x, float y, float z): x(x), y(y), z(z){}

Vec3 Vec3::add(Vec3 const & in){
	return Vec3(x + in.x,
				y + in.y,
				z + in.z);
}

Vec3 Vec3::add(double in){
	return Vec3(x + in,
				y + in,
				z + in);
}

Vec3 Vec3::operator + (Vec3 const & in){ 

	return (*this).add(in);

}

Vec3 Vec3::operator + (double in){ 

	return (*this).add(in);

}

void Vec3::operator += (Vec3 const & in) {

	x += in.x;
	y += in.y;
	z += in.z;

}

Vec3 Vec3::minus(Vec3 const & in) const{
	return Vec3(x - in.x,
				y - in.y,
				z - in.z);
}

Vec3 Vec3::operator - (Vec3 const & in) const {

	return (*this).minus(in);

}

Vec3 Vec3::mult(float in){

	return Vec3(x * in, y * in, z * in);
}

Vec3 Vec3::operator * (float in) {

	return (*this).mult(in);

}

Vec3 Vec3::div(float in){

	return (*this) * (1 / in);
}

Vec3 Vec3::operator / (float in){

	return (*this).div(in);

}

void Vec3::operator /= (float in){

	x /= in;
	y /= in;
	z /= in;

}

float Vec3::dot(Vec3 const & in1, Vec3 const & in2){

	return in1.x * in2.x + in1.y * in2.y + in1.z * in2.z;

}

float Vec3::length(){

	if(len == nullptr){

		len = new float;

		*len = std::sqrt(Vec3::dot(*this, *this));

		return *len;

	} else {
		
		return *len;

	}
}

Vec3 Vec3::normalize(){

	return (*this) / (*this).length();

}

Vec3 Vec3::clamp(double min, double max){

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

void Vec3::print(){
	printf("%.1f %.1f %.1f\n", x, y, z);
}

Ray::Ray(){}

Ray::Ray(Vec3 const & origin, Vec3 const & direction): origin(origin), direction(direction) {}

Vec3 Ray::getPoint(double t){

	return (*this).origin + ((*this).direction * t);

}

void Ray::setTBounds(Scene & scene) {

	tMin = scene.camera.z;

	tMax = -scene.focalLength / direction.z;

}

Sphere::Sphere(){}

Sphere::Sphere(Vec3 const & center, float radius, Vec3 const & color): center(center), radius(radius), color(color) {}

std::tuple<double, double, double> Sphere::wasHit(Ray const & ray) {

	Vec3 aMinC = ray.origin - (this -> center);
	
	double a = Vec3::dot(ray.direction, ray.direction);

	double b = 2 * Vec3::dot(ray.direction, aMinC);

	double c = Vec3::dot(aMinC, aMinC) - std::pow(this -> radius, 2);

	double discriminant = std::pow(b, 2) - 4 * a * c;

	std::tuple <double, double, double> solutions = std::make_tuple(discriminant, a, b);

	return solutions;
}

		
Scene::Scene(int height, double viewportHeight, double focalLength, double aspectRatio, int sampleRate): 
	height(height), 
	viewportHeight(viewportHeight), 
	focalLength(focalLength),
	aspectRatio(aspectRatio),
	sampleRate(sampleRate) {

		horizontal = Vec3(viewportHeight * aspectRatio,0,0);

		vertical = Vec3(0,viewportHeight,0);

		depth = Vec3(0,0,focalLength);

		upperLeft = camera - horizontal / 2 + vertical / 2 - depth;
}

void Scene::add(Sphere const &  sphere) {

	items.push_back(sphere);

}

Vec3 Scene::getRayColor(Ray & ray){

	double * smallestT = nullptr;

	Vec3 closestColor = Vec3(255, 255, 255);

	for(Sphere item: items){

		ray.setTBounds(*this);

		std::tuple<double, double, double> hitRecord = item.wasHit(ray);

		if (std::get<0>(hitRecord) >= 0) {

			double a = std::get<1>(hitRecord);

			double b = std::get<2>(hitRecord);

			double t = (-b - std::sqrt(std::get<0>(hitRecord))) / (2 * a);

			if (t >= ray.tMin && t <= ray.tMax) {

				if (smallestT == nullptr) {

					smallestT = new double(t);

					closestColor = item.color;

				} else if (t < *smallestT) {

					*smallestT = t;

					closestColor = item.color;

				}
			}
		}
	}

	return closestColor;
}

void Scene::render(){

	int width = int(aspectRatio * height);

	data = new unsigned char[height * width * 4];

	for(int i = 0; i < height; i++){

		for(int j = 0; j < width; j++) {

			Vec3 resultVec = Vec3();

			for(int s = 0; s < sampleRate; s++){

				double u = (double(j) + Util::randomDouble()) / double(width - 1);

				double v = (double(i) + Util::randomDouble()) / double(height - 1);

				Ray ray(camera, upperLeft + (horizontal * u) - (vertical * v) - camera);

				resultVec += getRayColor(ray);
			}

			resultVec /= sampleRate;

			Vec3 output = resultVec.clamp(0, 255);

			int offset = width * i * 4 + 4 * j;

			data[offset] = output.z;

			data[offset + 1] = output.y;

			data[offset + 2] = output.x;

			data[offset  + 3] = 0;			
		}
	}

	DisplayWindow display = DisplayWindow(height, width);

	display.render(data);

}

DisplayWindow::DisplayWindow(int height, int width): height(height), width(width){

	d = XOpenDisplay(NULL);
	if (d == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(1);
	}

	s = DefaultScreen(d);
	w = XCreateSimpleWindow(d, RootWindow(d, s), 0, 0, width, height, 0,
							BlackPixel(d, s), WhitePixel(d, s));
	
	gc = XCreateGC(d, w, 0, NULL);

}

void DisplayWindow::render(unsigned char * data){

	XSelectInput(d, w, ExposureMask | KeyPressMask);
	XMapWindow(d, w);

	XImage *image  = XCreateImage(d, DefaultVisual(d, 0), 24, ZPixmap, 0, (char*) data, width, height, 32, 0); 

	Pixmap pm = XCreatePixmap(d, w, width, height, 24);

	XPutImage(d, pm, gc, image, 0,0,0,0, width, height);

	while (1) {
		XNextEvent(d, &e);
		if (e.type == Expose) {
			XCopyArea(d, pm, w, gc, 0, 0, width, height, 0, 0);
		}
		if (e.type == KeyPress)
			break;
	}

	XCloseDisplay(d);
			
}

int main() {

	Scene scene(500, 2.0, 1.0, 16.0 / 9.0, 1);

	Sphere sphere1(Vec3(0,0,-1), 0.5, Vec3(255,0,0));

	Sphere sphere2(Vec3(0,-100.5,-1), 100, Vec3(153,153,255));

	scene.add(sphere1);

	scene.add(sphere2);

	scene.render();
  
	return 0;

}