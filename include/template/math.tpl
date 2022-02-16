/**
 * @file   math.tpl
 * @author Vic P.
 * @brief  Template for Math
 */

// Math

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

#define DEG2RAD(angle) ((angle) * M_PI / 180.0)
#define RAD2DEG(angle) ((angle) * 180.0 / M_PI)

template <typename T>
T abs_t(const T v)
{
  return v < T(0) ? -T(v) : T(v);
}

template <typename T>
T align_up_t(T v, T a = T(sizeof(size_t)))
{
  return ((v + (a - 1)) & ~(a - 1));
}

// Range Operation

template <typename type_trom, typename type_to>
type_to conv_range(type_trom v, type_trom lo1, type_trom hi1, type_to lo2, type_to hi2)
{
  return type_to(lo2 + double(v - lo1) * double(hi2 - lo2) / double(hi1 - lo1));
}

template <typename type_trom, typename type_to>
type_to conv_range(
  const type_trom v,
  const std::pair<type_trom, type_trom>& from,
  const std::pair<type_to, type_to>& to)
{
  return type_to(to.second + double(v - from.first) * double(to.second - to.first) / double(from.second - from.first));
}

// point_t - The point template

template <int N, typename T>
class point_t
{
public:
  enum { D = N };

  point_t()
  {
    for (int i = 0; i < D; i++)
    {
      m_v[i] = T(0);
    }
  }

  bool operator==(const point_t& right)
  {
    bool result = true;

    for (int i = 0; i < D; i++)
    {
      result &= m_v[i] == right.m_v[i];
    }

    return result;
  }

  bool operator!=(const point_t& right)
  {
    return !(*this == right);
  }

  point_t& operator=(const point_t& right)
  {
    for (int i = 0; i < D; i++)
    {
      m_v[i] = right.m_v[i];
    }

    return *this;
  }

  point_t& operator+=(const point_t& right)
  {
    for (int i = 0; i < D; i++)
    {
      m_v[i] += right.m_v[i];
    }

    return *this;
  }

  point_t& operator-=(const point_t& right)
  {
    for (int i = 0; i < D; i++)
    {
      m_v[i] -= right.m_v[i];
    }

    return *this;
  }

  point_t& operator*=(const point_t& v)
  {
    for (int i = 0; i < D; i++)
    {
      this->m_v[i] *= v.m_v[i];
    }

    return *this;
  }

  point_t& operator/=(const point_t& v)
  {
    for (int i = 0; i < D; i++)
    {
      this->m_v[i] /= v.m_v[i];
    }

    return *this;
  }

  point_t operator+(const point_t& right)
  {
    point_t result(*this);
    result += right;
    return result;
  }

  point_t operator-(const point_t& right)
  {
    point_t result(*this);
    result -= right;
    return result;
  }

  void set(const T X = 0, const T Y = 0, const T Z = 0, const T W = 0)
  {
    set_ex(D, X, Y, Z, W);
  }

  void x(const T& v)
  {
    m_v[0] = v;
  }

  const T& x() const
  {
    return m_v[0];
  }

  void y(const T& v)
  {
    m_v[1] = v;
  }

  const T& y() const
  {
    return m_v[1];
  }

  void z(const T& v)
  {
    m_v[2] = v;
  }

  const T& z() const
  {
    return m_v[2];
  }

  void w(const T& v)
  {
    m_v[3] = v;
  }

  const T& w() const
  {
    return m_v[3];
  }

  const T* data() const
  {
    return reinterpret_cast<T*>(&m_v);
  }

  friend std::ostream& operator<<(std::ostream& os, const point_t& point)
  {
    os << "(";

    for (int i = 0; i < D; i++)
    {
      os << point.m_v[i] << (i < D - 1 ? ", " : "");
    }

    os << ")";

    return os;
  }

  friend std::wostream& operator<<(std::wostream& os, const point_t& point)
  {
    os << L"(";

    for (int i = 0; i < D; i++)
    {
      os << point.m_v[i] << (i < D - 1 ? L", " : L"");
    }

    os << L")";

    return os;
  }

  double distance(const point_t& point) const
  {
    double result = 0.;

    for (int i = 0; i < D; i++)
    {
      double v = abs_t(point.m_v[i] - m_v[i]);
      result += v * v;
    }

    result = std::sqrt(result);

    return result;
  }

  point_t mid(const point_t& point) const
  {
    point_t result = *this;
    result += point;
    result.scale(0.5);
    return result;
  }

  point_t& scale(const double ratio)
  {
    for (int i = 0; i < D; i++)
    {
      this->m_v[i] = T(this->m_v[i] * ratio);
    }

    return *this;
  }

protected:
  void set_ex(const int count, const T X = 0, const T Y = 0, const T Z = 0, const T W = 0)
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

// point_2d_t - The 2D point template

template <typename T>
class point_2d_t : public point_t<2, T>
{
public:
  enum { D = 2 };

