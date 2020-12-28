#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>

#define POINT_WIDTH 2
#define DOT_COUNT 100

#include "geo.h"
#include "bmp.h"
#include "tools.h"

#define PI 3.1415926535
#define to_degres(x) (x * 180 / PI)

BMPColorHeader masks = BMPColorHeader();


class Camera : public Point {

private:
    int _width = 0, _height = 0;  // screen size

public:
    double width_max_angle = 0.0;
    double height_max_angle = 0.0;

    Vector *dir = nullptr;  // direction

    Camera(double x1, double y1, double z1, double _width_max_angle, double _height_max_angle) : Point(x1, y1, z1) {
        this->width_max_angle = _width_max_angle;
        this->height_max_angle = _height_max_angle;
    }

    ~Camera() {
        delete this->dir;
    }

    void out() const override {
        /**
         * Prints data to stdout
         */
        std::cout << "<Camera x=" << this->x << ", y=" << this->y << ", z=" << this->z << ">" << std::endl;
        std::cout << "<CameraDir x=" << this->dir->dx << ", y=" << this->dir->dy << ", z=" << this->dir->dz << ">"
                  << std::endl;
    }

    inline void set_direction(Vector *direction) {
        this->dir = direction;
    }

    inline void set_size(int width, int height) {
        this->_width = width;
        this->_height = height;
    }

    inline int width() const {
        return this->_width;
    }

    inline int height() const {
        return this->_height;
    }
};

class World {

private:
    Camera *camera = nullptr;
    std::vector<Point *> points;
    std::vector<Edge *> edges;

    inline void add_edge(Edge *edge) {
        this->edges.push_back(edge);
    }

    inline void add_point(Point *point) {
        this->points.push_back(point);
    }

    inline Point *get_point(int idx) {
        return this->points.at(idx);
    }

public:

    ~World() {
        for (auto &point : this->points) {
            delete point;
        }
        for (auto &edge : this->edges) {
            delete edge;
        }
        delete this->camera;
    }

    void set_camera(Camera *cam) {
        this->camera = cam;
    }

    void read_dat_file(const std::string &filename) {
        /**
         * read data from file
         */
        std::fstream fs;
        fs.open(filename, std::fstream::in);

        if (!fs.is_open()) {
            std::cout << "Error open file" << std::endl;
            return;
        }

        std::string line;

        int points_count = 0;
        if (fs >> line) {
            points_count = std::stoi(line);
        }

        for (int i = 0; i < points_count; i++) {
            std::string x, y, z;
            fs >> x >> y >> z;
            this->add_point(
                    new Point(
                            std::stod(x),
                            std::stod(y),
                            std::stod(z)
                    )
            );
        }

        int edges_count = 0;
        if (fs >> line) {
            std::istringstream iss(line);
            if (!(iss >> edges_count)) {
                fs.close();
                return;
            }
        }

        for (int i = 0; i < edges_count; i++) {
            std::string x, y;
            fs >> x >> y;
            this->add_edge(
                    new Edge(
                            this->get_point(std::stoi(x) - 1),
                            this->get_point(std::stoi(y) - 1)
                    )
            );
        }

        fs.close();
    }

    void out() {
        /**
         * Prints data to stdout
         */
        for (auto &point : this->points) {
            point->out();
        }
        for (auto &edge : this->edges) {
            edge->out();
        }
    }

    Point center() {
        double x = 0, y = 0, z = 0;
        for (auto &point : this->points) {
            x += point->x;
            y += point->y;
            z += point->z;
        }
        int count = this->points.size();
        return {x / count, y / count, z / count};
    }

private:
    inline void set_pixel(BMP* bmp, int i, int j, uint32_t color) {
        bmp->set_pixel(
                i, j,
                color & masks.blue_mask,
                color & masks.green_mask,
                color & masks.red_mask,
                1.0
        );
    }

public:
    BMP *render() {
        /**
         * Render picture
         */
        auto bmp = new BMP(this->camera->width(), this->camera->height(), false);

        double max_allow_angle = (this->camera->width_max_angle / this->camera->width());

        for (int i = 0; i < this->camera->width(); i++) {
            std::cout << "Progress: " << 100.0 * i / this->camera->width() << std::endl;
            for (int j = 0; j < this->camera->height(); j++) {

                bmp->set_pixel(i, j, 255, 255, 255, 1.0);

                double alpha = i_to_phi(i, this->camera->width() - 1, this->camera->width_max_angle);
                double beta = i_to_phi(j, this->camera->height() - 1, this->camera->height_max_angle);

                // Here is some error with CAMERA->DIR->DZ
                // so set camera location as (x, y, 0)
                auto ray = new Vector(
                        (this->camera->dir->dx * cos(alpha) + this->camera->dir->dy * sin(alpha)) * cos(beta),
                        (this->camera->dir->dy * cos(alpha) - this->camera->dir->dx * sin(alpha)) * cos(beta),
                        this->camera->dir->length() * sin(beta)
                );

                bool pixel_setted = false;

                // Draw point
                for (auto &point: this->points) {
                    int32_t color = point->pixel_color(ray, this->camera, max_allow_angle);
                    if (color != -1) {
                        this->set_pixel(bmp, i, j, color);
                        pixel_setted = true;
                        break;
                    }
                }

                // Draw edge
                if (!pixel_setted) {
                    for (auto &edge: this->edges) {
                        int32_t color = edge->pixel_color(ray, this->camera, max_allow_angle);
                        if (color != -1) {
                            this->set_pixel(bmp, i, j, color);
                            pixel_setted = true;
                            break;
                        }
                    }
                }
            }
        }
        return bmp;
    }
};

int main(int argc, const char **argv) {

    // Input / Output files
    std::string input, output;
    if (argc > 1) { input = std::string(argv[1]); } else { input = "input.dat"; }
    if (argc > 2) { output = std::string(argv[2]); } else { output = "output.bmp"; }

    // Camera position
    double cam_x, cam_y, cam_z;
    if (argc > 3) { cam_x = std::stod(std::string(argv[3])); } else { cam_x = 5; }
    if (argc > 4) { cam_y = std::stod(std::string(argv[4])); } else { cam_y = 0; }
    if (argc > 5) { cam_z = std::stod(std::string(argv[5])); } else { cam_z = 0; }

    auto world = new World();
    world->read_dat_file(input);

    auto camera = new Camera(cam_x, cam_y, cam_z, PI / 6, PI / 6);
    camera->set_size(250, 250);
    camera->set_direction(camera->direction(world->center()));

    world->set_camera(camera);

    auto bmp = world->render();
    bmp->write(output);

    delete world;
    delete bmp;
    return 0;
}
