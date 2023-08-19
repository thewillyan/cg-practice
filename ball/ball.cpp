#include <GL/freeglut_std.h>
#include <cmath>

// window constants
const int WIDTH = 500;
const int HEIGHT = 500;
GLubyte *PixelBuffer = new GLubyte[WIDTH * HEIGHT * 3];

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

  Vec(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  int mul(const Vec &other) {
    int x = this->x * other.x;
    int y = this->y * other.y;
    int z = this->z * other.z;
    return x + y + z;
  }

  Vec div(int k) {
    int x = this->x / k;
    int y = this->y / k;
    int z = this->z / k;
    return Vec(x, y, z);
  }

  int size() { return std::sqrt(this->mul(*this)); }

  Vec norm() { return (this->div(this->size())); }
};

class Point {
public:
  int x;
  int y;
  int z;

  Point(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  Point() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
  }

  Vec sub(const Point &other) {
    int x = this->x - other.x;
    int y = this->y - other.y;
    int z = this->z - other.z;
    return Vec(x, y, z);
  }
};

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
  float r = static_cast<float>(BG_R) / 255.0;
  float g = static_cast<float>(BG_G) / 255.0;
  float b = static_cast<float>(BG_B) / 255.0;
  glClearColor(r, g, b, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  int delta_x = HEIGHT / 500;
  int delta_y = HEIGHT / 500;
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

void handle_input(unsigned char key, int mouse_x, int mouse_y) {
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
