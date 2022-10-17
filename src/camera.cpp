#include "camera.h"

Camera::Camera()
    : eye(0, 0, -1)
    , up(0, 1, 0)
    , center(0, 0, 0)
{
    zoom = 1.0;
    dist_r = 1;
}

Camera::Camera(const QVector3D& eye, const QVector3D& up, const QVector3D& center, float dist)
    : eye(eye), up(up), center(center)
{
    zoom = 1.0;
    dist_r = dist;
}

QMatrix4x4 Camera::camera_mat() const {
    QVector3D x_eye = (eye - center).normalized() * zoom * dist_r + center;
    QMatrix4x4 mat;
    mat.lookAt(x_eye, center, up);
    return mat;
}

void Camera::translate_up(float dist) {
    QVector3D up_unit = up.normalized();
    auto tmp_eye = eye + dist * up_unit;
    if (! inside(tmp_eye)) return;

    eye += dist * up_unit;
    center += dist * up_unit;
}

void Camera::translate_left(float dist) {
    QVector3D z_axis = (center - eye).normalized();
    QVector3D x_axis = QVector3D::crossProduct(up, z_axis).normalized();
    auto tmp_eye = eye + dist * x_axis;
    if (! inside(tmp_eye)) return;

    eye += dist * x_axis;
    center += dist * x_axis;
}

void Camera::translate_forward(float dist) {
    QVector3D u = (center - eye).normalized();
    auto tmp_eye = eye + dist * u;
    if (! inside(tmp_eye)) return;

    eye += u * dist;
    center += u * dist;
}

void Camera::rotate_up(float degree) {
    auto tmp_eye = eye - center;
    QVector3D x = QVector3D::crossProduct(up, tmp_eye);
    x.normalize();
    tmp_eye = rotate_mat(degree, x).map(tmp_eye);
    auto tmp_up = QVector3D::crossProduct(tmp_eye, x);
    tmp_up.normalize();
    tmp_eye = tmp_eye + center;
    if (! inside(tmp_eye)) return;
    eye = tmp_eye;
    up = tmp_up;
}

void Camera::rotate_left(float degree) {
    auto tmp_eye = eye - center;
    tmp_eye = rotate_mat(degree, up).map(tmp_eye);
    tmp_eye = tmp_eye + center;
    if (! inside(tmp_eye)) return;
    eye = tmp_eye;
}

void Camera::zoom_near(float dist) {
    zoom -= dist;
    constexpr float zoom_max = 2.0;
    constexpr float zoom_min = 0.1;
    zoom = qMax(qMin(zoom, zoom_max), zoom_min);
}
