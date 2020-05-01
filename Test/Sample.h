#pragma once

#include <Vu.h>

/**
 * CSample
 */

class Sample
{
public:
	Sample() {}
	virtual ~Sample() {}

	virtual vu::IResult init()
	{
		return vu::VU_OK;
	}

	virtual vu::IResult run()
	{
		return vu::VU_OK;
	}
};

/*

#include "Sample.h"

class Example : public Sample
{
public:
	virtual vu::IResult run();
};

vu::IResult Example::run()
{
  return vu::VU_OK;
}

*/

#define DEF_SAMPLE(sample)\
class sample : public Sample\
{\
public:\
  virtual vu::IResult run();\
};\
\
vu::IResult sample::run()

/*

#include "Sample.h"

DEF_SAMPLE(Sample)
{
  // YOUR CODE HERE //

  return vu::VU_OK;
}

*/