  point_2d_t() : point_t<D, T>()
  {
  }

  point_2d_t(const T X, const T Y)
  {
    this->set_ex(D, X, Y);
  }

  void translate(const T X, const T Y)
  {
    point_2d_t v(X, Y);
    *this += v;
  }
};

typedef point_2d_t<int> p2i;
typedef point_2d_t<float> p2f;
typedef point_2d_t<double> p2d;

// point_3d_t - The 3D point template

template <typename T>
class point_3d_t : public point_t<3, T>
{
public:
  enum { D = 3 };

  point_3d_t() : point_t<D, T>()
  {
  }

  point_3d_t(const T X, const T Y, const T Z)
  {
    this->set_ex(D, X, Y, Z);
  }

  void translate(const T X, const T Y, const T Z)
  {
    point_3d_t v(X, Y, Z);
    *this += v;
  }
};

typedef point_3d_t<int> p3i;
typedef point_3d_t<float> p3f;
typedef point_3d_t<double> p3d;

// point_4d_t - The 4D point template

template <typename T>
class point_4d_t : public point_t<4, T>
{
public:
  enum { D = 4 };

  point_4d_t() : point_t<D, T>()
  {
  }

  point_4d_t(const T X, const T Y, const T Z, const T W)
  {
    this->set_ex(D, X, Y, Z, W);
  }

  void translate(const T X, const T Y, const T Z, const T W)
  {
    point_4d_t v(X, Y, Z, W);
    *this += v;
  }
};

typedef point_4d_t<int> p4i;
typedef point_4d_t<float> p4f;
typedef point_4d_t<double> p4d;

// vector_t - The vector template

template <int N, typename T>
class vector_t : public point_t<N, T>
{
public:
  enum { D = N };

  vector_t() : point_t<N, T>()
  {
  }

  vector_t(const point_t<N, T>& right)
  {
    static_cast<point_t<N, T>&>(*this) = right;
  }

  vector_t operator*(const vector_t& v)
  {
    vector_t result(*this);
    result *= v;
    return result;
  }

  vector_t operator/(const vector_t& v)
  {
    vector_t result(*this);
    result /= v;
    return result;
  }

  double angle(const vector_t& v) const
  {
    /**
     * a = acos(v1 * v2) / (|v1| * |v2|)
     *   v1 - the first vector
     *   v2 - the second vector
     *   a  - the angle between both 2 given vectors
     */

    double result = dot(v) / (this->mag() * v.mag());
    result = std::acos(result);
    return RAD2DEG(result);
  }

  double mag() const // magnitude/length
  {
    double result = 0.;

    for (int i = 0; i < D; i++)
    {
      result += this->m_v[i] * this->m_v[i];
    }

    result = std::sqrt(result);

    return result;
  }

  double dot(const vector_t& v) const // dot/scalar product
  {
    double result = 0.;

    for (int i = 0; i < D; i++)
    {
      result += this->m_v[i] * v.m_v[i];
    }

    return result;
  }

  vector_t cross(const vector_t& v) // cross/vector product
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
     // n.normalize();

     // double a = this->angle(v);
     // a = DEG2RAD(a);

     // result *= v;
     // result *= n;
     // result.scale(std::sin(a));

    vector_t result;
    assert(0 && "no base implementation for cross product");
    return result;
  }

  vector_t& normalize() // normalize/unit
  {
    this->scale(1. / mag());
    return *this;
  }
};

// vector_2d_t - The 2D vector template

template <typename T>
class vector_2d_t : public vector_t<2, T>
{
public:
  enum { D = 2 };

  vector_2d_t() : vector_t<D, T>()
  {
  }

  vector_2d_t(const T X, const T Y)
  {
    this->set_ex(D, X, Y);
  }

  vector_2d_t(const point_t<2, T>& right)
  {
    static_cast<point_t<2, T>&>(*this) = right;
  }

  T cross(const vector_2d_t& v)
  {
    return this->x() * v.y() - this->y() * v.x();
  }
};

typedef vector_2d_t<int> v2i;
typedef vector_2d_t<float> v2f;
typedef vector_2d_t<double> v2d;

// vector_3d_t - The 3D vector template

template <typename T>
class vector_3d_t : public vector_t<3, T>
{
public:
  enum { D = 3 };

  vector_3d_t() : vector_t<D, T>()
  {
  }

  vector_3d_t(const T X, const T Y, const T Z)
  {
    this->set_ex(D, X, Y, Z);
  }

  vector_3d_t(const point_t<2, T>& right)
  {
    static_cast<point_t<3, T>&>(*this) = right;
  }

