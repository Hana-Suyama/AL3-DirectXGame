#pragma once
#include <Matrix4x4.h>
#define _USE_MATH_DEFINES
#include "cmath"
#include <Vector3.h>
#include <cassert>

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

//行列の加法
Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2);

//行列の減法
Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2);

//行列の積
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

//単位行列
Matrix4x4 MakeIdentity4x4();

//ベクトルと行列の積
Vector3 Multiply(Vector3 vector, Matrix4x4 matrix);

//1.平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

//2.拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

//3.座標返還
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

//1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

//2.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

//3.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// 3. ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

//Vector3同士の加算
Vector3 Vec3Addition(Vector3& a, Vector3& b);
Vector3 Vec3Addition(const Vector3& a, Vector3& b);

//Vector3同士の減算
Vector3 Vec3Subtraction(Vector3& a, Vector3& b);

//Vector3同士の乗算
Vector3 Vec3Multiplication(Vector3& a, Vector3& b);
Vector3 Vec3Multiplication(Vector3 a, Vector3 b);

//Vector3とfloatの乗算
Vector3 Vec3FloatMultiplication(const Vector3& a, const float& b);

//ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

//正規化
Vector3 Normalize(Vector3& a);