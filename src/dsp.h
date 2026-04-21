/*
Peter Semiletov, 2023
*/


#ifndef DSP_H
#define DSP_H
/*
#if defined(__clang__)
//
#elif defined(__GNUC__) || defined(__GNUG__)

#define MXCSR_DAZ (1<<6)
#define MXCSR_FTZ (1<<15)

#elif defined(_MSC_VER)
//
#endif
*/

//#define _USE_MATH_DEFINES

#include <cmath>
#include <limits>
//#include <ostream>
#include <iostream>
#include <vector>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif



extern float db_scale;

inline float db2lin (float db)
{
  return (float) exp (db * db_scale);
}


void init_db();



inline bool float_greater_than (float a, float b)
{
  return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * std::numeric_limits<double>::epsilon());
}


inline bool float_less_than (float a, float b)
{
  return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * std::numeric_limits<double>::epsilon());
}


inline bool float_equal (float x, float y)
{
  return std::abs(x - y) <= std::numeric_limits<double>::epsilon() * std::abs(x);
}


inline float scale_val (float val, float from_min, float from_max, float to_min, float to_max)
{
  return (val - from_min) * (to_max - to_min) /
          (from_max - from_min) + to_min;
}



/*************** SATURATION/DIST/OVERDRIVE *******************/

float jimi_fuzz (float inputSample, float level, float distortion);
//float fuzz (float input, float level, float intensity);
float overdrive (float input, float drive, float level);
float gritty_guitar_distortion (float input_sample, float distortion_level);
float warmify (float x, float warmth);



/*************** MISC *******************/


float soft_limit (float input);
float apply_aggro (float input, float resonance_amount);


#endif