  vector_3d_t cross(const vector_3d_t& v)
  {
    vector_3d_t result(
      this->y() * v.z() - this->z() * v.y(),
      this->z() * v.x() - this->x() * v.z(),
      this->x() * v.y() - this->y() * v.x()
    );
    return result;
  }
};

typedef vector_3d_t<int> v3i;
typedef vector_3d_t<float> v3f;
typedef vector_3d_t<double> v3d;

// vector_4d_t - The 4D vector template

template <typename T>
class vector_4d_t : public vector_t<4, T>
{
public:
  enum { D = 4 };

  vector_4d_t() : vector_t<D, T>()
  {
  }

  vector_4d_t(const T X, const T Y, const T Z, const T W)
  {
    this->set_ex(D, X, Y, Z, W);
  }

  vector_4d_t(const point_t<4, T>& right)
  {
    static_cast<point_t<4, T>&>(*this) = right;
  }

  vector_4d_t cross(const vector_4d_t& v)
  {
    vector_4d_t result;
    assert(0 && "incompatible dimensions for cross product");
    return result;
  }
};

typedef vector_4d_t<int> v4i;
typedef vector_4d_t<float> v4f;
typedef vector_4d_t<double> v4d;

// rect_t - The rectangle template (window coordinate system)
//
//  .-----> Y
//  |
//  |
//  v
//  X

template <typename T>
class rect_t
{
public:
  enum flip_t
  {
    vertical,
    horizontal,
  };

  rect_t()
  {
    set(T(0), T(0), T(0), T(0));
  }

  rect_t(const T l, const T t, const T r, const T b)
  {
    set(l, t, r, b);
  }

  rect_t(const T width, const T height)
  {
    set(width, height);
  }

  const T& left() const
  {
    return m_left;
  }

  void left(const T v)
  {
    m_left = v;
  }

  const T& right() const
  {
    return m_right;
  }

  void right(const T v)
  {
    m_right = v;
  }

  const T& top() const
  {
    return m_top;
  }

  void top(const T v)
  {
    m_top = v;
  }

  const T& bottom() const
  {
    return m_bottom;
  }

  void bottom(const T v)
  {
    m_bottom = v;
  }

  void set(const T l, const T t, const T r, const T b)
  {
    m_left = T(l);
    m_right = T(r);
    m_top = T(t);
    m_bottom = T(b);
    m_fliped[flip_t::vertical] = false;
    m_fliped[flip_t::horizontal] = false;
  }

  void set(const rect_t& rect)
  {
    set(rect.left(), rect.top(), rect.right(), rect.bottom());
  }

  void set(const T width, const T height)
  {
    set(0, 0, width, height);
  }

  bool zero() const
  {
    return (m_left == T(0) && m_right == T(0) && m_top == T(0) && m_bottom == T(0));
  }

  point_2d_t<T> origin() const
  {
    return point_2d_t<T>(
      m_fliped[flip_t::horizontal] ? m_right : m_left,
      m_fliped[flip_t::vertical] ? m_bottom : m_top);
  }

  point_2d_t<T> center() const
  {
    const auto p = this->origin();
    return point_2d_t<T>(p.x() + T(width() / 2), p.y() + T(height() / 2));
  }

  T width() const
  {
    return T(abs(m_right - m_left));
  }

  T height() const
  {
    return T(abs(m_bottom - m_top));
  }

  T low() const
  {
    auto w = this->width();
    auto h = this->height();
    return w < h ? w : h;
  }

  T high() const
  {
    auto w = this->width();
    auto h = this->height();
    return w > h ? w : h;
  }

  float aspect() const
  {
    return float(this->width()) / float(this->height());
  }

  void flip(flip_t v)
  {
    if (v == flip_t::horizontal)
    {
      m_left += m_right;
      m_right = m_left - m_right;
      m_left -= m_right;
      m_fliped[v] = !m_fliped[v];
    }
    else if (v == flip_t::vertical)
    {
      m_top += m_bottom;
      m_bottom = m_top - m_bottom;
      m_top -= m_bottom;
      m_fliped[v] = !m_fliped[v];
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const rect_t& v)
  {
    const auto p = v.origin();
    os << "(" << p.x() << ", " << p.y() << ", " << v.width() << ", " << v.height() << ")";
    return os;
  }

  friend std::wostream& operator<<(std::wostream& os, const rect_t& v)
  {
    const auto p = v.origin();
    os << L"(" << p.x() << L", " << p.y() << L", " << v.width() << L", " << v.height() << L")";
    return os;

    return os;
  }

private:
  T m_left;
  T m_top;
  T m_right;
  T m_bottom;
  bool m_fliped[2]; // refer to flip_t
};

typedef rect_t<int> r4i;
typedef rect_t<float> r4f;
typedef rect_t<double> r4d;
