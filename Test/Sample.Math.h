#pragma once

#include "Sample.h"

DEF_SAMPLE(Math)
{
  std::cout << "GCD : " << vu::GCD(3, 9, 27, 81) << std::endl;
  std::cout << "LCM : " << vu::LCM(3, 9, 27, 81) << std::endl;

/*
  {
    vu::Point2DT<int> p1(1, 2), p2(3, 4), p3;

    p3 =  p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 +  p2;
    p3 -  p1;

    p3.Translate(5, 6);

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    p1.Set(1, 1);
    p2.Set(2, 2);
    std::tcout << p1.Distance(p2) << std::endl;
  }

  {
    vu::Point3DT<int> p1(1, 2, 3), p2(4, 5, 6), p3;

    p3 =  p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 +  p2;
    p3 -  p1;

    p3.Translate(7, 8, 9);

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    p1.Set(1, 1, 1);
    p2.Set(2, 2, 2);
    std::tcout << p1.Distance(p2) << std::endl;
  }

  {
    vu::Point4DT<int> p1(1, 2, 3, 4), p2(5, 6, 7, 8), p3;

    p3 =  p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 +  p2;
    p3 -  p1;

    p3.Translate(3, 4, 5, 6);

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    p1.Set(1, 1, 1);
    p2.Set(2, 2, 2);
    std::tcout << p1.Distance(p2) << std::endl;
  }*/

  {
    // vu::Vector2DT<double> v21(1, 2), v22(3, 4);
    // std::tcout << v21 << v22 << std::endl;
    // std::tcout << v21 + v22 << std::endl;
    // std::tcout << v21 - v22 << std::endl;
    // std::tcout << v21 * v22 << std::endl;
    // std::tcout << v21 / v22 << std::endl;
    // std::tcout << v21.Mag() << std::endl;
    // std::tcout << v21.Dot(v22) << std::endl;
    // std::tcout << v21.Angle(v22) << std::endl;
    // std::tcout << v21.Normalize() << std::endl;

    vu::Vector3DT<double> v31(1, 2, 3), v32(4, 5, 6);
    // std::tcout << v31 << v32 << std::endl;
    // std::tcout << v31 + v32 << std::endl;
    // std::tcout << v31 - v32 << std::endl;
    // std::tcout << v31 * v32 << std::endl;
    // std::tcout << v31 / v32 << std::endl;
    std::tcout << v31.Mag() << std::endl;
    std::tcout << v31.Dot(v32) << std::endl;
    std::tcout << v31.Cross(v32) << std::endl;
    std::tcout << v31.Angle(v32) << std::endl;
    std::tcout << v31.Normalize() << std::endl;

    // vu::Vector4DT<double> v41(1, 2, 3, 4), v42(5, 6, 7, 8);
    // std::tcout << v41 << v42 << std::endl;
    // std::tcout << v41 + v42 << std::endl;
    // std::tcout << v41 - v42 << std::endl;
    // std::tcout << v41 * v42 << std::endl;
    // std::tcout << v41 / v42 << std::endl;
    // std::tcout << v41.Mag() << std::endl;
    // std::tcout << v41.Dot(v42) << std::endl;
    // std::tcout << v41.Angle(v42) << std::endl;
    // std::tcout << v41.Normalize() << std::endl;
  }

  {
    // vu::RectT<int> rect(100, 200);
    // rect.Center();
    // std::tcout << rect << std::endl;
  }

  return vu::VU_OK;
}
