//
// Created by moff4 on 28.12.2020.
//

#ifndef GRAPHLAB_GEO_H
#define GRAPHLAB_GEO_H

#include <vector>

class Vector {
public:
    double dx, dy, dz;

    Vector(double _dx, double _dy, double _dz) {
        this->dx = _dx;
        this->dy = _dy;
        this->dz = _dz;
    }

    void out() const {
        std::cout << "<Vec dx=" << this->dx << " dy=" << this->dy << " dz=" << this->dz << ">" << std::endl;
    }

    void normilize() {
        double l = this->length();
        this->dx /= l;
        this->dy /= l;
        this->dz /= l;
    }

    inline double length() const {
        return sqrt(this->dx * this->dx + this->dy * this->dy + this->dz * this->dz);
    }

    Vector *mul(Vector *other) const {
        return new Vector(
                (this->dy * other->dz) - (this->dz * other->dy),
                -(this->dx * other->dz) + (this->dz * other->dx),
                (this->dx * other->dy) - (this->dy * other->dx)
        );
    }

    double scalar(Vector *other) const {
        return (this->dx * other->dx + this->dy * other->dy + this->dz * other->dz);
    }

    double angle(Vector *other) const {
        return acos(this->scalar(other) / (this->length() * other->length()));
    }
};

class Point {

public:
    double x, y, z;

    Point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    virtual void out() const {
        /**
         * Prints data to stdout
         */
        std::cout << "<Point x=" << this->x << ", y=" << this->y << ", z=" << this->z << ">" << std::endl;
    }

    inline Vector *direction(const Point &other) const {
        return new Vector(
                other.x - this->x,
                other.y - this->y,
                other.z - this->z
        );
    }

    inline int32_t pixel_color(Vector *ray, Point *cam, double max_allow_angle) const {
        auto vec_to_point = cam->direction(*this);
        auto angle = fabs(vec_to_point->angle(ray));
        if (angle <= max_allow_angle * POINT_WIDTH) {
            return 0;  // set black
        }
        delete vec_to_point;
        return -1; // say not to draw
    }
};

class Edge {
public:

    Point *src;
    Point *dst;

    Edge(Point *src, Point *dst) {
        this->src = src;
        this->dst = dst;
    }

    void out() const {
        /**
         * Prints data to stdout
         */
        std::cout << "<Edge src=" << this->src << ", dst=" << this->dst << ">" << std::endl;
    }

    inline double length() const {
        auto dx = this->src->x - this->dst->x;
        auto dy = this->src->y - this->dst->y;
        auto dz = this->src->z - this->dst->z;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }

private:
    inline Point *vec_point(Vector *vec, int i, int num) const {
        double step = (double) i / num;
        return new Point(
                this->src->x + vec->dx * step,
                this->src->y + vec->dy * step,
                this->src->z + vec->dz * step
        );
    }

public:

    inline uint32_t pixel_color(Vector *ray, Point *cam, double max_allow_angle) const {
        uint32_t color = -1;
        int num = round(this->length() * DOT_COUNT);
        auto vec = this->src->direction(*this->dst);
        for (int i = 1; i < num; i++) {
            auto point = this->vec_point(vec, i, num);
            color = point->pixel_color(ray, cam, max_allow_angle);
            delete point;
            if (color != -1) {
                return color;
            }
        }
        return color;
    }

};


#endif //GRAPHLAB_GEO_H
