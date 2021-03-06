/**
 * @file   math.tpl
 * @author Vic P.
 * @brief  Template for Math
 */

// Misc

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

#define DEG2RAD(angle) ((angle) * M_PI / 180.0)
#define RAD2DEG(angle) ((angle) * 180.0 / M_PI)

template <typename T>
T absT(const T v)
{
  return v < T(0) ? -T(v) : T(v);
}

template <typename T>
T AlignUp(T v, T a = T(sizeof(size_t)))
{
  return ((v + (a - 1)) & ~(a - 1));
}

// Range Operation

template <typename TypeFrom, typename TypeTo>
TypeTo ConvRange(TypeFrom v, TypeFrom lo1, TypeFrom hi1, TypeTo lo2, TypeTo hi2)
{
  return TypeTo(lo2 + double(v - lo1) * double(hi2 - lo2) / double(hi1 - lo1));
}

template <typename TypeFrom, typename TypeTo>
TypeTo ConvRange(TypeFrom v, const std::pair<TypeFrom, TypeFrom>& from, const std::pair<TypeTo, TypeTo>& to)
{
  return TypeTo(to.second + double(v - from.first) * double(to.second - to.first) / double(from.second - from.first));
}

// PointT - The point template

template <int N, typename T>
class PointT
{
public:
  enum { D = N };

  PointT()
  {
    for (register int i = 0; i < D; i++)
    {
      m_v[i] = T(0);
    }
  }

  bool operator==(const PointT& right)
  {
    bool result = true;

    for (register int i = 0; i < D; i++)
    {
      result &= m_v[i] == right.m_v[i];
    }

    return result;
  }

  bool operator!=(const PointT& right)
  {
    return !(*this == right);
  }

  PointT& operator=(const PointT& right)
  {
    for (register int i = 0; i < D; i++)
    {
      m_v[i] = right.m_v[i];
    }

    return *this;
  }

  PointT& operator+=(const PointT& right)
  {
    for (register int i = 0; i < D; i++)
    {
      m_v[i] += right.m_v[i];
    }

    return *this;
  }

  PointT& operator-=(const PointT& right)
  {
    for (register int i = 0; i < D; i++)
    {
      m_v[i] -= right.m_v[i];
    }

    return *this;
  }

  PointT& operator*=(const PointT& v)
  {
    for (register int i = 0; i < D; i++)
    {
      this->m_v[i] *= v.m_v[i];
    }

    return *this;
  }

  PointT& operator/=(const PointT& v)
  {
    for (register int i = 0; i < D; i++)
    {
      this->m_v[i] /= v.m_v[i];
    }

    return *this;
  }

  PointT operator+(const PointT& right)
  {
    PointT result(*this);
    result += right;
    return result;
  }

  PointT operator-(const PointT& right)
  {
    PointT result(*this);
    result -= right;
    return result;
  }

  void Set(const T X = 0, const T Y = 0, const T Z = 0, const T W = 0)
  {
    SetEx(D, X, Y, Z, W);
  }

  const T& X() const
  {
    return m_v[0];
  }

  const T& Y() const
  {
    return m_v[1];
  }

  const T& Z() const
  {
    return m_v[2];
  }

  const T& W() const
  {
    return m_v[3];
  }

  const T* Data() const
  {
    return reinterpret_cast<T*>(&m_v);
  }

  friend std::ostream& operator<<(std::ostream& os, const PointT& point)
  {
    os << "(";

    for (register int i = 0; i < D; i++)
    {
      os << point.m_v[i] << (i < D - 1 ? ", " : "");
    }

    os << ")";

    return os;
  }

  friend std::wostream& operator<<(std::wostream& os, const PointT& point)
  {
    os << L"(";

    for (register int i = 0; i < D; i++)
    {
      os << point.m_v[i] << (i < D - 1 ? L", " : L"");
    }

    os << L")";

    return os;
  }

  double Distance(const PointT& point) const
  {
    double result = 0.;

    for (register int i = 0; i < D; i++)
    {
      double v = absT(point.m_v[i] - m_v[i]);
      result += v * v;
    }

    result = std::sqrt(result);

    return result;
  }

  PointT Midpoint(const PointT& point) const
  {
    PointT result = *this;
    result += point;
    result.Scale(0.5);
    return result;
  }

  PointT& Scale(const double ratio)
  {
    for (register int i = 0; i < D; i++)
    {
      this->m_v[i] = T(this->m_v[i] * ratio);
    }

    return *this;
  }

protected:
  void SetEx(const int count, const T X = 0, const T Y = 0, const T Z = 0, const T W = 0)
  {
    assert(count == D);

    if (D >= 1) m_v[0] = X;
    if (D >= 2) m_v[1] = Y;
    if (D >= 3) m_v[2] = Z;
    if (D >= 4) m_v[3] = W;
  }

protected:
  T m_v[D];
};

