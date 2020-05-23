/**
 * @file   math.tpl
 * @author Vic P.
 * @brief  Template for Math
 */

// Misc

template <typename T>
T absT(const T v)
{
  return v < T(0) ? -T(v) : T(v);
}

// Range Validation

template <typename T>
bool InRange(const T lo, const T hi, const T v)
{
  return (v > lo) && (v < hi);
}

template <typename T>
bool InRange(const std::pair<T, T>& range, const T v)
{
  return (v > range.first) && (v < range.second);
}

template <typename T>
bool InERange(const T lo, const T hi, const T v)
{
  return (v >= lo) && (v <= hi);
}

template <typename T>
bool InERange(const std::pair<T, T>& range, const T v)
{
  return (v >= range.first) && (v <= range.second);
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

  T* Data()
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

// VectorT - The vector template

template <int N, typename T>
class VectorT : public PointT<N, T>
{
public:
  enum { D = N };

  VectorT() : PointT<N, T>()
  {
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
    VectorT result;

    // YOUR CODE HERE

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
};

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
};

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
};

// RectT - The rectangle template (Window Coordinate System)

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
