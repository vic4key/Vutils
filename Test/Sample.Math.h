#pragma once

#include "Sample.h"

DEF_SAMPLE(Math)
{
  std::cout << "GCD : " << vu::gcd(3, 9, 27, 81) << std::endl;
  std::cout << "LCM : " << vu::lcm(3, 9, 27, 81) << std::endl;

  {
    vu::p2i p1(1, 2), p2(3, 4), p3;

    p3 =  p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 +  p2;
    p3 -  p1;

    p3.translate(5, 6);

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    std::tcout << p1.distance(p2) << std::endl;
    std::tcout << p1.mid(p2) << std::endl;
    std::tcout << p1.scale(2) << std::endl;
  }

  {
    vu::p3i p1(1, 2, 3), p2(4, 5, 6), p3;

    p3 =  p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 +  p2;
    p3 -  p1;

    p3.translate(7, 8, 9);

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    std::tcout << p1.distance(p2) << std::endl;
    std::tcout << p1.mid(p2) << std::endl;
    std::tcout << p1.scale(2) << std::endl;
  }

  {
    vu::p4i p1(1, 2, 3, 4), p2(5, 6, 7, 8), p3;

    p3 =  p1;
    p3 == p1;
    p3 != p2;
    p3 += p1;
    p3 -= p2;
    p3 +  p2;
    p3 -  p1;

    p3.translate(3, 4, 5, 6);

    std::tcout << p1 << std::endl;
    std::tcout << p2 << std::endl;
    std::tcout << p3 << std::endl;

    std::tcout << p1.distance(p2) << std::endl;
    std::tcout << p1.mid(p2) << std::endl;
    std::tcout << p1.scale(2) << std::endl;
  }

  {
    vu::v2d v21(1, 2), v22(3, 4);
    std::tcout << v21 << v22 << std::endl;
    std::tcout << v21 + v22 << std::endl;
    std::tcout << v21 - v22 << std::endl;
    std::tcout << v21 * v22 << std::endl;
    std::tcout << v21 / v22 << std::endl;
    std::tcout << v21.mag() << std::endl;
    std::tcout << v21.dot(v22) << std::endl;
    std::tcout << v21.cross(v22) << std::endl;
    std::tcout << v21.angle(v22) << std::endl;
    std::tcout << v21.normalize() << std::endl;

    vu::v3d v31(1, 2, 3), v32(4, 5, 6);
    std::tcout << v31 << v32 << std::endl;
    std::tcout << v31 + v32 << std::endl;
    std::tcout << v31 - v32 << std::endl;
    std::tcout << v31 * v32 << std::endl;
    std::tcout << v31 / v32 << std::endl;
    std::tcout << v31.mag() << std::endl;
    std::tcout << v31.dot(v32) << std::endl;
    std::tcout << v31.cross(v32) << std::endl;
    std::tcout << v31.angle(v32) << std::endl;
    std::tcout << v31.normalize() << std::endl;

    vu::v4d v41(1, 2, 3, 4), v42(5, 6, 7, 8);
    std::tcout << v41 << v42 << std::endl;
    std::tcout << v41 + v42 << std::endl;
    std::tcout << v41 - v42 << std::endl;
    std::tcout << v41 * v42 << std::endl;
    std::tcout << v41 / v42 << std::endl;
    std::tcout << v41.mag() << std::endl;
    std::tcout << v41.dot(v42) << std::endl;
    // std::tcout << v41.cross(v42) << std::endl;
    std::tcout << v41.angle(v42) << std::endl;
    std::tcout << v41.normalize() << std::endl;
  }

  {
    vu::r4i rect(100, 200);
    rect.center();
    std::tcout << rect << std::endl;
  }

  {
    vu::BigNumber big1 = 1234567890, big2, big3 = 1;

    big2 = "9876543210123456789098765432101234567890";
    std::cout << big1 * big2 * 123456 << std::endl;

    big1 = big2 + 1234567890;
    big1 = big2 - "123456789012345678901234567890";
    big1 = big2 * big3;
    std::cout << big1 << std::endl;

    big1 = pow(big2, 3);
    big1 = pow(987654321LL, 4);
    big1 = pow("1234567890", 5);
    std::cout << big1 << std::endl;
  }

  return vu::VU_OK;
}
