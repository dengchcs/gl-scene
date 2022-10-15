#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class camera
{
    QVector3D eye, up, center;
    float dist_r, zoom;
    static QMatrix4x4 rotate_mat(float degree, const QVector3D& axis) {
        QMatrix4x4 mat;
        mat.rotate(degree, axis);
        return mat;
    }
public:
    camera();
    camera(const QVector3D& eye, const QVector3D& up, const QVector3D& center, float dist = 1.0);
public:
    QMatrix4x4 camera_mat();
    void translate_up(float dist);
    void translate_left(float dist);
    void translate_forward(float dist);
    void rotate_up(float degree);
    void rotate_left(float degree);
    void zoom_near(float dist);
};

#endif // CAMERA_H
