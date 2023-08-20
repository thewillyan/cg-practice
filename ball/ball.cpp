#include <GL/freeglut_std.h>
#include <cmath>

// window constants
const int WIDTH = 500;
const int HEIGHT = 500;
static GLubyte *PixelBuffer = new GLubyte[WIDTH * HEIGHT * 3];

// circle constants
const int CENTER_X = WIDTH / 2;
const int CENTER_Y = HEIGHT / 2;
const int CENTER_Z = -50;
const int RADIUS = 200;

// canvas constants
const int CANVAS_DIST = -10;
const int BG_R = 10;
const int BG_G = 14;
const int BG_B = 20;
const int FG_R = 194;
const int FG_G = 217;
const int FG_B = 76;

class Vec {
public:
  int x;
  int y;
  int z;

  Vec(int x_axis, int y_axis, int z_axis) {
    x = x_axis;
    y = y_axis;
    z = z_axis;
  }

  int mul(const Vec &other) {
    return (x * other.x) + (y * other.y) + (z * other.z);
  }

  Vec div(int k) { return Vec(x / k, y / k, z / k); }

  int size() {
    double s = std::sqrt(this->mul(*this));
    return static_cast<int>(s);
  }

  Vec norm() { return (this->div(this->size())); }
};

class Point {
public:
  int x;
  int y;
  int z;

  Point(int x_axis, int y_axis, int z_axis) {
    x = x_axis;
    y = y_axis;
    z = z_axis;
  }

  Point() {
    x = 0;
    y = 0;
    z = 0;
  }

  Vec sub(const Point &other) {
    return Vec(x - other.x, y - other.y, z - other.z);
  }
};

// function prototypes
void draw_pixel(Point, Point);
void raycasting(void);
void handle_input(unsigned char, int, int);

void draw_pixel(Point s, Point d) {
  Point c = Point(CENTER_X, CENTER_Y, CENTER_Z);
  Vec dr = d.sub(s).norm();

  Vec w = d.sub(c);
  int w_dr = w.mul(dr);
  int delta = (w_dr * w_dr) - (w.mul(w) - (RADIUS * RADIUS));

  if (d.x >= 0 && d.x < HEIGHT && d.y >= 0 && d.y < WIDTH) {
    int position = (d.y + d.x * WIDTH) * 3;
    if (delta >= 0) {
      PixelBuffer[position] = FG_R;
      PixelBuffer[position + 1] = FG_G;
      PixelBuffer[position + 2] = FG_B;
    } else {
      PixelBuffer[position] = BG_R;
      PixelBuffer[position + 1] = BG_G;
      PixelBuffer[position + 2] = BG_B;
    }
  }
}

void raycasting() {
  float r = static_cast<float>(BG_R) / 255;
  float g = static_cast<float>(BG_G) / 255;
  float b = static_cast<float>(BG_B) / 255;
  glClearColor(r, g, b, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
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
