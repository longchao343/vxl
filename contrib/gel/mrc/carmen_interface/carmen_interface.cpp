//-*- c++ -*-------------------------------------------------------------------
// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//--------------------------------------------------------------

//:
//  \file
//
// Class : gauss_simulator
//
// Modifications : see gauss_simulator.h
//
//-----------------------------------------------------------------------------
//#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
//--------------------------------------------
// Carmen Includes
#define DEFINED_MAX_MIN
#undef REALLOC
#undef NEW
#undef Status
#include <Carmen/Carmen.h>
#include <CarmenModels/Perspective/pp.h>
#include "carmen_interface.h"

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//: Constructor
carmen_interface::carmen_interface()
{
  PerspectiveCamera::Enregister();
  _carmen = new Carmen;
}

//: Destructor
carmen_interface::~carmen_interface()
{
  delete _carmen;
}

//-----------------------------------------------------------------------
//: intialize carmen
void carmen_interface::set_carmen_camera(int view_no)
{
  if(!_carmen)
    return;
  bool stat = (bool)_carmen->create_camera(view_no, "PerspectiveCamera");
  if(!stat)
    {
      vcl_cerr << _carmen->geterror() << vcl_endl;
      return;
    }
}
//----------------------------------------------------------------------
//: add a full 3d correspondence to Carmen
//
bool carmen_interface::add_full_correspondence(int view_no, int point_id,
                                               double x, double y, double z,
                                               float u, float v)
{
  double default_3d_point_sdev = 0.0;
  double default_2d_point_sdev = 1.0;
  bool stat=false;

  //Set up the primary point3D.
  stat = (bool)_carmen->set_primary_parameter(point_id, "X", x,
                                              default_3d_point_sdev, FIX);
  if(!stat)
    {
      vcl_cerr << _carmen->geterror() << vcl_endl;
      return stat;
    }

  stat = (bool)_carmen->set_primary_parameter(point_id, "Y", y,
                                              default_3d_point_sdev, FIX);
  if(!stat)
    {
      vcl_cerr << _carmen->geterror() << vcl_endl;
      return stat;
    }
  stat = (bool)_carmen->set_primary_parameter(point_id, "Z", z,
                                              default_3d_point_sdev, FIX);
  if(!stat)
    {
      vcl_cerr << _carmen->geterror() << vcl_endl;
      return stat;
    }
  //define the corresponding 2d secondary
  stat = _carmen->define_control_point(view_no, point_id, u, v,
                                       default_2d_point_sdev);
  if(!stat)
    {
      vcl_cerr << _carmen->geterror() << vcl_endl;
      return stat;
    }
  return stat;
}
//-------------------------------------------------------------------------
//: load a correspondence file
//
bool carmen_interface::load_correspondence_file(const string& file_path)
{
  vcl_ifstream instr(file_path.c_str());
  if(!instr)
    {
      vcl_cout <<"In carmen_interface::load_correspondence_file()"
               <<" - can't open file  " << file_path.c_str() << vcl_endl;
      return false;
    }
  char buf[100];
  int npts;
  instr >> buf ;
  string keyword=buf;

  if(keyword=="NUMPOINTS:"&&instr)
    instr >> npts;
  else
    {
      vcl_cout << "bad file " << vcl_endl;
      return false;
    }
  int view_no=0, point_id=0;
  double x=0, y=0, z=0;
  float u=0, v=0;
  for(int i=0; i<npts&&instr; i++)
    {
      instr >> buf;
      keyword = buf;
      if(keyword!="CORRESP:")
        {
          vcl_cout << "wrong number of correspondences " << vcl_endl;
          return false;
        }
      instr >> view_no >> point_id >> x >> y >> z >> u >> v;
      vcl_cout << "Corr: " <<  view_no << " " << point_id << " " << x << " " <<  y
               << " " << z << " " <<  u << " " << v << vcl_endl;
      bool stat = add_full_correspondence(view_no, point_id, x, y, z, u, v);
      if(!stat)
        return false;
    }
  _carmen->DoneDefiningProblem();
}

//-----------------------------------------------------------------------
//: solve for a perspective camera
//
void carmen_interface::solve()
{
  if(!_carmen)
    return;
  bool stat = (bool)_carmen->init_cameras();
  if(!stat)
    {
      vcl_cerr << _carmen->geterror() << vcl_endl;
      return;
    }
  stat = (bool)_carmen->solve_for_everything();
  if (!stat)
    vcl_cerr << _carmen->geterror() << vcl_endl;
}

//--------------------------------------------------------------------
//: save camera
//
void carmen_interface::save_camera(const string& file_path)
{
}

//--------------------------------------------------------------------
//: Print Carmen results
//
void carmen_interface::print_results()
{
  _carmen->print_camera_parameters(stdout);
  _carmen->print_primary_parameters(stdout);
  _carmen->print_secondary_parameters(stdout);
  _carmen->print_measurements(stdout);
}
