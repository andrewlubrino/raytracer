class Vec3 {

	public:
		Vec3();

		Vec3 (float x, float y, float z);

		Vec3 add(Vec3 const & in);

		Vec3 add(double in);

		Vec3 operator + (Vec3 const & in);

		Vec3 operator + (double in);

		void operator += (Vec3 const & in);

		Vec3 minus(Vec3 const & in) const;

		Vec3 operator - (Vec3 const & in) const;

		Vec3 mult(float in);

		Vec3 operator * (float in);

		Vec3 div(float in);

		Vec3 operator / (float in);

		void operator /= (float in);

		static float dot(Vec3 const & in1, Vec3 const & in2);

		float length();

		Vec3 normalize();

		Vec3 clamp(double min, double max);

		void print();

		float x { 0.0 };
		float y { 0.0 };
		float z { 0.0 };
		float * len {nullptr};
};

class Scene;

class Ray {

	public:

		Vec3 origin;

		Vec3 direction;

		double tMin, tMax;

		Ray();

		Ray(Vec3 const & origin, Vec3 const & direction);

		Vec3 getPoint(double t);

		void setTBounds(Scene & scene);
};

class Sphere {

	public:

		Vec3 center;

		Vec3 color;

		float radius {1.0};

		Sphere();

		Sphere(Vec3 const & center, float radius, Vec3 const & color);

		std::tuple<double, double, double> wasHit(Ray const & ray);
};

class DisplayWindow {

	public:

		Display *d;
		Window w;
		XEvent e;
		GC gc;
		int s;
		int width {500};
		int height {500};

	DisplayWindow(int height, int width);

	void render(unsigned char * data);

};

class Scene {

	public:

		std::vector<Sphere> items;
		double aspectRatio;
		int height;
		double viewportHeight;
		double focalLength;
		int sampleRate;
		Vec3 camera;
		
		Scene(int height, double viewportHeight, double focalLength, double aspectRatio, int sampleRate);

		void add(Sphere const &  sphere);

		Vec3 getRayColor(Ray & ray);

		void render();

	private:

		Vec3 horizontal, vertical, depth, upperLeft;
		unsigned char * data;

};