// Point2DT - The 2D point template

template <typename T>
class Point2DT : public PointT<2, T>
{
public:
  enum { D = 2 };

  Point2DT() : PointT<D, T>()
  {
  }

  Point2DT(const T X, const T Y)
  {
    this->SetEx(D, X, Y);
  }

  void Translate(const T X, const T Y)
  {
    Point2DT v(X, Y);
    *this += v;
  }
};

typedef Point2DT<int> P2I;
typedef Point2DT<float> P2F;
typedef Point2DT<double> P2D;

// Point3DT - The 3D point template

template <typename T>
class Point3DT : public PointT<3, T>
{
public:
  enum { D = 3 };

  Point3DT() : PointT<D, T>()
  {
  }

  Point3DT(const T X, const T Y, const T Z)
  {
    this->SetEx(D, X, Y, Z);
  }

  void Translate(const T X, const T Y, const T Z)
  {
    Point3DT v(X, Y, Z);
    *this += v;
  }
};

typedef Point3DT<int> P3I;
typedef Point3DT<float> P3F;
typedef Point3DT<double> P3D;

// Point4DT - The 4D point template

template <typename T>
class Point4DT : public PointT<4, T>
{
public:
  enum { D = 4 };

  Point4DT() : PointT<D, T>()
  {
  }

  Point4DT(const T X, const T Y, const T Z, const T W)
  {
    this->SetEx(D, X, Y, Z, W);
  }

  void Translate(const T X, const T Y, const T Z, const T W)
  {
    Point4DT v(X, Y, Z, W);
    *this += v;
  }
};

typedef Point4DT<int> P4I;
typedef Point4DT<float> P4F;
typedef Point4DT<double> P4D;

// VectorT - The vector template

template <int N, typename T>
class VectorT : public PointT<N, T>
{
public:
  enum { D = N };

  VectorT() : PointT<N, T>()
  {
  }

  VectorT(const PointT<N, T>& right)
  {
    static_cast<PointT<N, T>&>(*this) = right;
  }

  VectorT operator*(const VectorT& v)
  {
    VectorT result(*this);
    result *= v;
    return result;
  }

  VectorT operator/(const VectorT& v)
  {
    VectorT result(*this);
    result /= v;
    return result;
  }

  double Angle(const VectorT& v) const
  {
    /**
     * a = acos(v1 * v2) / (|v1| * |v2|)
     *   v1 - the first vector
     *   v2 - the second vector
     *   a  - the angle between both 2 given vectors
     */

    double result = Dot(v) / (this->Mag() * v.Mag());
    result = std::acos(result);
    return RAD2DEG(result);
  }

  double Mag() const // magnitude/length
  {
    double result = 0.;

    for (register int i = 0; i < D; i++)
    {
      result += this->m_v[i] * this->m_v[i];
    }

    result = std::sqrt(result);

    return result;
  }

  double Dot(const VectorT& v) const // dot/scalar product
  {
    double result = 0.;

    for (register int i = 0; i < D; i++)
    {
      result += this->m_v[i] * v.m_v[i];
    }

    return result;
  }

  VectorT Cross(const VectorT& v) // cross/vector product
  {
    /**
     * v = v1 x v2 = |v1| * |v2| * sin(a) * n
     *   v1 - the first vector
     *   v2 - the second vector
     *   a  - the angle between both a and b vector
     *   n  - normalized/unit vector perpendicular to a and b simultaneously
     *   v  - the cross/vector product of 2 given vectors
     */

    // VectorT result = *this;

    // VectorT n = ? ; // TODO: Vic. Uncompleted.
    // n.Normalize();

    // double a = this->Angle(v);
    // a = DEG2RAD(a);

    // result *= v;
    // result *= n;
    // result.Scale(std::sin(a));

    VectorT result;
    assert(0 && "no base implementation for cross product");
    return result;
  }

  VectorT& Normalize() // normalize/unit
  {
    this->Scale(1. / Mag());
    return *this;
  }
};

// Vector2DT - The 2D vector template

template <typename T>
class Vector2DT : public VectorT<2, T>
{
public:
  enum { D = 2 };

  Vector2DT() : VectorT<D, T>()
  {
  }

  Vector2DT(const T X, const T Y)
  {
    this->SetEx(D, X, Y);
  }

  Vector2DT(const PointT<2, T>& right)
  {
    static_cast<PointT<2, T>&>(*this) = right;
  }

  T Cross(const Vector2DT& v)
  {
    return this->X() * v.Y() - this->Y() * v.X();
  }
};

typedef Vector2DT<int> V2I;
typedef Vector2DT<float> V2F;
typedef Vector2DT<double> V2D;

