#include "SDK.h"

#pragma once

#include <cmath> // For std::sqrt
#include <stdexcept> // For std::runtime_error

class Vector {
public:
    float x, y, z;

    // Constructors
    Vector() : x(0.0f), y(0.0f), z(0.0f) {} // Default constructor
    Vector(float x, float y, float z) : x(x), y(y), z(z) {
        if (!isValid()) {
            throw std::runtime_error("Invalid vector initialized.");
        }
    }

    // Initialize vector components
    void init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) {
        x = ix; y = iy; z = iz;
        if (!isValid()) {
            throw std::runtime_error("Invalid vector initialized.");
        }
    }

    // Check if the vector is valid (not NaN or Inf)
    bool isValid() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }

    // Access operators
    float operator[](int index) const {
        if (index < 0 || index >= 3) {
            throw std::out_of_range("Index out of range");
        }
        return *(&x + index);
    }

    float& operator[](int index) {
        if (index < 0 || index >= 3) {
            throw std::out_of_range("Index out of range");
        }
        return *(&x + index);
    }

    // Set vector components to zero
    void zero() {
        x = y = z = 0.0f;
    }

    // Comparison operators
    bool operator==(const Vector3D& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    bool operator!=(const Vector3D& other) const {
        return !(*this == other);
    }

    // Arithmetic operators
    Vector3D& operator+=(const Vector3D& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    Vector3D& operator-=(const Vector3D& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }

    Vector3D& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    Vector3D& operator/=(float scalar) {
        if (scalar == 0.0f) {
            throw std::runtime_error("Division by zero.");
        }
        float invScalar = 1.0f / scalar;
        x *= invScalar; y *= invScalar; z *= invScalar;
        return *this;
    }

    // Unary operators
    Vector3D operator-() const {
        return Vector3D(-x, -y, -z);
    }

    // Binary operators
    Vector3D operator+(const Vector3D& v) const {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }

    Vector3D operator-(const Vector3D& v) const {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }

    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(float scalar) const {
        if (scalar == 0.0f) {
            throw std::runtime_error("Division by zero.");
        }
        return *this * (1.0f / scalar);
    }

    // Dot product
    float dot(const Vector3D& other) const {
        return (x * other.x + y * other.y + z * other.z);
    }

    // Length and distance calculations
    float length() const {
        return std::sqrt(lengthSquared());
    }

    float lengthSquared() const {
        return (x * x + y * y + z * z);
    }

    bool isZero(float tolerance = 1e-5f) const {
        return (std::fabs(x) < tolerance && std::fabs(y) < tolerance && std::fabs(z) < tolerance);
    }

    float distanceSquared(const Vector3D& other) const {
        return (*this - other).lengthSquared();
    }

    // Normalize the vector in place and return its length before normalization
    float normalize() {
        float len = length();
        if (len > 1e-5f) { // Avoid division by zero
            *this /= len;
            return len;
        }
        return 0.0f; // Return zero if the vector is too small to normalize
    }
};

// Cross product function as a free function
inline Vector3D crossProduct(const Vector3D& a, const Vector3D& b) {
    return Vector3D(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}