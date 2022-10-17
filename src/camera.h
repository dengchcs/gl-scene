#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
    QVector3D eye, up, center;  // 相机位置、上方向和观察点
    float dist_r, zoom;
public:
    Camera();
    Camera(const QVector3D& eye, const QVector3D& up, const QVector3D& center, float dist = 1.0);
public:
    [[nodiscard]] QVector3D get_eye() const { return eye; }
    [[nodiscard]] QVector3D get_center() const { return center; }
    [[nodiscard]] QVector3D get_up() const { return up; }
private:
    /**
     * @brief 用于判断相机是否脱离了天空盒内部
     * @param location 相机位置
     */
    [[nodiscard]] static bool inside(const QVector3D& location) {
        return std::abs(location.x()) <= 1 && std::abs(location.y()) <= 1 && std::abs(location.z()) <= 1;
    }
    /**
     * @brief 构造旋转矩阵, 用于旋转相机
     */
    [[nodiscard]] static QMatrix4x4 rotate_mat(float degree, const QVector3D& axis) {
        QMatrix4x4 mat;
        mat.rotate(degree, axis);
        return mat;
    }
public:
    /**
     * @brief 获取观察矩阵
     */
    [[nodiscard]] QMatrix4x4 camera_mat() const;
    /**
     * @brief 使相机向上(dist>0)或向下(dist<0)平移指定长度, 但不能脱离天空盒
     */
    void translate_up(float dist);
    /**
     * @brief 使相机向左(dist>0)或向右(dist<0)平移指定长度, 但不能脱离天空盒
     */
    void translate_left(float dist);
    /**
     * @brief 使相机向前(dist>0)或向后(dist<0)平移指定长度, 但不能脱离天空盒
     */
    void translate_forward(float dist);
    /**
     * @brief 使相机向上(degree>0)或向下(degree)旋转指定角度, 但不能脱离天空盒
     */
    void rotate_up(float degree);
    /**
     * @brief 使相机向左(degree>0)或向右(degree)旋转指定角度, 但不能脱离天空盒
     */
    void rotate_left(float degree);
    /**
     * @brief 调整相机放大倍数
     */
    void zoom_near(float dist);
};

#endif // CAMERA_H