// Vector3DT - The 3D vector template

template <typename T>
class Vector3DT : public VectorT<3, T>
{
public:
  enum { D = 3 };

  Vector3DT() : VectorT<D, T>()
  {
  }

  Vector3DT(const T X, const T Y, const T Z)
  {
    this->SetEx(D, X, Y, Z);
  }

  Vector3DT(const PointT<2, T>& right)
  {
    static_cast<PointT<3, T>&>(*this) = right;
  }

  Vector3DT Cross(const Vector3DT& v)
  {
    Vector3DT result(
      this->Y() * v.Z() - this->Z() * v.Y(),
      this->Z() * v.X() - this->X() * v.Z(),
      this->X() * v.Y() - this->Y() * v.X()
    );
    return result;
  }
};

typedef Vector3DT<int> V3I;
typedef Vector3DT<float> V3F;
typedef Vector3DT<double> V3D;

// Vector4DT - The 4D vector template

template <typename T>
class Vector4DT : public VectorT<4, T>
{
public:
  enum { D = 4 };

  Vector4DT() : VectorT<D, T>()
  {
  }

  Vector4DT(const T X, const T Y, const T Z, const T W)
  {
    this->SetEx(D, X, Y, Z, W);
  }

  Vector4DT(const PointT<4, T>& right)
  {
    static_cast<PointT<4, T>&>(*this) = right;
  }

  Vector4DT Cross(const Vector4DT& v)
  {
    Vector4DT result;
    assert(0 && "incompatible dimensions for cross product");
    return result;
  }
};

typedef Vector4DT<int> V4I;
typedef Vector4DT<float> V4F;
typedef Vector4DT<double> V4D;

// RectT - The rectangle template (Window Coordinate System)

// .-----> X
// |
// |
// v
// Y

template <typename T>
class RectT
{
public:
  RectT()
  {
    Set(T(0), T(0), T(0), T(0));
  }

  RectT(const T l, const T t, const T r, const T b)
  {
    Set(l, t, r, b);
  }

  RectT(const T nWidth, const T nHeight)
  {
    Set(nWidth, nHeight);
  }

  RectT(const RECT& rect)
  {
    Set(T(rect.left), T(rect.top), T(rect.right), T(rect.bottom));
  }

  void Set(const T l, const T t, const T r, const T b)
  {
    m_left   = T(l);
    m_right  = T(r);
    m_top    = T(t);
    m_bottom = T(b);
  }

  void Set(const T nWidth, const T nHeight)
  {
    Set(0, 0, nWidth, nHeight);
  }

  bool Empty()
  {
    return (m_left == T(0) && m_right == T(0) && m_top == T(0) && m_bottom == T(0));
  }

  Point2DT<T> Origin()
  {
    return Point2DT<T>(m_left, m_top);
  }

  Point2DT<T> Center()
  {
    return Point2DT<T>(m_left + T(Width() / 2), m_top + T(Height() / 2));
  }

  T Width()
  {
    return T(abs(m_right - m_left));
  }

  T Height()
  {
    return T(abs(m_bottom - m_top));
  }

  void Translate(const T cx, const T cy)
  {
    m_left   += cx;
    m_right  += cx;
    m_top    += cy;
    m_bottom += cy;
  }

  void Translate(const Vector2DT<T>& vector)
  {
    m_left   += vector.X();
    m_right  += vector.X();
    m_top    += vector.Y();
    m_bottom += vector.Y();
  }

  RectT Resize(const T dx, const T dy)
  {
    return RectT(m_left + dx, m_top - dy, m_right - dx, m_bottom + dy);
  }

  void Flip()
  {
    m_top   += m_bottom;
    m_bottom = m_top - m_bottom;
    m_top   -= m_bottom;
  }

  // operator RECT() const // cast to RECT
  // {
  //   RECT rect;

  //   rect.left   = m_left;
  //   rect.top    = m_top;
  //   rect.right  = m_right;
  //   rect.bottom = m_bottom;

  //   return rect;
  // }

  friend std::ostream& operator<<(std::ostream& os, const RectT& v)
  {
    os << "("
       << v.m_left << ", "
       << v.m_top << ", "
       << v.m_right << ", "
       << v.m_bottom << ")";

    return os;
  }

  friend std::wostream& operator<<(std::wostream& os, const RectT& v)
  {
    os << L"("
       << v.m_left << L", "
       << v.m_top << L", "
       << v.m_right << L", "
       << v.m_bottom << L")";

    return os;
  }

private:
  T m_left;
  T m_top;
  T m_right;
  T m_bottom;
};

typedef RectT<int> R4I;
typedef RectT<float> R4F;
typedef RectT<double> R4D;
