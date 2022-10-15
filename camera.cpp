#include "camera.h"

camera::camera()
    : eye(0, 0, -1)
    , up(0, 1, 0)
    , center(0, 0, 0)
{
    zoom = 1.0;
    dist_r = 8.0;
}

camera::camera(const QVector3D& eye, const QVector3D& up, const QVector3D& center, float dist)
    : eye(eye), up(up), center(center)
{
    zoom = 1.0;
    dist_r = dist;
}

QMatrix4x4 camera::camera_mat() {
    QVector3D x_eye = (eye - center).normalized() * zoom * dist_r + center;
    QMatrix4x4 mat;
    mat.lookAt(x_eye, center, up);
    return mat;
}

void camera::translate_up(float dist) {
    QVector3D up_unit = up.normalized();
    eye += dist * up_unit;
    center += dist * up_unit;
}

void camera::translate_left(float dist) {
    QVector3D z_axis = (center - eye).normalized();
    QVector3D x_axis = QVector3D::crossProduct(up, z_axis).normalized();
    center += dist * x_axis;
    eye += dist * x_axis;
}

void camera::translate_forward(float dist) {
    QVector3D u = (center - eye).normalized();
    eye += u * dist;
    center += u * dist;
}

void camera::rotate_up(float degree) {
    eye = eye - center;
    QVector3D x = QVector3D::crossProduct(up, eye);
    x.normalize();
    eye = rotate_mat(degree, x).map(eye);
    up = QVector3D::crossProduct(eye, x);
    up.normalize();
    eye = eye + center;
}

void camera::rotate_left(float degree) {
    eye = eye - center;
    eye = rotate_mat(degree, up).map(eye);
    eye = eye + center;
}

void camera::zoom_near(float dist) {
    zoom -= dist;
    constexpr float zoom_max = 2.0;
    constexpr float zoom_min = 0.1;
    zoom = qMax(qMin(zoom, zoom_max), zoom_min);
}
