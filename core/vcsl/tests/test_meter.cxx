//*****************************************************************************
// File name: test_meter.cxx
// Description: Test the vcsl_meter class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/16| Fran�ois BERTEL          |Creation
//*****************************************************************************

//:
// \file

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcsl/vcsl_meter.h>

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  vcsl_meter_sptr m=vcsl_meter::instance();

  return 0;
}
