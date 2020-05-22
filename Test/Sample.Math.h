#pragma once

#include "Sample.h"

DEF_SAMPLE(Math)
{
  std::cout << "GCD : " << vu::GCD(3, 9, 27, 81) << std::endl;
  std::cout << "LCM : " << vu::LCM(3, 9, 27, 81) << std::endl;

  {
    vu::Point2DT<int> p1(1, 2), p2(3, 4), p3;

    p3 = p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 -  p1;
    p3 +  p2;

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    p1.Set(1, 1);
    p2.Set(2, 2);
    std::tcout << p1.DistanceTo(p2) << std::endl;
  }

  {
    vu::Point3DT<int> p1(1, 2, 3), p2(4, 5, 6), p3;

    p3 =  p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 -  p1;
    p3 +  p2;

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    p1.Set(1, 1, 1);
    p2.Set(2, 2, 2);
    std::tcout << p1.DistanceTo(p2) << std::endl;
  }

  {
    vu::Vector2DT<int> v2(1, 2);
    std::tcout << v2 << std::endl;
    std::tcout << v2.Length() << std::endl;

    vu::Vector3DT<int> v3(1, 2, 3);
    std::tcout << v3 << std::endl;
    std::tcout << v3.Length() << std::endl;
  }

  {
    vu::RectT<int> rect(100, 200);
    rect.Center();
    std::tcout << rect << std::endl;
  }

  return vu::VU_OK;
}
