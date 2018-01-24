#ifndef _DELAUNAY_H_
#define _DELAUNAY_H_
#pragma once
#include <utility>
#include <math.h>
#include <vector>
#include <algorithm>

namespace Delaunay {
    template <class T>
    struct Vector2 {
        T x;
        T y;

        Vector2() : x(0), y(0) { }

        Vector2(T x, T y) : x(x), y(y) { }

        Vector2(const Vector2 &other) : x(other.x), y(other.y) { }

        T MagnitudeSqr(const Vector2 &v) {
            T dx = x - v.x;
            T dy = y - v.y;
            return dx * dx + dy * dy;
        }

        float Magnitude(const Vector2 &v) {
            return sqrtf(magnitudeSqr(v));
        }
    };
    template <class T>
    struct Vector2Hasher {
        std::size_t operator()(const Vector2<T>& k) const {
            size_t res = 17;
            res = res * 31 + std::hash<int>()(k.x);
            res = res * 31 + std::hash<int>()(k.y);
            return res;
        }
    };


    template <class T>
    bool operator == (const Vector2<T> &v1, const Vector2<T> &v2) {
        return (v1.x == v2.x) && (v1.y == v2.y);
    }

    template <class T>
    struct Edge {
        Edge(const Vector2<T> &p1, const Vector2<T> &p2) : p1(p1), p2(p2), isBad(false) { };
        Edge(const Vector2<T> &e) : p1(e.p1), p2(e.p2), isBad(false) { };

        Vector2<T> p1;
        Vector2<T> p2;
        bool isBad;
    };

    template <class T>
    bool operator == (const Edge<T> &e1, const Edge<T> &e2) {
        return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) || (e1.p1 == e2.p2 && e1.p2 == e2.p1);
    }

    template <class T>
    struct Triangle {
        Vector2<T> p1;
        Vector2<T> p2;
        Vector2<T> p3;
        Edge<T> e1;
        Edge<T> e2;
        Edge<T> e3;
        bool isBad;

        Triangle(const Vector2<T> &_p1, const Vector2<T> &_p2, const Vector2<T> &_p3)
            : p1(_p1), p2(_p2), p3(_p3), e1(_p1, _p2), e2(_p2, _p3), e3(_p3, _p1), isBad(false) { }

        bool HasVertex(const Vector2<T> &v) {
            return p1 == v || p2 == v || p3 == v;
        }

        bool CircumCircleContains(const Vector2<T> &v) {
            float ab = (p1.x * p1.x) + (p1.y * p1.y);
            float cd = (p2.x * p2.x) + (p2.y * p2.y);
            float ef = (p3.x * p3.x) + (p3.y * p3.y);

            float circum_x = (ab * (p3.y - p2.y) + cd * (p1.y - p3.y) + ef * (p2.y - p1.y)) / (p1.x * (p3.y - p2.y) + p2.x * (p1.y - p3.y) + p3.x * (p2.y - p1.y)) / 2.0f;
            float circum_y = (ab * (p3.x - p2.x) + cd * (p1.x - p3.x) + ef * (p2.x - p1.x)) / (p1.y * (p3.x - p2.x) + p2.y * (p1.x - p3.x) + p3.y * (p2.x - p1.x)) / 2.0f;
            float circum_radius = sqrtf(((p1.x - circum_x) * (p1.x - circum_x)) + ((p1.y - circum_y) * (p1.y - circum_y)));

            float dist = sqrtf(((v.x - circum_x) * (v.x - circum_x)) + ((v.y - circum_y) * (v.y - circum_y)));
            return dist <= circum_radius;
        }
    };

    template <class T>
    bool operator == (const Triangle<T> &t1, const Triangle<T> &t2) {
        return	(t1.p1 == t2.p1 || t1.p1 == t2.p2 || t1.p1 == t2.p3) && (t1.p2 == t2.p1 || t1.p2 == t2.p2 || t1.p2 == t2.p3) && (t1.p3 == t2.p1 || t1.p3 == t2.p2 || t1.p3 == t2.p3);
    }

    template <class T>
    class Delaunay {
    public:

        const std::vector< Triangle<T> >& GetTriangles() const { return _triangles; };

        const std::vector< Edge<T> >& GetEdges() const { return _edges; };

        const std::vector< Vector2<T> >& GetVertices() const { return _vertices; };

        const std::vector< Triangle<T> >& Triangulate(std::vector< Vector2<T> > &vertices) {
            _vertices = vertices;

            // Determinate the super triangle
            float minX = vertices[0].x;
            float minY = vertices[0].y;
            float maxX = minX;
            float maxY = minY;

            for (std::size_t i = 0; i < vertices.size(); ++i) {
                if (vertices[i].x < minX) minX = vertices[i].x;
                if (vertices[i].y < minY) minY = vertices[i].y;
                if (vertices[i].x > maxX) maxX = vertices[i].x;
                if (vertices[i].y > maxY) maxY = vertices[i].y;
            }

            float dx = maxX - minX;
            float dy = maxY - minY;
            float deltaMax = dx > dy ? dx : dy;
            float midx = (minX + maxX) / 2.f;
            float midy = (minY + maxY) / 2.f;

            Vector2<T> p1(midx - 20 * deltaMax, midy - deltaMax);
            Vector2<T> p2(midx, midy + 20 * deltaMax);
            Vector2<T> p3(midx + 20 * deltaMax, midy - deltaMax);

            // Create a list of triangles, and add the supertriangle in it
            _triangles.push_back(Triangle<T>(p1, p2, p3));

            for (auto p = begin(vertices); p != end(vertices); p++) {
                std::vector< Edge<T> > polygon;
                for (auto t = begin(_triangles); t != end(_triangles); t++) {
                    if (t->CircumCircleContains(*p)) {
                        t->isBad = true;
                        polygon.push_back(t->e1);
                        polygon.push_back(t->e2);
                        polygon.push_back(t->e3);
                    }  
                    //else: does not contains  p in circum center
                }

                _triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [](Triangle<T> &t) {
                    return t.isBad;
                }), end(_triangles));

                for (auto e1 = begin(polygon); e1 != end(polygon); e1++) {
                    for (auto e2 = begin(polygon); e2 != end(polygon); e2++) {
                        if (e1 == e2) {
                            continue;
                        }
                        if (*e1 == *e2) {
                            e1->isBad = true;
                            e2->isBad = true;
                        }
                    }
                }
                polygon.erase(std::remove_if(begin(polygon), end(polygon), [](Edge<T> &e) {
                    return e.isBad;
                }), end(polygon));
                for (auto e = begin(polygon); e != end(polygon); e++)
                    _triangles.push_back(Triangle<T>(e->p1, e->p2, *p));
            }

            _triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](Triangle<T> &t) {
                return t.HasVertex(p1) || t.HasVertex(p2) || t.HasVertex(p3);
            }), end(_triangles));

            for (auto t = begin(_triangles); t != end(_triangles); t++) {
                _edges.push_back(t->e1);
                _edges.push_back(t->e2);
                _edges.push_back(t->e3);
            }

            return _triangles;
        }

    private:
        std::vector< Triangle<T> > _triangles;
        std::vector< Edge<T> > _edges;
        std::vector< Vector2<T> > _vertices;
    };
}
#endif