#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
    QVector3D eye, up, center;
    float dist_r, zoom;
    static QMatrix4x4 rotate_mat(float degree, const QVector3D& axis) {
        QMatrix4x4 mat;
        mat.rotate(degree, axis);
        return mat;
    }
public:
    Camera();
    Camera(const QVector3D& eye, const QVector3D& up, const QVector3D& center, float dist = 1.0);
public:
    QVector3D get_eye() const {
        return eye;
    }
    QVector3D get_center() const {
        return center;
    }
    QVector3D get_up() const {
        return up;
    }
public:
    QMatrix4x4 camera_mat() const;
    void translate_up(float dist);
    void translate_left(float dist);
    void translate_forward(float dist);
    void rotate_up(float degree);
    void rotate_left(float degree);
    void zoom_near(float dist);
};

#endif // CAMERA_H
