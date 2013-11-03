/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dump_movie.h"
#include "comm.h"
#include "error.h"
#include "force.h"
#include "memory.h"

using namespace LAMMPS_NS;

/* ---------------------------------------------------------------------- */

DumpMovie::DumpMovie(LAMMPS *lmp, int narg, char **arg) :
  DumpImage(lmp, narg, arg)
{
  if (multiproc || compressed || multifile)
    error->all(FLERR,"Invalid dump movie filename");

  filetype = PPM;
  bitrate = 2000;
  framerate = 24;
  fp = NULL;
}

/* ---------------------------------------------------------------------- */

void DumpMovie::openfile()
{
  char moviecmd[1024];

  if ((comm->me == 0) && (fp == NULL)) {
    sprintf(moviecmd,"ffmpeg -v error -y -r %d -f image2pipe -c:v ppm -i - "
            "-r 24 -b:v %dk %s ", framerate, bitrate, filename);
    fp = popen(moviecmd,"w");
  }
}
/* ---------------------------------------------------------------------- */

void DumpMovie::init_style()
{
  // initialize image style circumventing multifile check
  multifile = 1;
  DumpImage::init_style();
  multifile = 0;
}

/* ---------------------------------------------------------------------- */

int DumpMovie::modify_param(int narg, char **arg)
{
  int n = DumpImage::modify_param(narg,arg);
  if (n) return n;

  if (strcmp(arg[0],"bitrate") == 0) {
    if (narg < 2) error->all(FLERR,"Illegal dump_modify command");
    bitrate = force->inumeric(FLERR,arg[1]);
    if (bitrate <= 0.0) error->all(FLERR,"Illegal dump_modify command");
    return 2;
  }

  if (strcmp(arg[0],"framerate") == 0) {
    if (narg < 2) error->all(FLERR,"Illegal dump_modify command");
    framerate = force->inumeric(FLERR,arg[1]);
    if (framerate <= 0) error->all(FLERR,"Illegal dump_modify command");
    return 2;
  }

  return 0;
}

