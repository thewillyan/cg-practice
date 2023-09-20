#include <GL/freeglut_std.h>
#include <cmath>

// window constants
const int WIDTH = 500;
const int HEIGHT = 500;
static GLubyte *PixelBuffer = new GLubyte[WIDTH * HEIGHT * 3];

// circle constants
const int CENTER_X = 0;
const int CENTER_Y = 0;
const int CENTER_Z = -50;
const int RADIUS = 200;

// canvas constants
const int CANVAS_DIST = -30;
const int Y_MAX = HEIGHT / 2;
const int Y_MIN = -Y_MAX;
const int X_MAX = WIDTH / 2;
const int X_MIN = -X_MAX;

// colors
const int BG_R = 10;
const int BG_G = 14;
const int BG_B = 20;
const int FG_R = 194;
const int FG_G = 217;
const int FG_B = 76;

// light
const int SOURCE_X = 0;
const int SOURCE_Y = 5;
const int SOURCE_Z = 0;
const double SOURCE_I_R = 0.7;
const double SOURCE_I_G = 0.7;
const double SOURCE_I_B = 0.7;
const double K_R = static_cast<double>(FG_R) / 255.0;
const double K_G = static_cast<double>(FG_G) / 255.0;
const double K_B = static_cast<double>(FG_B) / 255.0;

class Vec {
public:
  double x;
  double y;
  double z;

  Vec(double x_axis, double y_axis, double z_axis) {
    x = x_axis;
    y = y_axis;
    z = z_axis;
  }

  double dot(const Vec &other) {
    return (x * other.x) + (y * other.y) + (z * other.z);
  }

  Vec div(double k) { return Vec(x / k, y / k, z / k); }

  double size() { return std::sqrt(this->dot(*this)); }

  Vec norm() { return (this->div(this->size())); }

  Vec mul(const Vec &other) {
    return Vec((x * other.x), (y * other.y), (z * other.z));
  }

  Vec mul_scalar(double k) { return Vec(x * k, y * k, z * k); }

  Vec add(const Vec &other) {
    return Vec((x + other.x), (y + other.y), (z + other.z));
  }

  Vec sub(const Vec &other) {
    return Vec((x - other.x), (y - other.y), (z - other.z));
  }
};

class Point {
public:
  double x;
  double y;
  double z;

  Point(double x_axis, double y_axis, double z_axis) {
    x = x_axis;
    y = y_axis;
    z = z_axis;
  }

  Point() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  }

  static Point from_vec(const Vec &v) { return Point(v.x, v.y, v.z); }

  Vec sub(const Point &other) {
    return Vec(x - other.x, y - other.y, z - other.z);
  }
};

// Get the centered index of a pixel on the `PixelBuffer` from a given `Point`
// p.
int cpixel_idx(Point);
// Draw a single pixel of a sphere on the `PixelBuffer` from a `Point` of
// origin (where we are looking from) to a `Point` of destination (where) we
// are looking at.
Vec eye_intensity(Point eye_pos, Point inter, Point src_pos, Vec src_int,
                  Vec dr, Vec kdif, Vec kspec, double spec_idx);
void draw_pixel(Point, Point);
void raycasting(void);
void handle_input(unsigned char, int, int);

int cpixel_idx(Point p) {
  return static_cast<int>(((p.y + Y_MAX) + (p.x + X_MAX) * WIDTH) * 3);
}

Vec eye_intensity(Point eye_pos, Point inter, Point src_pos, Vec src_int,
                  Vec dr, Vec kdif, Vec kspec, double spec_idx) {
  Point sphere_center = Point(CENTER_X, CENTER_Y, CENTER_Z);
  Vec l = src_pos.sub(sphere_center).norm();
  Vec r = eye_pos.sub(sphere_center).norm();
  Vec n = inter.sub(sphere_center).norm();

  Vec reflex_src = n.mul_scalar(2 * r.dot(n)).sub(r);
  Vec difuse = src_int.mul(kdif).mul_scalar(l.dot(n));
  Vec specular =
      src_int.mul(kspec).mul_scalar(std::pow(reflex_src.dot(dr), spec_idx));

  return difuse.add(specular);
}

void draw_pixel(Point s, Point d) {
  Point c = Point(CENTER_X, CENTER_Y, CENTER_Z);
  Vec dr = d.sub(s).norm();

  Vec w = d.sub(c);
  double w_dr = w.dot(dr);
  double delta = (w_dr * w_dr) - (w.dot(w) - (RADIUS * RADIUS));

  int position = cpixel_idx(d);
  if (delta >= 0) {
    double ray_len = (-(2 * w_dr) + std::sqrt(delta)) / 2.0;
    Point inter_point = Point::from_vec(dr.mul_scalar(ray_len));
    Point src_pos = Point(SOURCE_X, SOURCE_Y, SOURCE_Z);
    Vec src_int = Vec(SOURCE_I_R, SOURCE_I_G, SOURCE_I_B);
    Vec k = Vec(K_R, K_G, K_B);
    Vec intensity =
        eye_intensity(Point(), inter_point, src_pos, src_int, dr, k, k, 0.1);
    PixelBuffer[position] =
        static_cast<GLubyte>(intensity.x * static_cast<double>(FG_R));
    PixelBuffer[position + 1] =
        static_cast<GLubyte>(intensity.y * static_cast<double>(FG_G));
    PixelBuffer[position + 2] =
        static_cast<GLubyte>(intensity.z * static_cast<double>(FG_B));
  } else {
    PixelBuffer[position] = BG_R;
    PixelBuffer[position + 1] = BG_G;
    PixelBuffer[position + 2] = BG_B;
  }
}

void raycasting() {
  float r = static_cast<float>(BG_R) / 255;
  float g = static_cast<float>(BG_G) / 255;
  float b = static_cast<float>(BG_B) / 255;
  glClearColor(r, g, b, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i = Y_MIN; i < Y_MAX; i++) {
    for (int j = X_MIN; j < X_MAX; j++) {
      Point source = Point();
      Point dest = Point(i, j, CANVAS_DIST);
      draw_pixel(source, dest);
    }
  }

  glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, PixelBuffer);
  glFlush();
}

void handle_input(unsigned char key, int, int) {
  if (key == 'q' || key == 'Q')
    exit(0);
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(0, 0);

  glutCreateWindow("Maball");

  glutKeyboardFunc(handle_input);
  glutDisplayFunc(raycasting);

  glutMainLoop();
  return 0;
}
