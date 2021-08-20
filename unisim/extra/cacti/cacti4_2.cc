/*------------------------------------------------------------
 *                              CACTI 4.0
 *         Copyright 2005 Hewlett-Packard Development Corporation
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein, and
 * hereby grant back to Hewlett-Packard Company and its affiliated companies ("HP")
 * a non-exclusive, unrestricted, royalty-free right and license under any changes, 
 * enhancements or extensions  made to the core functions of the software, including 
 * but not limited to those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to HP any such changes,
 * enhancements or extensions that they make and inform HP of noteworthy uses of
 * this software.  Correspondence should be provided to HP at:
 *
 *                       Director of Intellectual Property Licensing
 *                       Office of Strategy and Technology
 *                       Hewlett-Packard Company
 *                       1501 Page Mill Road
 *                       Palo Alto, California  94304
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HP DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL HP 
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/

#include "cacti4_2.hh"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef MAX
#undef MAX
#endif

#define MAX(a,b) (((a)>(b))?(a):(b))

#ifndef NULL
#define NULL 0
#endif

#ifdef ERROR
#undef ERROR
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#ifdef OK
#undef OK
#endif

namespace unisim {
namespace service {
namespace power {

using std::cerr;
using std::endl;

const int TRUE = 1;
const int FALSE = 0;
const int OK = 1;
const int ERROR = 0;

const int Cacti4_2::CHUNKSIZE = 8;

const int Cacti4_2::LONG = 1;
const int Cacti4_2::SHORT = 2;

const int Cacti4_2::OUTPUTTYPE = LONG;

const int Cacti4_2::ADDRESS_BITS = 42;

const int Cacti4_2::EXTRA_TAG_BITS = 5;

const int Cacti4_2::MAXDATAN = 32;          /* Maximum for Ndwl,Ndbl */
const int Cacti4_2::MAXTAGN = 32;          /* Maximum for Ndwl,Ndbl */
const int Cacti4_2::MAXSUBARRAYS = 256;    /* Maximum subarrays for data and tag arrays */
const int Cacti4_2::MAXDATASPD = 32;         /* Maximum for Nspd */
const int Cacti4_2::MAXTAGSPD = 32;         /* Maximum for Ntspd */
const double Cacti4_2::WIREHEIGTHRATIO = 1.8;
const double Cacti4_2::Default_Cmetal = (2.9/3.9*230e-18*3.9/3.6);
const double Cacti4_2::Default_Rmetal = 48e-3;
const double Cacti4_2::Default_CopperSheetResistancePerMicroM = 32e-3;
const double Cacti4_2::CRatiolocal_to_interm = (1.0/1.4);
const double Cacti4_2::RRatiolocal_to_interm = (1.0/2.04);
const double Cacti4_2::CRatiointerm_to_global = (1.0/1.9);
const double Cacti4_2::RRatiointerm_to_global = (1.0/3.05);
const double Cacti4_2::Vdd = 5;

const double Cacti4_2::SizingRatio   = 0.33;
const double Cacti4_2::VTHNAND       = 0.561;
const double Cacti4_2::VTHFA1        = 0.452;
const double Cacti4_2::VTHFA2        = 0.304;
const double Cacti4_2::VTHFA3        = 0.420;
const double Cacti4_2::VTHFA4        = 0.413;
const double Cacti4_2::VTHFA5        = 0.405;
const double Cacti4_2::VTHFA6        = 0.452;
const double Cacti4_2::VSINV         = 0.452;
const double Cacti4_2::VTHINV100x60  = 0.438;   /* inverter with p=100,n=60 */
const double Cacti4_2::VTHINV360x240 = 0.420;   /* inverter with p=360, n=240 */
const double Cacti4_2::VTHNAND60x90  = 0.561;   /* nand with p=60 and three n=90 */
const double Cacti4_2::VTHNOR12x4x1  = 0.503;   /* nor with p=12, n=4, 1 input */
const double Cacti4_2::VTHNOR12x4x2  = 0.452;   /* nor with p=12, n=4, 2 inputs */
const double Cacti4_2::VTHNOR12x4x3  = 0.417;   /* nor with p=12, n=4, 3 inputs */
const double Cacti4_2::VTHNOR12x4x4  = 0.390;   /* nor with p=12, n=4, 4 inputs */
const double Cacti4_2::VTHOUTDRINV    = 0.437;
const double Cacti4_2::VTHOUTDRNOR   = 0.379;
const double Cacti4_2::VTHOUTDRNAND  = 0.63;
const double Cacti4_2::VTHOUTDRIVE   = 0.425;
const double Cacti4_2::VTHCOMPINV    = 0.437;
const double Cacti4_2::VTHMUXNAND    = 0.548;
const double Cacti4_2::VTHMUXDRV1    = 0.406;
const double Cacti4_2::VTHMUXDRV2    = 0.334;
const double Cacti4_2::VTHMUXDRV3    = 0.478;
const double Cacti4_2::VTHEVALINV    = 0.452;
const double Cacti4_2::VTHSENSEEXTDRV  = 0.438;

const double Cacti4_2::VTHNAND60x120 = 0.522;

const double Cacti4_2::Vbitpre = (3.3);
const double Cacti4_2::Vt = (1.09);
const double Cacti4_2::Vbitsense = (0.05*5/*Vdd*/);
const double Cacti4_2::Vbitswing = (0.20*5/*Vdd*/);
const double Cacti4_2::BIGNUM = 1e30;
const int Cacti4_2::WAVE_PIPE = 3;
const int Cacti4_2::MAX_COL_MUX = 16;
const int Cacti4_2::RISE = 1;
const int Cacti4_2::FALL = 0;
const int Cacti4_2::NCH = 1;
const int Cacti4_2::PCH = 0;
const int Cacti4_2::MAX_CACHE_ENTRIES = 512;
const double Cacti4_2::EPSILON = 0.5;
const int Cacti4_2::tracks_precharge_p = 12;
const int Cacti4_2::tracks_precharge_nx2 = 5;
const int Cacti4_2::tracks_outdrvselinv_p = 3;
const int Cacti4_2::tracks_outdrvfanand_p = 6;

const double Cacti4_2::CONVERT_TO_MMSQUARE = 1.0/1000000.0;
const double Cacti4_2::Bk = 1.38066E-23;  /* Boltzman Constant */
const double Cacti4_2::Qparam = 1.602E-19;    /* FIXME     */
const double Cacti4_2::Eox = 3.5E-11;

const int Cacti4_2::No_of_Samples = 10;
const double Cacti4_2::Tox_Std = 0;
const double Cacti4_2::Tech_Std = 0;
const double Cacti4_2::Vdd_Std = 0;
const double Cacti4_2::Vthn_Std = 0;
const double Cacti4_2::Vthp_Std = 0;


/* from basic_circuit.c */
int Cacti4_2::powers (int base, int n)
{
  int i, p;

  p = 1;
  for (i = 1; i <= n; ++i)
    p *= base;
  return p;
}

/*----------------------------------------------------------------------*/

double Cacti4_2::logtwo (double x) const
{
	/* GM: modified from original to avoid inaccuracies with integer numbers */
  if (x <= 0)
    printf ("%e\n", x);
  
  if(x <= 0 || x > floor(x))
  {
	  // x is not an integer
	  return ((double) (log (x) / log (2.0)));
  }
  
  // x is an integer (> 0)
  unsigned long long v = (unsigned long long) x;
  unsigned long long n = 0;
		
  while((v & 1) == 0)
  {
	  v >>= 1;
	  n++;
  }
  if(v != 1) // x is not a power of 2: floating point accuracy is sufficient in this case
	  return ((double) (log (x) / log (2.0)));
  else // x is a power of 2: returns the exact value
	  return (double) n;
}

/*----------------------------------------------------------------------*/

double Cacti4_2::gatecap (double width,double  wirelength)	/* returns gate capacitance in Farads */
     /* width: gate width in um (length is Leff) */
     /* wirelength: poly wire length going to gate in lambda */
{
  return (width * Leff * Cgate + wirelength * Cpolywire * Leff);
}

double Cacti4_2::gatecappass (double width,double  wirelength)	/* returns gate capacitance in Farads */
     /* width: gate width in um (length is Leff) */
     /* wirelength: poly wire length going to gate in lambda */
{
  return (width * Leff * Cgatepass + wirelength * Cpolywire * Leff);
}


/*----------------------------------------------------------------------*/

/* Routine for calculating drain capacitances.  The draincap routine
 * folds transistors larger than 10um */

double Cacti4_2::draincap (double width,int nchannel,int stack)	/* returns drain cap in Farads */
	 /* width: in um */
     /* nchannel: whether n or p-channel (boolean) */
     /* stack: number of transistors in series that are on */
{
  double Cdiffside, Cdiffarea, Coverlap, cap;

  Cdiffside = (nchannel) ? Cndiffside : Cpdiffside;
  Cdiffarea = (nchannel) ? Cndiffarea : Cpdiffarea;
  Coverlap = (nchannel) ? (Cndiffovlp + Cnoxideovlp) :
                          (Cpdiffovlp + Cpoxideovlp);
  /* calculate directly-connected (non-stacked) capacitance */
  /* then add in capacitance due to stacking */
  if(stack > 1) {
	if (width >= 10/FUDGEFACTOR) {
		cap = 3.0 * Leff * width / 2.0 * Cdiffarea + 6.0 * Leff * Cdiffside +
		width * Coverlap;
		cap += (double) (stack - 1) * (Leff * width * Cdiffarea +
						4.0 * Leff * Cdiffside +
						2.0 * width * Coverlap);
	}
	else {
		cap =
		3.0 * Leff * width * Cdiffarea + (6.0 * Leff + width) * Cdiffside +
		width * Coverlap;
		cap +=
		(double) (stack - 1) * (Leff * width * Cdiffarea +
					2.0 * Leff * Cdiffside +
					2.0 * width * Coverlap);
	}
  }
  else {
	  if (width >= 10/FUDGEFACTOR) {
		cap = 3.0 * Leff * width / 2.0 * Cdiffarea + 6.0 * Leff * Cdiffside +
		width * Coverlap;
	}
	else {
		cap = 3.0 * Leff * width * Cdiffarea + (6.0 * Leff + width) * Cdiffside +
		width * Coverlap;
	}
  }
  return (cap);
}

/*----------------------------------------------------------------------*/

/* The following routines estimate the effective resistance of an
   on transistor as described in the tech report.  The first routine
   gives the "switching" resistance, and the second gives the 
   "full-on" resistance */

double Cacti4_2::transresswitch (double width,int nchannel,int stack)	/* returns resistance in ohms */
     /* width: in um */
     /* nchannel: whether n or p-channel (boolean) */
     /* stack: number of transistors in series */
{
  double restrans;
  restrans = (nchannel) ? (Rnchannelstatic) : (Rpchannelstatic);
  /* calculate resistance of stack - assume all but switching trans
     have 0.8X the resistance since they are on throughout switching */
  return ((1.0 + ((stack - 1.0) * 0.8)) * restrans / width);
}

/*----------------------------------------------------------------------*/

double Cacti4_2::transreson (double width,int nchannel,int stack)	/* returns resistance in ohms */
     /* width: in um */
     /* nchannel: whether n or p-channel (boolean) */
     /* stack: number of transistors in series */
{
  double restrans;
  restrans = (nchannel) ? Rnchannelon : Rpchannelon;

  /* calculate resistance of stack.  Unlike transres, we don't
     multiply the stacked transistors by 0.8 */
  return (stack * restrans / width);

}

/*----------------------------------------------------------------------*/

/* This routine operates in reverse: given a resistance, it finds
 * the transistor width that would have this R.  It is used in the
 * data wordline to estimate the wordline driver size. */

double Cacti4_2::restowidth (double res,int nchannel)	/* returns width in um */
     /* res: resistance in ohms */
     /* nchannel: whether N-channel or P-channel */
{
  double restrans;

  restrans = (nchannel) ? Rnchannelon : Rpchannelon;

  return (restrans / res);

}

/*----------------------------------------------------------------------*/

double Cacti4_2::horowitz (double inputramptime,double  tf,double  vs1,double  vs2,int rise)
	/* inputramptime: input rise time */
    /* tf: time constant of gate */
    /* vs1, vs2: threshold voltages */
    /* rise: whether INPUT rise or fall (boolean) */
{
  double a, b, td;

  a = inputramptime / tf;
  if (rise == RISE)
    {
      b = 0.5;
      td = tf * sqrt (log (vs1) * log (vs1) + 2 * a * b * (1.0 - vs1)) +
	tf * (log (vs1) - log (vs2));
    }
  else
    {
      b = 0.4;
      td = tf * sqrt (log (1.0 - vs1) * log (1.0 - vs1) + 2 * a * b * (vs1)) +
	tf * (log (1.0 - vs1) - log (1.0 - vs2));
    }
  return (td);
}

/* from area.c */
//v4.1: Earlier all the dimensions (length/width/thickness) of the transistors and wires
//were calculated for the 0.8 micron process and then scaled to the input techology. Now
//all dimensions are calculated directly for the input technology, so area no longer needs to 
//be scaled to the input technology.

//double
//calculate_area (Cacti4_2::area_type module_area,double techscaling_factor)
//{
  //return (module_area.height * module_area.width * (1 / techscaling_factor) *
	  //(1 / techscaling_factor));
//}

Cacti4_2::area_type
Cacti4_2::inverter_area (double Widthp,double Widthn)
{
  double Width_n, Width_p;
  Cacti4_2::area_type invarea;
  int foldp = 0, foldn = 0;
  if (Widthp > 10.0 / FUDGEFACTOR)
    {
      Widthp = Widthp / 2, foldp = 1;
    }
  if (Widthn > 10.0 / FUDGEFACTOR)
    {
      Widthn = Widthn / 2, foldn = 1;
    }
  invarea.height = Widthp + Widthn + Widthptondiff + 2 * Widthtrack;
  Width_n =
    (foldn) ? (3 * Widthcontact +
	       2 * (Wpoly + 2 * ptocontact)) : (2 * Widthcontact + Wpoly +
						2 * ptocontact);
  Width_p =
    (foldp) ? (3 * Widthcontact +
	       2 * (Wpoly + 2 * ptocontact)) : (2 * Widthcontact + Wpoly +
						2 * ptocontact);
  invarea.width = MAX (Width_n, Width_p);
  return (invarea);
}

Cacti4_2::area_type
Cacti4_2::subarraymem_area (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int RWP,int ERP,int EWP,int NSER,double techscaling_factor)	/* returns area of subarray */
{
  Cacti4_2::area_type memarea;
  int noof_rows, noof_colns;

  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  //noof_rows = (C / (B * A * Ndbl * Nspd));
  //noof_colns = (8 * B * A * Nspd / Ndwl);
  noof_rows = (int)((C / (B * A * Ndbl * Nspd)) + EPSILON);
  noof_colns = (int)((8 * B * A * Nspd / Ndwl) + EPSILON);

  memarea.height = ceil((double)(noof_rows)/16.0)*stitch_ramv+(BitHeight1x1+Widthtrack*2*(RWP+ERP+EWP-1))*noof_rows;
  memarea.width  = noof_colns*(BitWidth1x1+(Widthtrack*2*(RWP+(ERP-NSER)+EWP-1)+Widthtrack*NSER));
	/* dt
	was : memarea.height = ceil((double)(noof_rows)/16.0)*stitch_ramv+(BitHeight16x2+2*Widthtrack*2*(RWP+ERP+EWP-1))*ceil((double)(noof_rows)/2.0);
		  memarea.width  = ceil((double)(noof_colns)/16.0)*(BitWidth16x2+16*(Widthtrack*2*(RWP+(ERP-NSER)+EWP-1)+Widthtrack*NSER));
	now : use single cell for width and height
	*/

  //area_all_dataramcells =
    //Ndwl * Ndbl * calculate_area (memarea, techscaling_factor) * CONVERT_TO_MMSQUARE;
  area_all_dataramcells = Ndwl * Ndbl * memarea.height * memarea.width * CONVERT_TO_MMSQUARE;
  return (memarea);
}

Cacti4_2::area_type
Cacti4_2::decodemem_row (int C,int B,int A,int Ndbl,double Nspd,int Ndwl,int RWP,int ERP,int EWP)	/* returns area of post decode */
{
  int noof_colns, numstack;
  double decodeNORwidth;
  double desiredrisetime, Cline, Rpdrive, psize, nsize;
  Cacti4_2::area_type decinv, worddriveinv, postdecodearea;

  /*
  How many bit columns do we have in each subarray?
  Since our basic unit is the byte, we have 8 bits to the byte.
  Each Block is made up of B bytes -> 8*B
  If we have associativity A, then all the ways are mapped to one wordline -> 8*B*A
  If we mapped more than one set to each wordline (if Nspd > 1) than the wordline is longer again -> 8*B*A*Nspd
  If we have subdivided the global wordline into segments (if Ndwl > 1) than the local wordline is shorter -> 8*B*A*Nspd/Ndwl
  */
  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  //noof_colns = 8 * B * A * Nspd / Ndwl;
  noof_colns = (int) (8 * B * A * Nspd / Ndwl + EPSILON);
  desiredrisetime = krise * log ((double) (noof_colns)) / 2.0;
  Cline = (2 * Wmemcella * Leff * Cgatepass + Cwordmetal) * noof_colns;
  Rpdrive = desiredrisetime / (Cline * log (VSINV) * -1.0);
  psize = Rpchannelon / Rpdrive;
  if (psize > Wworddrivemax)
    {
      psize = Wworddrivemax;
    }
  numstack =
    (int)ceil ((1.0 / 3.0) * logtwo ((double)
		       ((double) C / (double) (B * A * Ndbl * Nspd))));
  if (numstack == 0)
    numstack = 1;
  if (numstack > 5)
    numstack = 5;
  switch (numstack)
    {
    case 1:
      decodeNORwidth = WidthNOR1;
      break;
    case 2:
      decodeNORwidth = WidthNOR2;
      break;
    case 3:
      decodeNORwidth = WidthNOR3;
      break;
    case 4:
      decodeNORwidth = WidthNOR4;
      break;
    case 5:
      decodeNORwidth = WidthNOR4;
      break;
    default:
      printf ("error:numstack=%d\n", numstack);
      printf ("Cacti does not support a series stack of %d transistors !\n",
	      numstack);
      exit(0);
      break;

    }
  nsize = psize * Wdecinvn / Wdecinvp;
  decinv = inverter_area (Wdecinvp, Wdecinvn);
  worddriveinv = inverter_area (psize, nsize);
  /*
	was: postdecodearea.height = (BitHeight16x2+2*Widthtrack*2*(RWP+ERP+EWP-1));
  */
  postdecodearea.height = (2*BitHeight1x1+2*Widthtrack*2*(RWP+ERP+EWP-1));
  postdecodearea.width =
    (decodeNORwidth + decinv.height + worddriveinv.height) * (RWP + ERP +
							      EWP);
  return (postdecodearea);
}

//v4.1: Making noof_rows double since the variable colns_datasubarray is function area
//was made double and is used as an argument corresponding to noof_rows in function calls 
//to predecode_area
//Cacti4_2::area_type
//predecode_area (int noof_rows,int RWP,int ERP,int EWP)	/*returns the area of predecode */
Cacti4_2::area_type
Cacti4_2::predecode_area (double noof_rows,int RWP,int ERP,int EWP)
/* this puts the different predecode blocks for the different ports side by side and does not put them as an array or something */

{
  Cacti4_2::area_type predecode, predecode_temp;
  int N3to8;
  //v4.1: noof_rows can be less than 1 now since because Nspd can be a fraction. When
  //noof_rows is less than 1 making N3to8 1. This is not clean and needs to be fixed later. 
  if(noof_rows < 1) 
	N3to8 = 1;
  else 
	//v4.1: using integer casting below 
	//N3to8 = ceil ((1.0 / 3.0) * logtwo ((double) (noof_rows)));
      N3to8 = (int) (ceil ((1.0 / 3.0) * logtwo ((double) (noof_rows))));
  if (N3to8 == 0)
    {
      N3to8 = 1;
    }

  switch (N3to8)
    {
    case 1:
      predecode_temp.height = Predec_height1;
      predecode_temp.width = Predec_width1;
      break;
    case 2:
      predecode_temp.height = Predec_height2;
      predecode_temp.width = Predec_width2;
      break;
    case 3:
      predecode_temp.height = Predec_height3;
      predecode_temp.width = Predec_width3;
      break;
    case 4:
      predecode_temp.height = Predec_height4;
      predecode_temp.width = Predec_width4;
      break;
    case 5:
      predecode_temp.height = Predec_height5;
      predecode_temp.width = Predec_width5;
      break;
    case 6:
      predecode_temp.height = Predec_height6;
      predecode_temp.width = Predec_width6;
      break;
    default:
      printf ("error:N3to8=%d\n", N3to8);
      exit (0);

    }

  predecode.height = predecode_temp.height;
  predecode.width = predecode_temp.width * (RWP + ERP + EWP);
  return (predecode);
}

//v4.1: Making noof_rows double since the variable colns_datasubarray is function area
//was made double and is used as an argument corresponding to noof_rows in function calls 
//to postdecode_area
Cacti4_2::area_type
Cacti4_2::postdecode_area (int noof_rows,int RWP,int ERP,int EWP)
{
  //v4.1:	Making decodeNORwidth double which is what it should be
  //int numstack, decodeNORwidth;
  int numstack;
  double decodeNORwidth;
  Cacti4_2::area_type postdecode, decinverter;
  decinverter = inverter_area (Wdecinvp, Wdecinvn);
  //v4.1: noof_rows can be less than 1 now since because Nspd can be a fraction. When
  //noof_rows is less than 1 making N3to8 1. This is not clean and needs to be fixed later. 
  if(noof_rows < 1) 
	numstack = 1;
  else
    //v4.1: using integer casting below 
	//numstack = ceil ((1.0 / 3.0) * logtwo ((double) (noof_rows)));
    numstack = (int) (ceil ((1.0 / 3.0) * logtwo ((double) (noof_rows))));
  if (numstack == 0)
    numstack = 1;
  if (numstack > 5)
    numstack = 5;
  switch (numstack)
    {
    case 1:
      decodeNORwidth = WidthNOR1;
      break;
    case 2:
      decodeNORwidth = WidthNOR2;
      break;
    case 3:
      decodeNORwidth = WidthNOR3;
      break;
    case 4:
      decodeNORwidth = WidthNOR4;
      break;
    case 5:
      decodeNORwidth = WidthNOR4;
      break;
    default:
      printf ("error:numstack=%d\n", numstack);
      printf ("Cacti does not support a series stack of %d transistors !\n",
	      numstack);
      exit (0);
      break;

    }
  postdecode.height =
    (BitHeight + Widthtrack * 2 * (RWP + ERP + EWP - 1)) * noof_rows;
  postdecode.width =
    (2 * decinverter.height + decodeNORwidth) * (RWP + ERP + EWP);
  return (postdecode);
}

Cacti4_2::area_type
Cacti4_2::colmux (int Ndbl,double Nspd,int RWP,int ERP,int EWP,int NSER)	/* gives the height of the colmux */
{
  Cacti4_2::area_type colmux_area;
  colmux_area.height =
    (2 * Wiso + 3 * (2 * Widthcontact + 1 / FUDGEFACTOR)) * (RWP + ERP + EWP);//Shyam: Need to understand what the +1 is for
  colmux_area.width =
    (BitWidth + Widthtrack * 2 * (RWP + (ERP - NSER) + EWP - 1) +
     Widthtrack * NSER);
  return (colmux_area);
}

Cacti4_2::area_type
Cacti4_2::precharge (int Ndbl,double Nspd,int RWP,int ERP,int EWP,int NSER)
{
  Cacti4_2::area_type precharge_area;
  if (Ndbl * Nspd > 1)
    {
      precharge_area.height =
	(Wbitpreequ + 2 * Wbitdropv + Wwrite + 2 * (2 * Widthcontact + 1 / FUDGEFACTOR) +
	 3 * Widthptondiff) * 0.5 * (RWP + EWP);
      precharge_area.width =
	2 * (BitWidth + Widthtrack * 2 * (RWP + (ERP - NSER) + EWP - 1) +
	     Widthtrack * NSER);
    }
  else
    {
      precharge_area.height =
	(Wbitpreequ + 2 * Wbitdropv + Wwrite + 2 * (2 * Widthcontact + 1 / FUDGEFACTOR) +
	 3 * Widthptondiff) * (RWP + EWP);
      precharge_area.width =
	BitWidth + Widthtrack * 2 * (RWP + (ERP - NSER) + EWP - 1) +
	Widthtrack * NSER;
    }
  return (precharge_area);
}

Cacti4_2::area_type
Cacti4_2::senseamp (int Ndbl,double Nspd,int RWP,int ERP,int EWP,int NSER)
{
  Cacti4_2::area_type senseamp_area;
  if (Ndbl * Nspd > 1)
    {
      senseamp_area.height = 0.5 * SenseampHeight * (RWP + ERP);
      senseamp_area.width =
	2 * (BitWidth + Widthtrack * 2 * (RWP + (ERP - NSER) + EWP - 1) +
	     Widthtrack * NSER);
    }
  else
    {
      senseamp_area.height = SenseampHeight * (RWP + ERP);
      senseamp_area.width =
	BitWidth + Widthtrack * 2 * (RWP + (ERP - NSER) + EWP - 1) +
	Widthtrack * NSER;
    }
  return (senseamp_area);
}

/* define OutdriveHeight OutdriveWidth DatainvHeight DatainvWidth */

Cacti4_2::area_type
Cacti4_2::subarraytag_area (int baddr,int C,int B,int A,int Ntdbl,int Ntdwl,int Ntspd,double NSubbanks,int RWP,int ERP,
		  int EWP,int NSER,double techscaling_factor)	/* returns area of subarray */
{
  Cacti4_2::area_type tagarea;
  int noof_rows, noof_colns, Tagbits;
  int conservative_NSER;

  conservative_NSER = 0;

  //Added by Shyam to make area model sensitive to "change tag" feature

  if(!force_tag) {
	    //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//Tagbits = ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		Tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
  }
  else {
	    Tagbits = force_tag_size;
	}
 
  //Commented by Shyam Tagbits =
    //baddr - (int) (logtwo ((double) (C))) +
    //(int) (logtwo ((double) (A))) + 2 - (int) (logtwo (NSubbanks));

  noof_rows = (C / (B * A * Ntdbl * Ntspd));
  noof_colns = (Tagbits * A * Ntspd / Ntdwl);
  tagarea.height = ceil((double)(noof_rows)/16.0)*stitch_ramv+(BitHeight1x1+Widthtrack*2*(RWP+ERP+EWP-1))*noof_rows;
  tagarea.width  = noof_colns*(BitWidth1x1+(Widthtrack*2*(RWP+(ERP-conservative_NSER)+EWP-1)+Widthtrack*conservative_NSER));
  /*
  was:
  tagarea.height = ceil((double)(noof_rows)/16.0)*stitch_ramv+(BitHeight16x2+2*Widthtrack*2*(RWP+ERP+EWP-1))*ceil((double)(noof_rows)/2.0);
  tagarea.width = ceil((double)(noof_colns)/16.0)*(BitWidth16x2+16*(Widthtrack*2*(RWP+(ERP-conservative_NSER)+EWP-1)+Widthtrack*conservative_NSER));
  now:
  using single cell for width and height
  */

  //area_all_tagramcells =
    //Ntdwl * Ntdbl * calculate_area (tagarea,
				    //techscaling_factor) * CONVERT_TO_MMSQUARE;
  area_all_tagramcells =  Ntdwl * Ntdbl * tagarea.height *tagarea.width * CONVERT_TO_MMSQUARE;
  return (tagarea);
}

Cacti4_2::area_type
Cacti4_2::decodetag_row (int baddr,int C,int B,int A,int Ntdbl,int Ntspd,int Ntdwl,double NSubbanks,int RWP,int ERP,int EWP)	/* returns area of post decode */
{
  int numstack, Tagbits;
  double decodeNORwidth;
  Cacti4_2::area_type decinv, worddriveinv, postdecodearea;

  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  //Tagbits =
    //baddr - logtwo ((double) (C)) + logtwo ((double) (A)) + 2 -
    //logtwo ((double) (NSubbanks));
  Tagbits = (int) (baddr - logtwo ((double) (C)) + logtwo ((double) (A)) + 2 -
    logtwo ((double) (NSubbanks)) + EPSILON);

  //v4.1: using integer casting below 
  //numstack =
    //ceil ((1.0 / 3.0) *
	  //logtwo ((double)
		       //((double) C / (double) (B * A * Ntdbl * Ntspd))));
  numstack =
    (int) (ceil ((1.0 / 3.0) *
	  logtwo ((double)
		       ((double) C / (double) (B * A * Ntdbl * Ntspd)))));
  if (numstack == 0)
    numstack = 1;
  if (numstack > 5)
    numstack = 5;
  switch (numstack)
    {
    case 1:
      decodeNORwidth = WidthNOR1;
      break;
    case 2:
      decodeNORwidth = WidthNOR2;
      break;
    case 3:
      decodeNORwidth = WidthNOR3;
      break;
    case 4:
      decodeNORwidth = WidthNOR4;
      break;
    case 5:
      decodeNORwidth = WidthNOR4;
      break;
    default:
      printf ("error:numstack=%d\n", numstack);
      printf ("Cacti does not support a series stack of %d transistors !\n",
	      numstack);
      exit (0);
      break;

    }
  decinv = inverter_area (Wdecinvp, Wdecinvn);
  worddriveinv = inverter_area (Wdecinvp, Wdecinvn);
  /*
  postdecodearea.height = (BitHeight16x2+2*Widthtrack*2*(RWP+ERP+EWP-1));
  */
  postdecodearea.height = (2*BitHeight1x1+2*Widthtrack*2*(RWP+ERP+EWP-1));
  postdecodearea.width =
    (decodeNORwidth + decinv.height + worddriveinv.height) * (RWP + ERP +
							      EWP);
  return (postdecodearea);
}

Cacti4_2::area_type
Cacti4_2::comparatorbit (int RWP,int ERP,int EWP)
{
  Cacti4_2::area_type compbit_area;
  compbit_area.width = 3 * Widthcontact + 2 * (3 * Wpoly + 2 * ptocontact);
  compbit_area.height = (Wcompn + 2 * (2 * Widthcontact + 1 / FUDGEFACTOR)) * (RWP + ERP);
  return (compbit_area);
}

Cacti4_2::area_type
Cacti4_2::muxdriverdecode (int B,int b0,int RWP,int ERP,int EWP)
{
  int noof_rows;
  Cacti4_2::area_type muxdrvdecode_area, predecode, postdecode;
  noof_rows = (8 * B) / b0;
  predecode = predecode_area (noof_rows, RWP, ERP, EWP);
  postdecode = postdecode_area (noof_rows, RWP, ERP, EWP);
  muxdrvdecode_area.width =
    predecode.height + postdecode.width + noof_rows * Widthtrack * (RWP +
								    ERP +
								    EWP);
  muxdrvdecode_area.height = MAX (predecode.width, postdecode.height);
  return (muxdrvdecode_area);
}

Cacti4_2::area_type
Cacti4_2::muxdrvsig (int A,int B,int b0)		/* generates the 8B/b0*A signals */
{
  int noof_rows;
  Cacti4_2::area_type outdrvsig_area;
  Cacti4_2::area_type muxdrvsig_area;
  noof_rows = (8 * B) / b0;
  //debug
  muxdrvsig_area.height = 0;
  muxdrvsig_area.width = 0;

  outdrvsig_area.height =
    0.5 * (WmuxdrvNORn + WmuxdrvNORp) + 9 * Widthcontact + 0.5 * (Wmuxdrv3n +
								  Wmuxdrv3p) +
    Widthptondiff + 3 * Widthcontact; 
  outdrvsig_area.width =
    (3 * Widthcontact + 2 * (3 * Wpoly + 2 * ptocontact)) * noof_rows;
  switch (A)
    {
    case 1:
      muxdrvsig_area.height =
	outdrvsig_area.height + noof_rows * Widthtrack * 2 + A * Widthtrack;
      muxdrvsig_area.width =
	outdrvsig_area.width + noof_rows * Widthtrack + A * Widthtrack;
      break;
    case 2:
      muxdrvsig_area.height =
	outdrvsig_area.height * 2 + noof_rows * Widthtrack * 3 +
	A * Widthtrack;
      muxdrvsig_area.width =
	outdrvsig_area.width + noof_rows * Widthtrack + A * Widthtrack;
      break;
    case 4:
      muxdrvsig_area.height =
	outdrvsig_area.height * 2 + noof_rows * Widthtrack * 5 +
	A * Widthtrack;
      muxdrvsig_area.width =
	outdrvsig_area.width * 2 + noof_rows * Widthtrack + A * Widthtrack;
      break;
    case 8:
      muxdrvsig_area.height =
	outdrvsig_area.height * 2 + noof_rows * Widthtrack * 9 +
	A * Widthtrack;
      muxdrvsig_area.width =
	outdrvsig_area.width * 4 + noof_rows * Widthtrack + A * Widthtrack;
      break;
    case 16:
      muxdrvsig_area.height =
	outdrvsig_area.height * 4 + noof_rows * Widthtrack * 18 +
	A * Widthtrack;
      muxdrvsig_area.width =
	outdrvsig_area.width * 4 + noof_rows * Widthtrack + A * Widthtrack;
      break;
    case 32:
      muxdrvsig_area.height =
	outdrvsig_area.height * 4 + noof_rows * Widthtrack * 35 +
	2 * A * Widthtrack;
      muxdrvsig_area.width =
	2 * (outdrvsig_area.width * 4 + noof_rows * Widthtrack +
	     A * Widthtrack);
      break;
    default:
      printf ("error:Associativity=%d\n", A);
    }

  return (muxdrvsig_area);
}


Cacti4_2::area_type
Cacti4_2::datasubarray (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int RWP,int ERP,int EWP,int NSER,
	      double techscaling_factor)
{

  //Cacti4_2::area_type datasubarray_area, mem_area, postdecode_area, colmux_area,
    //precharge_area, senseamp_area, outdrv_area;
  Cacti4_2::area_type datasubarray_area, mem_area, postdecode_area, colmux_area,
    precharge_area, senseamp_area;
  mem_area =
    subarraymem_area (C, B, A, Ndbl, Ndwl, Nspd, RWP, ERP, EWP, NSER,
		      techscaling_factor);
  postdecode_area = decodemem_row (C, B, A, Ndbl, Nspd, Ndwl, RWP, ERP, EWP);
  colmux_area = colmux (Ndbl, Nspd, RWP, ERP, EWP, NSER);
  precharge_area = precharge (Ndbl, Nspd, RWP, ERP, EWP, NSER);
  senseamp_area = senseamp (Ndbl, Nspd, RWP, ERP, EWP, NSER);
  datasubarray_area.height =
    mem_area.height + colmux_area.height + precharge_area.height +
    senseamp_area.height + DatainvHeight * (RWP + EWP) +
    OutdriveHeight * (RWP + ERP);
  datasubarray_area.width = mem_area.width + postdecode_area.width;

  return (datasubarray_area);
}

Cacti4_2::area_type
Cacti4_2::datasubblock (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int SB,int b0,int RWP,int ERP,int EWP,int NSER,
	      double techscaling_factor)
{
  int N3to8;
  int colmuxtracks_rem, outrdrvtracks_rem, writeseltracks_rem;
  int SB_;
  double tracks_h, tracks_w;
  Cacti4_2::area_type datasubarray_area, datasubblock_area;
  SB_ = SB;
  if (SB_ == 0)
    {
      SB_ = 1;
    }
  colmuxtracks_rem =
    (Ndbl * Nspd >
     tracks_precharge_p) ? (Ndbl * Nspd - tracks_precharge_p) : 0;
  outrdrvtracks_rem =
    ((2 * B * A) / (b0) >
     tracks_outdrvselinv_p) ? ((2 * B * A) / (b0) -
			       tracks_outdrvselinv_p) : 0;
  writeseltracks_rem =
    ((2 * B * A) / (b0) >
     tracks_precharge_nx2) ? ((2 * B * A) / (b0) - tracks_precharge_nx2) : 0;
 //v4.1: using integer casting below 
  //N3to8 =
    //ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd))));
 N3to8 =
    (int) (ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd)))));
  if (N3to8 == 0)
    {
      N3to8 = 1;
    }

  tracks_h =
    Widthtrack * (N3to8 * 8 * (RWP + ERP + EWP) +
		  (RWP + EWP) * colmuxtracks_rem + Ndbl * Nspd * ERP +
		  4 * outrdrvtracks_rem * (RWP + ERP) +
		  4 * writeseltracks_rem * (RWP + EWP) + (RWP + ERP +
							  EWP) * b0 / SB_);
  tracks_w = Widthtrack * (N3to8 * 8) * (RWP + ERP + EWP);
  datasubarray_area =
    datasubarray (C, B, A, Ndbl, Ndwl, Nspd, RWP, ERP, EWP, NSER,
		  techscaling_factor);
  datasubblock_area.height = 2 * datasubarray_area.height + tracks_h;
  datasubblock_area.width = 2 * datasubarray_area.width + tracks_w;

  return (datasubblock_area);
}

Cacti4_2::area_type
Cacti4_2::dataarray (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int b0,int RWP,int ERP,int EWP,int NSER,
	   double techscaling_factor)
{
  int SB, N3to8;
  Cacti4_2::area_type dataarray_area, datasubarray_area, datasubblock_area;
  Cacti4_2::area_type temp;
  double temp_aspect;
  double fixed_tracks_internal, fixed_tracks_external, variable_tracks;
  double data, driver_select, colmux, predecode, addresslines;
  int blocks, htree, htree_half, i, multiplier, iter_height;
  double inter_height, inter_width, total_height, total_width;

  SB = Ndwl * Ndbl / 4;
  //v4.1: using integer casting below 
  //N3to8 =
    //ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd))));
  N3to8 =
    (int) (ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd)))));
  if (N3to8 == 0)
    {
      N3to8 = 1;
    }

  data = b0 * (RWP + ERP + EWP) * Widthtrack;
  driver_select = (2 * RWP + ERP + EWP) * 8 * B * A / b0 * Widthtrack;
  colmux = Ndbl * Nspd * (RWP + EWP + ERP) * Widthtrack;
  predecode = (RWP + ERP + EWP) * N3to8 * 8 * Widthtrack;
  addresslines = ADDRESS_BITS * (RWP + ERP + EWP) * Widthtrack;

  fixed_tracks_internal = colmux + predecode + driver_select;
  fixed_tracks_external = colmux + driver_select + addresslines;
  variable_tracks = data;

  datasubarray_area =
    datasubarray (C, B, A, Ndbl, Ndwl, Nspd, RWP, ERP, EWP, NSER,
		  techscaling_factor);
  datasubblock_area =
    datasubblock (C, B, A, Ndbl, Ndwl, Nspd, SB, b0, RWP, ERP, EWP, NSER,
		  techscaling_factor);
  //area_all_datasubarrays =
    //Ndbl * Ndwl * calculate_area (datasubarray_area,
				  //techscaling_factor) * CONVERT_TO_MMSQUARE;
  area_all_datasubarrays = Ndbl * Ndwl * datasubarray_area.height * datasubarray_area.width * CONVERT_TO_MMSQUARE;


  if (SB == 0)
    {
      if (Ndbl * Ndwl == 1)
	{
	  total_height =
	    datasubarray_area.height + fixed_tracks_external + data;
	  total_width = datasubarray_area.width + predecode;
	}
      else
	{
	  total_height =
	    2 * datasubarray_area.height + fixed_tracks_external + data;
	  total_width = datasubarray_area.width + predecode;
	}
    }
  else if (SB == 1)
    {
      total_height = datasubblock_area.height;
      total_width = datasubblock_area.width;
    }
  else if (SB == 2)
    {
      total_height = datasubblock_area.height;
      total_width =
	2 * datasubblock_area.width + fixed_tracks_external + data;
    }
  else if (SB == 4)
    {
      total_height =
	2 * datasubblock_area.height + fixed_tracks_external + data;
      total_width =
	2 * datasubblock_area.width + fixed_tracks_internal +
	variable_tracks / 2;
    }
  else if (SB == 8)
    {
      total_height =
	2 * datasubblock_area.height + fixed_tracks_internal +
	variable_tracks / 2;
      total_width =
	2 * (2 * datasubblock_area.width + variable_tracks / 4) +
	fixed_tracks_external + data;
    }

  else if (SB > 8)
    {
      blocks = SB / 4;
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
      //htree = (int) (logtwo ((double) (blocks)));
	  htree = (int) (logtwo ((double) (blocks)) + EPSILON);
      inter_height = datasubblock_area.height;
      inter_width = datasubblock_area.width;
      multiplier = 1;

      if (htree % 2 == 0)
	{
	  iter_height = htree / 2;
	}

      if (htree % 2 == 0)
	{
	  for (i = 0; i <= iter_height; i++)
	    {
	      if (i == iter_height)
		{
		  total_height =
		    2 * inter_height + data / blocks * multiplier +
		    fixed_tracks_external;
		  total_width =
		    2 * inter_width + data / (2 * blocks) * multiplier +
		    fixed_tracks_internal;}
	      else
		{
		  total_height =
		    2 * inter_height + data / blocks * multiplier +
		    fixed_tracks_internal;
		  total_width =
		    2 * inter_width + data / (2 * blocks) * multiplier +
		    fixed_tracks_internal;
		  inter_height = total_height;
		  inter_width = total_width;
		  multiplier = multiplier * 4;
		}
	    }
	}
      else
	{
	  htree_half = htree - 1;
	  iter_height = htree_half / 2;
	  for (i = 0; i <= iter_height; i++)
	    {
	      total_height =
		2 * inter_height + data / blocks * multiplier +
		fixed_tracks_internal;
	      total_width =
		2 * inter_width + data / (2 * blocks) * multiplier +
		fixed_tracks_internal;
	      inter_height = total_height;
	      inter_width = total_width;
	      multiplier = multiplier * 4;
	    }
	  total_width =
	    2 * inter_width + data / (2 * blocks) * multiplier +
	    fixed_tracks_external;
	}
    }

  dataarray_area.width = total_width;
  dataarray_area.height = total_height;

  temp.height = dataarray_area.width;
  temp.width = dataarray_area.height;

  temp_aspect =
    ((temp.height / temp.width) >
     1.0) ? (temp.height / temp.width) : 1.0 / (temp.height / temp.width);
  aspect_ratio_data =
    ((dataarray_area.height / dataarray_area.width) >
     1.0) ? (dataarray_area.height / dataarray_area.width) : 1.0 /
    (dataarray_area.height / dataarray_area.width);
  if (aspect_ratio_data > temp_aspect)
    {
      dataarray_area.height = temp.height;
      dataarray_area.width = temp.width;
    }

  aspect_ratio_data =
    ((dataarray_area.height / dataarray_area.width) >
     1.0) ? (dataarray_area.height / dataarray_area.width) : 1.0 /
    (dataarray_area.height / dataarray_area.width);

  return (dataarray_area);
}

Cacti4_2::area_type
Cacti4_2::tagsubarray (int baddr,int C,int B,int A,int Ndbl,int Ndwl,double Nspd,double NSubbanks,int RWP,int ERP,int EWP,int NSER,
	     double techscaling_factor)
{
  int conservative_NSER;
  Cacti4_2::area_type tagsubarray_area, tag_area, postdecode_area, colmux_area,
    precharge_area, senseamp_area, comp_area;

  conservative_NSER = 0;
  tag_area =
    subarraytag_area (baddr, C, B, A, Ndbl, Ndwl, Nspd, NSubbanks, RWP, ERP,
		      EWP, conservative_NSER, techscaling_factor);
  postdecode_area =
    decodetag_row (baddr, C, B, A, Ndbl, Nspd, Ndwl, NSubbanks, RWP, ERP,
		   EWP);
  colmux_area = colmux (Ndbl, Nspd, RWP, ERP, EWP, conservative_NSER);
  precharge_area = precharge (Ndbl, Nspd, RWP, ERP, EWP, conservative_NSER);
  senseamp_area = senseamp (Ndbl, Nspd, RWP, ERP, EWP, conservative_NSER);
  comp_area = comparatorbit (RWP, ERP, EWP);
  tagsubarray_area.height =
    tag_area.height + colmux_area.height + precharge_area.height +
    senseamp_area.height + comp_area.height;
  tagsubarray_area.width = tag_area.width + postdecode_area.width;

  return (tagsubarray_area);
}

Cacti4_2::area_type
Cacti4_2::tagsubblock (int baddr,int C,int B,int A,int Ndbl,int Ndwl,double Nspd,double NSubbanks,int SB,int RWP,int ERP,int EWP,
	     int NSER,double techscaling_factor)
{
  int N3to8, T;
  int SB_;
  //int colmuxtracks_rem, writeseltracks_rem;
  int colmuxtracks_rem;
  double tracks_h, tracks_w;
  Cacti4_2::area_type tagsubarray_area, tagsubblock_area;
  int conservative_NSER;

  conservative_NSER = 0;
  SB_ = SB;
  if (SB_ == 0)
    {
      SB_ = 1;
    }
  //v4.1: using integer casting below 
  //N3to8 =
    //ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd))));
  N3to8 =
    (int) (ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd)))));
  if (N3to8 == 0)
    {
      N3to8 = 1;
    }

  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  //T =
    //baddr - logtwo ((double) (C)) + logtwo ((double) (A)) + 2 -
    //logtwo ((double) (NSubbanks));
  T =
    (int) (baddr - logtwo ((double) (C)) + logtwo ((double) (A)) + 2 -
    logtwo ((double) (NSubbanks)) + EPSILON);

  colmuxtracks_rem =
    (Ndbl * Nspd >
     tracks_precharge_p) ? (Ndbl * Nspd - tracks_precharge_p) : 0;
  /*writeseltracks_rem = ((2*B*A)/(b0) > tracks_precharge_nx2) ? ((2*B*A)/(b0)-tracks_precharge_nx2) : 0; */

  tracks_h =
    Widthtrack * (N3to8 * 8 * (RWP + ERP + EWP) +
		  (RWP + EWP) * colmuxtracks_rem + Ndbl * Nspd * ERP + (RWP +
									ERP +
									EWP) *
		  T / SB_ + (ERP + RWP) * A);
  tracks_w = Widthtrack * (N3to8 * 8) * (RWP + ERP + EWP);
  tagsubarray_area =
    tagsubarray (baddr, C, B, A, Ndbl, Ndwl, Nspd, NSubbanks, RWP, ERP, EWP,
		 conservative_NSER, techscaling_factor);
  tagsubblock_area.height = 2 * tagsubarray_area.height + tracks_h;
  tagsubblock_area.width = 2 * tagsubarray_area.width + tracks_w;

  return (tagsubblock_area);
}

Cacti4_2::area_type
Cacti4_2::tagarray (int baddr,int C,int B,int A,int Ndbl,int Ndwl,double Nspd,double NSubbanks,int RWP,int ERP,int EWP,int NSER,
	  double techscaling_factor)
{
  //int SB, CSB, N3to8, T;
  int SB, N3to8, T;
  Cacti4_2::area_type tagarray_area, tagsubarray_area, tagsubblock_area;
  Cacti4_2::area_type temp;
  double temp_aspect;
  int conservative_NSER;

  double fixed_tracks_internal, fixed_tracks_external, variable_tracks;
  double tag, assoc, colmux, predecode, addresslines;
  int blocks, htree, htree_half, i, multiplier, iter_height;
  double inter_height, inter_width, total_height, total_width;

  conservative_NSER = 0;
  SB = Ndwl * Ndbl / 4;
  
  //v4.1: using integer casting below 
  //N3to8 =
    //ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd))));
  N3to8 =
    (int) (ceil ((1.0 / 3.0) * logtwo ((double) (C / (B * A * Ndbl * Nspd)))));
  if (N3to8 == 0)
    {
      N3to8 = 1;
    }

  //Added by Shyam to make area model sensitive to "change tag" feature
   //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  if(!force_tag) {
		//T = ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		T = (int) (ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
	}
  else {
	    T = force_tag_size;
	}
 
  //Commented by Shyam   T =
    //baddr - logtwo ((double) (C)) + logtwo ((double) (A)) + 2 -
    //logtwo ((double) (NSubbanks));

  tag = T * (RWP + ERP + EWP) * Widthtrack;
  assoc = (RWP + ERP) * A * Widthtrack;
  colmux = Ndbl * Nspd * (RWP + EWP + ERP) * Widthtrack;
  predecode = (RWP + ERP + EWP) * N3to8 * 8 * Widthtrack;
  addresslines = ADDRESS_BITS * (RWP + ERP + EWP) * Widthtrack;

  tagsubarray_area =
    tagsubarray (baddr, C, B, A, Ndbl, Ndwl, Nspd, NSubbanks, RWP, ERP, EWP,
		 conservative_NSER, techscaling_factor);
  tagsubblock_area =
    tagsubblock (baddr, C, B, A, Ndbl, Ndwl, Nspd, NSubbanks, SB, RWP, ERP,
		 EWP, conservative_NSER, techscaling_factor);
  //area_all_tagsubarrays =
    //Ndbl * Ndwl * calculate_area (tagsubarray_area,
				  //techscaling_factor) * CONVERT_TO_MMSQUARE;
  area_all_tagsubarrays = Ndbl * Ndwl * tagsubarray_area.height * tagsubarray_area.width * CONVERT_TO_MMSQUARE;


  fixed_tracks_internal = colmux + predecode + assoc;
  fixed_tracks_external = colmux + assoc + addresslines;
  variable_tracks = tag;

  if (SB == 0)
    {
      if (Ndbl * Ndwl == 1)
	{
	  total_height =
	    tagsubarray_area.height + fixed_tracks_external + tag;
	  total_width = tagsubarray_area.width + predecode;
	}
      else
	{
	  total_height =
	    2 * tagsubarray_area.height + fixed_tracks_external + tag;
	  total_width = tagsubarray_area.width + predecode;
	}
    }
  if (SB == 1)
    {
      total_height = tagsubblock_area.height;
      total_width = tagsubblock_area.width;
    }
  if (SB == 2)
    {
      total_height = tagsubblock_area.height;
      total_width = 2 * tagsubblock_area.width + fixed_tracks_external + tag;
    }
  if (SB == 4)
    {
      total_height =
	2 * tagsubblock_area.height + fixed_tracks_external + tag;
      total_width =
	2 * tagsubblock_area.width + fixed_tracks_internal +
	variable_tracks / 2;
    }
  if (SB == 8)
    {
      total_height =
	2 * tagsubblock_area.height + fixed_tracks_internal +
	variable_tracks / 2;
      total_width =
	2 * (2 * tagsubblock_area.width + variable_tracks / 4) +
	fixed_tracks_external + tag;
    }
  if (SB > 8)
    {
      blocks = SB / 4;
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
      //htree = (int) (logtwo ((double) (blocks)));
	  htree = (int) (logtwo ((double) (blocks)) + EPSILON);
      inter_height = tagsubblock_area.height;
      inter_width = tagsubblock_area.width;
      multiplier = 1;

      if (htree % 2 == 0)
	{
	  iter_height = htree / 2;
	}

      if (htree % 2 == 0)
	{
	  for (i = 0; i <= iter_height; i++)
	    {
	      if (i == iter_height)
		{
		  total_height =
		    2 * inter_height + tag / blocks * multiplier +
		    fixed_tracks_external;
		  total_width =
		    2 * inter_width + tag / (2 * blocks) * multiplier +
		    fixed_tracks_internal;}
	      else
		{
		  total_height =
		    2 * inter_height + tag / blocks * multiplier +
		    fixed_tracks_internal;
		  total_width =
		    2 * inter_width + tag / (2 * blocks) * multiplier +
		    fixed_tracks_internal;
		  inter_height = total_height;
		  inter_width = total_width;
		  multiplier = multiplier * 4;
		}
	    }
	}
      else
	{
	  htree_half = htree - 1;
	  iter_height = htree_half / 2;
	  for (i = 0; i <= iter_height; i++)
	    {
	      total_height =
		2 * inter_height + tag / blocks * multiplier +
		fixed_tracks_internal;
	      total_width =
		2 * inter_width + tag / (2 * blocks) * multiplier +
		fixed_tracks_internal;
	      inter_height = total_height;
	      inter_width = total_width;
	      multiplier = multiplier * 4;
	    }
	  total_width =
	    2 * inter_width + tag / (2 * blocks) * multiplier +
	    fixed_tracks_external;
	}
    }

  tagarray_area.width = total_width;
  tagarray_area.height = total_height;

  temp.height = tagarray_area.width;
  temp.width = tagarray_area.height;
  temp_aspect =
    ((temp.height / temp.width) >
     1.0) ? (temp.height / temp.width) : 1.0 / (temp.height / temp.width);
  aspect_ratio_tag =
    ((tagarray_area.height / tagarray_area.width) >
     1.0) ? (tagarray_area.height / tagarray_area.width) : 1.0 /
    (tagarray_area.height / tagarray_area.width);
  if (aspect_ratio_tag > temp_aspect)
    {
      tagarray_area.height = temp.height;
      tagarray_area.width = temp.width;
    }

  aspect_ratio_tag =
    ((tagarray_area.height / tagarray_area.width) >
     1.0) ? (tagarray_area.height / tagarray_area.width) : 1.0 /
    (tagarray_area.height / tagarray_area.width);

  return (tagarray_area);
}

void
Cacti4_2::area (int baddr,int b0,int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,double NSubbanks,parameter_type *parameters,
      arearesult_type *result)
{
  int rows_datasubarray;
  //v4.1. Making colns_datasubarray double datatype since it is dependent on Nspd 
  //which is double. Note that based on its usage below colns_datasubarray is a misnomer. 
  //colns_datasubarray is actually the degree of output muxing. 
  double colns_datasubarray;
  int rows_tagsubarray, colns_tagsubarray;

  rows_datasubarray =
    (parameters->cache_size /
     ((parameters->block_size) * (parameters->data_associativity) * Ndbl * Nspd));
  colns_datasubarray = (Ndbl * Nspd);
  rows_tagsubarray =
    (parameters->cache_size /
     ((parameters->block_size) * (parameters->tag_associativity) * Ntbl * Ntspd));
  colns_tagsubarray = (Ntbl * Ntspd);

  result->dataarray_area =
    dataarray (parameters->cache_size, parameters->block_size,
	       parameters->data_associativity, Ndbl, Ndwl, Nspd, b0,
	       parameters->num_readwrite_ports, parameters->num_read_ports,
	       parameters->num_write_ports,
	       parameters->num_single_ended_read_ports,
	       parameters->fudgefactor);

  result->datapredecode_area =
    predecode_area (rows_datasubarray, parameters->num_readwrite_ports,
		    parameters->num_read_ports, parameters->num_write_ports);
  result->datacolmuxpredecode_area =
    predecode_area (colns_datasubarray, parameters->num_readwrite_ports,
		    parameters->num_read_ports, parameters->num_write_ports);
  result->datacolmuxpostdecode_area =
    postdecode_area (colns_datasubarray, parameters->num_readwrite_ports,
		     parameters->num_read_ports, parameters->num_write_ports);
  result->datawritesig_area =
    muxdrvsig (parameters->data_associativity, parameters->block_size, b0);

 //if-else on pure_sram_flag added by Shyam so that tag array area not calculated in
  //pure SRAM mode
  if(!pure_sram_flag)  {

	  result->tagarray_area =
		tagarray (baddr, parameters->cache_size, parameters->block_size,
			  parameters->tag_associativity, Ntbl, Ntwl, Ntspd, NSubbanks,
			  parameters->num_readwrite_ports, parameters->num_read_ports,
			  parameters->num_write_ports,
			  parameters->num_single_ended_read_ports,
			  parameters->fudgefactor);
	  result->tagpredecode_area =
		predecode_area (rows_tagsubarray, parameters->num_readwrite_ports,
				parameters->num_read_ports, parameters->num_write_ports);
	  result->tagcolmuxpredecode_area =
		predecode_area (colns_tagsubarray, parameters->num_readwrite_ports,
				parameters->num_read_ports, parameters->num_write_ports);
	  result->tagcolmuxpostdecode_area =
		postdecode_area (colns_tagsubarray, parameters->num_readwrite_ports,
				 parameters->num_read_ports, parameters->num_write_ports);
	  result->tagoutdrvdecode_area =
		muxdriverdecode (parameters->block_size, b0,
				 parameters->num_readwrite_ports,
				 parameters->num_read_ports, parameters->num_write_ports);
	  result->tagoutdrvsig_area =
		muxdrvsig (parameters->tag_associativity, parameters->block_size, b0);
  }
  else{
		result->tagarray_area.height = 0;
		result->tagarray_area.width = 0;
		result->tagarray_area.scaled_area = 0;
		result->tagpredecode_area.height = 0;
		result->tagpredecode_area.width = 0;
		result->tagpredecode_area.scaled_area = 0;
		result->tagcolmuxpredecode_area.height = 0;
		result->tagcolmuxpredecode_area.width = 0;
		result->tagcolmuxpredecode_area.scaled_area = 0;
		result->tagcolmuxpostdecode_area.height = 0;
		result->tagcolmuxpostdecode_area.width = 0;
		result->tagcolmuxpostdecode_area.scaled_area = 0;
		result->tagoutdrvdecode_area.height = 0;
		result->tagoutdrvdecode_area.width = 0;
		result->tagoutdrvdecode_area.scaled_area = 0;
		result->tagoutdrvsig_area.height = 0;
		result->tagoutdrvsig_area.width = 0;
		result->tagoutdrvsig_area.scaled_area = 0;

  }

  /*result->totalarea =
    calculate_area (result->dataarray_area,
		    parameters->fudgefactor) +
    calculate_area (result->datapredecode_area,
		    parameters->fudgefactor) +
    calculate_area (result->datacolmuxpredecode_area,
		    parameters->fudgefactor) +
    calculate_area (result->datacolmuxpostdecode_area,
		    parameters->fudgefactor) +
    (parameters->num_readwrite_ports +
     parameters->num_write_ports) * calculate_area (result->datawritesig_area,
						    parameters->fudgefactor) +
    calculate_area (result->tagarray_area,
		    parameters->fudgefactor) +
    calculate_area (result->tagpredecode_area,
		    parameters->fudgefactor) +
    calculate_area (result->tagcolmuxpredecode_area,
		    parameters->fudgefactor) +
    calculate_area (result->tagcolmuxpostdecode_area,
		    parameters->fudgefactor) +
    calculate_area (result->tagoutdrvdecode_area,
		    parameters->fudgefactor) +
    (parameters->num_readwrite_ports +
     parameters->num_read_ports) * calculate_area (result->tagoutdrvsig_area,
						   parameters->fudgefactor);*/


  result->totalarea = result->dataarray_area.height * result->dataarray_area.width + 
	  result->datapredecode_area.height * result->datapredecode_area.width  + 
	  result->datacolmuxpredecode_area.height * result->datacolmuxpredecode_area.width + 
	  result->datacolmuxpostdecode_area.height * result->datacolmuxpostdecode_area.width +
    (parameters->num_readwrite_ports + parameters->num_write_ports) * result->datawritesig_area.height * result->datawritesig_area.width + 
	result->tagarray_area.height * result->tagarray_area.width + 
	result->tagpredecode_area.height * result->tagpredecode_area.width + 
	result->tagcolmuxpredecode_area.height * result->tagcolmuxpredecode_area.width +
    result->tagcolmuxpostdecode_area.height * result->tagcolmuxpostdecode_area.width + 
	result->tagoutdrvdecode_area.height * result->tagoutdrvdecode_area.width +
    (parameters->num_readwrite_ports +  parameters->num_read_ports) * result->tagoutdrvsig_area.height * result->tagoutdrvsig_area.width;

}

Cacti4_2::area_type
Cacti4_2::fadecode_row (int C,int B,int Ndbl,int RWP,int ERP,int EWP)	/*returns area of post decode */
{
  int numstack;
  double decodeNORwidth, firstinv;
  Cacti4_2::area_type decinv, worddriveinv, postdecodearea;

  //v4.1: using integer casting below 
  //numstack =
    //ceil ((1.0 / 3.0) * logtwo ((double) ((double) C / (double) (B))));
  numstack =
    (int) (ceil ((1.0 / 3.0) * logtwo ((double) ((double) C / (double) (B)))));
  if (numstack == 0)
    numstack = 1;
  if (numstack > 6)
    numstack = 6;
  switch (numstack)
    {
    case 1:
      decodeNORwidth = WidthNOR1;
      break;
    case 2:
      decodeNORwidth = WidthNOR2;
      break;
    case 3:
      decodeNORwidth = WidthNOR3;
      break;
    case 4:
      decodeNORwidth = WidthNOR4;
      break;
    case 5:
      decodeNORwidth = WidthNOR5;
      break;
    case 6:
      decodeNORwidth = WidthNOR6;
      break;
    default:
      printf ("error:numstack=%d\n", numstack);
      printf ("Cacti does not support a series stack of %d transistors !\n",
	      numstack);
      exit (0);
      break;

    }
  decinv = inverter_area (Wdecinvp, Wdecinvn);
  worddriveinv = inverter_area (Wdecinvp, Wdecinvn);
  switch (numstack)
    {
    case 1:
      firstinv = decinv.height;
      break;
    case 2:
      firstinv = decinv.height;
      break;
    case 3:
      firstinv = decinv.height;
      break;
    case 4:
      firstinv = decNandWidth;
      break;
    case 5:
      firstinv = decNandWidth;
      break;
    case 6:
      firstinv = decNandWidth;
      break;
    default:
      printf ("error:numstack=%d\n", numstack);
      printf ("Cacti does not support a series stack of %d transistors !\n",
	      numstack);
      exit (0);
      break;

    }

  /*
  was : postdecodearea.height = BitHeight16x2;
  */
  postdecodearea.height = 2*BitHeight1x1;
  postdecodearea.width =
    (decodeNORwidth + firstinv + worddriveinv.height) * (RWP + EWP);
  return (postdecodearea);
}

Cacti4_2::area_type
Cacti4_2::fasubarray (int baddr,int C,int B,int Ndbl,int RWP,int ERP,int EWP,int NSER,double techscaling_factor)	/* returns area of subarray */
{
  Cacti4_2::area_type FAarea, fadecoderow, faramcell;
  int noof_rowsdata, noof_colnsdata;
  int Tagbits, HTagbits;
  double precharge, widthoverhead, heightoverhead;

  noof_rowsdata = (C / (B * Ndbl));
  noof_colnsdata = (8 * B);
  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  //Tagbits = baddr - logtwo ((double) (B)) + 2;
  Tagbits = (int) (baddr - logtwo ((double) (B)) + 2 + EPSILON);
  
  //v4.1: using integer casting below 
  //HTagbits = ceil ((double) (Tagbits) / 2.0);
  HTagbits = (int)(ceil ((double) (Tagbits) / 2.0));
  precharge =
    Wbitpreequ + 2 * Wbitdropv + Wwrite + 2 * (2 * Widthcontact + 1 / FUDGEFACTOR) +
    3 * Widthptondiff;

  if ((RWP == 1) && (ERP == 0) && (EWP == 0))
    {
      heightoverhead = 0;
      widthoverhead = 0;
    }
  else
    {
      if ((RWP == 1) && (ERP == 1) && (EWP == 0))
	{
	  widthoverhead = FAWidthIncrPer_first_r_port;
	  heightoverhead = FAHeightIncrPer_first_r_port;
	}
      else
	{
	  if ((RWP == 1) && (ERP == 0) && (EWP == 1))
	    {
	      widthoverhead = FAWidthIncrPer_first_rw_or_w_port;
	      heightoverhead = FAHeightIncrPer_first_rw_or_w_port;
	    }
	  else
	    {
	      if (RWP + EWP >= 2)
		{
		  widthoverhead =
		    FAWidthIncrPer_first_rw_or_w_port + (RWP + EWP -
							 2) *
		    FAWidthIncrPer_later_rw_or_w_port +
		    ERP * FAWidthIncrPer_later_r_port;
		  heightoverhead =
		    FAHeightIncrPer_first_rw_or_w_port + (RWP + EWP -
							  2) *
		    FAHeightIncrPer_later_rw_or_w_port +
		    ERP * FAHeightIncrPer_later_r_port;}
	      else
		{
		  if ((RWP == 0) && (EWP == 0))
		    {
		      widthoverhead =
			FAWidthIncrPer_first_r_port + (ERP -
						       1) *
			FAWidthIncrPer_later_r_port;
		      heightoverhead =
			FAHeightIncrPer_first_r_port + (ERP -
							1) *
			FAHeightIncrPer_later_r_port;;}
		  else
		    {
		      if ((RWP == 0) && (EWP == 1))
			{
			  widthoverhead = ERP * FAWidthIncrPer_later_r_port;
			  heightoverhead = ERP * FAHeightIncrPer_later_r_port;
			}
		      else
			{
			  if ((RWP == 1) && (EWP == 0))
			    {
			      widthoverhead =
				ERP * FAWidthIncrPer_later_r_port;
			      heightoverhead =
				ERP * FAHeightIncrPer_later_r_port;}
			}
		    }
		}
	    }
	}
    }

  faramcell.height =
    ceil ((double) (noof_rowsdata) / 16.0) * stitch_ramv + (CAM2x2Height_1p +
							    2 *
							    heightoverhead) *
    ceil ((double) (noof_rowsdata) / 2.0);

  /*
  was: faramcell.width=(ceil((double)(noof_colnsdata)/16.0))*(BitWidth16x2+16*(Widthtrack*2*(RWP+(ERP-NSER)+EWP-1)+Widthtrack*NSER))+2*(HTagbits*((CAM2x2Width_1p+2*widthoverhead)-Widthcontact))+(BitWidth+Widthtrack*2*(RWP+ERP+EWP-1))+(FArowNANDWidth+FArowNOR_INVWidth)*(RWP+ERP+EWP);
  */
  faramcell.width=noof_colnsdata*(BitWidth1x1+(Widthtrack*2*(RWP+(ERP-NSER)+EWP-1)+Widthtrack*NSER))+2*(HTagbits*((CAM2x2Width_1p+2*widthoverhead)-Widthcontact))+(BitWidth+Widthtrack*2*(RWP+ERP+EWP-1))+(FArowNANDWidth+FArowNOR_INVWidth)*(RWP+ERP+EWP);

  FAarea.height =
    faramcell.height + precharge * (RWP + EWP) + SenseampHeight * (RWP +
								   ERP) +
    DatainvHeight * (RWP + EWP) + FAOutdriveHeight * (RWP + ERP);
  FAarea.width = faramcell.width;

  fadecoderow = fadecode_row (C, B, Ndbl, RWP, ERP, EWP);
  FAarea.width = FAarea.width + fadecoderow.width;

  //area_all_dataramcells =
    //Ndbl * calculate_area (faramcell, techscaling_factor) * CONVERT_TO_MMSQUARE;
  //faarea_all_subarrays =
    //Ndbl * calculate_area (FAarea, techscaling_factor) * CONVERT_TO_MMSQUARE;
  area_all_dataramcells = Ndbl * faramcell.height * faramcell.width * CONVERT_TO_MMSQUARE;
  faarea_all_subarrays = Ndbl * FAarea.height * FAarea.width * CONVERT_TO_MMSQUARE;
  return (FAarea);
}

Cacti4_2::area_type
Cacti4_2::faarea (int baddr,int b0,int C,int B,int Ndbl,int RWP,int ERP,int EWP,int NSER,double techscaling_factor)
{
  Cacti4_2::area_type fasubarray_area, fa_area;
  int Tagbits, blocksel, N3to8;
  double fixed_tracks, predecode, base_height, base_width;
  Cacti4_2::area_type temp;
  double temp_aspect;

  int blocks, htree, htree_half, i, iter;
  double inter_height, inter_width, total_height, total_width;

  
  //v4.1: using integer casting below 
  //N3to8 =
    //ceil ((1.0 / 3.0) * logtwo ((double) ((double) C / (double) (B))));
 N3to8 =
    (int) (ceil ((1.0 / 3.0) * logtwo ((double) ((double) C / (double) (B)))));
  if (N3to8 == 0)
    {
      N3to8 = 1;
    }

  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  Tagbits =  (int)(baddr - logtwo ((double) (B)) + 2 + EPSILON);
  fasubarray_area =
    fasubarray (baddr, C, B, Ndbl, RWP, ERP, EWP, NSER, techscaling_factor);
  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  //blocksel = MAX (logtwo ((double) (B)), (8 * B) / b0);
  blocksel = MAX ((int)(logtwo ((double) (B)) + EPSILON), (8 * B) / b0);
  blocksel =
    (blocksel >
     tracks_outdrvfanand_p) ? (blocksel - tracks_outdrvfanand_p) : 0;

  fixed_tracks =
    Widthtrack * (1 * (RWP + EWP) + b0 * (RWP + ERP + EWP) +
		  Tagbits * (RWP + ERP + EWP) + blocksel * (RWP + ERP + EWP));
  predecode = Widthtrack * (N3to8 * 8) * (RWP + EWP);

  if (Ndbl == 1)
    {
      total_height = fasubarray_area.height + fixed_tracks;
      total_width = fasubarray_area.width + predecode;
    }
  if (Ndbl == 2)
    {
      total_height = 2 * fasubarray_area.height + fixed_tracks;
      total_width = fasubarray_area.width + predecode;
    }
  if (Ndbl == 4)
    {
      total_height = 2 * fasubarray_area.height + fixed_tracks + predecode;
      total_width = 2 * fasubarray_area.width + predecode;
    }
  if (Ndbl > 4)
    {
      blocks = Ndbl / 4;
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
      //htree = (int) (logtwo ((double) (blocks)));
	  htree = (int) (logtwo ((double) (blocks)) + EPSILON);
      base_height = 2 * fasubarray_area.height + fixed_tracks + predecode;
      base_width = 2 * fasubarray_area.width + predecode;

      inter_height = base_height;
      inter_width = base_width;

      if (htree % 2 == 0)
	{
	  iter = htree / 2;
	}

      if (htree % 2 == 0)
	{
	  for (i = 1; i <= iter; i++)
	    {
	      total_height = 2 * (inter_height) + fixed_tracks + predecode;
	      inter_height = total_height;
	      total_width = 2 * (inter_width) + fixed_tracks + predecode;
	      inter_width = total_width;
	    }
	}
      else
	{
	  htree_half = htree - 1;
	  iter = htree_half / 2;
	  if (iter == 0)
	    {
	      total_height = base_height;
	      total_width = 2 * base_width + fixed_tracks + predecode;
	    }
	  else
	    {
	      for (i = 0; i <= iter; i++)
		{
		  total_height = 2 * inter_height + fixed_tracks + predecode;
		  total_width = 2 * inter_width + fixed_tracks + predecode;
		  inter_height = total_height;
		  inter_width = total_width;
		}
	      total_width = 2 * inter_width + fixed_tracks + predecode;
	    }
	}
    }

  fa_area.height = total_height;
  fa_area.width = total_width;

  temp.height = fa_area.width;
  temp.width = fa_area.height;
  temp_aspect =
    ((temp.height / temp.width) >
     1.0) ? (temp.height / temp.width) : 1.0 / (temp.height / temp.width);
  aspect_ratio_data =
    ((fa_area.height / fa_area.width) >
     1.0) ? (fa_area.height / fa_area.width) : 1.0 / (fa_area.height /
						      fa_area.width);
  if (aspect_ratio_data > temp_aspect)
    {
      fa_area.height = temp.height;
      fa_area.width = temp.width;
    }

  aspect_ratio_data =
    ((fa_area.height / fa_area.width) >
     1.0) ? (fa_area.height / fa_area.width) : 1.0 / (fa_area.height /
						      fa_area.width);

  return (fa_area);
}

void
Cacti4_2::fatotalarea (int baddr,int b0,int Ndbl,parameter_type *parameters,arearesult_type *faresult)
{
  Cacti4_2::area_type null_area;

  null_area.height = 0.0;
  null_area.width = 0.0;

  faresult->dataarray_area =
    faarea (baddr, b0, parameters->cache_size, parameters->block_size, Ndbl,
	    parameters->num_readwrite_ports, parameters->num_read_ports,
	    parameters->num_write_ports,
	    parameters->num_single_ended_read_ports, parameters->fudgefactor);
  faresult->datapredecode_area =
    predecode_area (parameters->cache_size / parameters->block_size,
		    parameters->num_readwrite_ports,
		    parameters->num_read_ports, parameters->num_write_ports);
  faresult->datacolmuxpredecode_area = null_area;
  faresult->datacolmuxpostdecode_area = null_area;
  faresult->datawritesig_area = null_area;

  faresult->tagarray_area = null_area;
  faresult->tagpredecode_area = null_area;
  faresult->tagcolmuxpredecode_area = null_area;
  faresult->tagcolmuxpostdecode_area = null_area;
  faresult->tagoutdrvdecode_area =
    muxdriverdecode (parameters->block_size, b0,
		     parameters->num_readwrite_ports,
		     parameters->num_read_ports, parameters->num_write_ports);
  faresult->tagoutdrvsig_area = null_area;
  /*faresult->totalarea =
    (calculate_area (faresult->dataarray_area, parameters->fudgefactor) +
     calculate_area (faresult->datapredecode_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->datacolmuxpredecode_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->datacolmuxpostdecode_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->tagarray_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->tagpredecode_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->tagcolmuxpredecode_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->tagcolmuxpostdecode_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->tagoutdrvdecode_area,
		     parameters->fudgefactor) +
     calculate_area (faresult->tagoutdrvsig_area, parameters->fudgefactor));*/

   faresult->totalarea = faresult->dataarray_area.height * faresult->dataarray_area.width + 
	   faresult->datapredecode_area.height * faresult->datapredecode_area.width +
     faresult->datacolmuxpredecode_area.height * faresult->datacolmuxpredecode_area.width + 
	 faresult->datacolmuxpostdecode_area.height * faresult->datacolmuxpostdecode_area.width +
     faresult->tagarray_area.height * faresult->tagarray_area.width + 
	 faresult->tagpredecode_area.height * faresult->tagpredecode_area.width + 
	 faresult->tagcolmuxpredecode_area.height * faresult->tagcolmuxpredecode_area.width + 
	 faresult->tagcolmuxpostdecode_area.height * faresult->tagcolmuxpostdecode_area.width + 
	 faresult->tagoutdrvdecode_area.height * faresult->tagoutdrvdecode_area.width +
     faresult->tagoutdrvsig_area.height * faresult->tagoutdrvsig_area.width;

}


void
Cacti4_2::area_subbanked (int baddr,int b0,int RWP,int ERP,int EWP,int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,
		double NSubbanks,parameter_type *parameters,Cacti4_2::area_type *result_subbanked,arearesult_type *result)
{
  arearesult_type result_area;
  Cacti4_2::area_type temp;
  double temp_aspect;
  int blocks, htree, htree_double;
  double base_height, base_width, inter_width, inter_height, total_height,
    total_width;
  int base_subbanks, inter_subbanks;
  int i, iter_height, iter_width, iter_width_double;

  area_all_dataramcells = 0.0;
  area_all_tagramcells = 0.0;
  aspect_ratio_data = 1.0;
  aspect_ratio_tag = 1.0;
  aspect_ratio_subbank = 1.0;
  aspect_ratio_total = 1.0;


  if (parameters->fully_assoc == 0)
    {
      area (baddr, b0, Ndbl, Ndwl, Nspd, Ntbl, Ntwl, Ntspd, NSubbanks,
	    parameters, &result_area);
    }
  else
    {
      fatotalarea (baddr, b0, Ndbl, parameters, &result_area);
    }

  result->dataarray_area = result_area.dataarray_area;
  result->datapredecode_area = result_area.datapredecode_area;
  result->datacolmuxpredecode_area = result_area.datacolmuxpredecode_area;
  result->datacolmuxpostdecode_area = result_area.datacolmuxpostdecode_area;
  result->datawritesig_area = result_area.datawritesig_area;
  result->tagarray_area = result_area.tagarray_area;
  result->tagpredecode_area = result_area.tagpredecode_area;
  result->tagcolmuxpredecode_area = result_area.tagcolmuxpredecode_area;
  result->tagcolmuxpostdecode_area = result_area.tagcolmuxpostdecode_area;
  result->tagoutdrvdecode_area = result_area.tagoutdrvdecode_area;
  result->tagoutdrvsig_area = result_area.tagoutdrvsig_area;
  result->totalarea = result_area.totalarea;
  result->total_dataarea = result_area.total_dataarea;
  result->total_tagarea = result_area.total_tagarea;

  if (NSubbanks == 1)
    {
      total_height = result_area.dataarray_area.height;
      total_width =
	result_area.dataarray_area.width + result_area.tagarray_area.width;
    }
  //v4.2: The height/width components were not getting multiplied by WIREPITCH. Fixing 
  //that problem
  if (NSubbanks == 2)
    {
      total_height =
	result_area.dataarray_area.height + (RWP + ERP + EWP) * ADDRESS_BITS * WIREPITCH;
      total_width =
	(result_area.dataarray_area.width +
	 result_area.tagarray_area.width) * 2 + (ADDRESS_BITS +
+						 BITOUT) * WIREPITCH * NSubbanks * (RWP +
									ERP +
									EWP);
    }
  if (NSubbanks == 4)
    {
      total_height =
	2 * result_area.dataarray_area.height + 2 * (RWP + ERP +
						     EWP) * ADDRESS_BITS * WIREPITCH;
      total_width =
	(result_area.dataarray_area.width +
	 result_area.tagarray_area.width) * 2 + (ADDRESS_BITS +
						 BITOUT) * WIREPITCH * NSubbanks * (RWP +
									ERP +
									EWP);

    }
  if (NSubbanks == 8)
    {
      total_height =
	(result_area.dataarray_area.width +
	 result_area.tagarray_area.width) * 2 + (ADDRESS_BITS +
						 BITOUT) * WIREPITCH * NSubbanks * (RWP +
									ERP +
									EWP) *
	0.5;
      total_width =
	2 * (2 * result_area.dataarray_area.height +
	     2 * (RWP + ERP + EWP) * ADDRESS_BITS * WIREPITCH) + (ADDRESS_BITS +
						      BITOUT) * WIREPITCH * NSubbanks *
	(RWP + ERP + EWP);
    }

  if (NSubbanks > 8)
    {
	   //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
       //the final int value is the correct one 
      //blocks = NSubbanks / 16 ;
	  blocks = (int) (NSubbanks / 16 + EPSILON) ;
      //htree = (int) (logtwo ((double) (blocks)));
	  htree = (int) (logtwo ((double) (blocks)) + EPSILON);
      base_height =
	2 *
	((result_area.
	  dataarray_area.width + result_area.tagarray_area.width) * 2 +
	 (ADDRESS_BITS + BITOUT) * WIREPITCH * 16 * (RWP + ERP + EWP) * 0.25) +
	(ADDRESS_BITS + BITOUT) * WIREPITCH * 16 * (RWP + ERP + EWP) * 0.5;
      base_width =
	2 * (2 * result_area.dataarray_area.height +
	     2 * (RWP + ERP + EWP) * ADDRESS_BITS) * WIREPITCH + (ADDRESS_BITS +
						      BITOUT) * WIREPITCH * 16 * (RWP +
								      ERP +
								      EWP) *
	0.25;
      base_subbanks = 16;
      if (htree % 2 == 0)
	{
	  iter_height = htree / 2;
	}
      else
	{
	  iter_height = (htree - 1) / 2;
	}

      inter_height = base_height;
      inter_subbanks = base_subbanks;

      if (iter_height == 0)
	{
	  total_height = base_height;
	}
      else
	{
	  for (i = 1; i <= iter_height; i++)
	    {
	      total_height =
		2 * (inter_height) + (ADDRESS_BITS +
				      BITOUT) * WIREPITCH * 4 * inter_subbanks * (RWP +
								      ERP +
								      EWP) *
		0.5;
	      inter_height = total_height;
	      inter_subbanks = inter_subbanks * 4;
	    }
	}

      inter_width = base_width;
      inter_subbanks = base_subbanks;
      iter_width = 10;

      if (htree % 2 == 0)
	{
	  iter_width = htree / 2;
	}

      if (iter_width == 0)
	{
	  total_width = base_width;
	}
      else
	{
	  if (htree % 2 == 0)
	    {
	      for (i = 1; i <= iter_width; i++)
		{
		  total_width =
		    2 * (inter_width) + (ADDRESS_BITS +
					 BITOUT) * WIREPITCH * inter_subbanks * (RWP +
								     ERP +
								     EWP);
		  inter_width = total_height;
		  inter_subbanks = inter_subbanks * 4;
		}
	    }
	  else
	    {
	      htree_double = htree + 1;
	      iter_width_double = htree_double / 2;
	      for (i = 1; i <= iter_width_double; i++)
		{
		  total_width =
		    2 * (inter_width) + (ADDRESS_BITS +
					 BITOUT) * WIREPITCH * inter_subbanks * (RWP +
								     ERP +
								     EWP);
		  inter_width = total_height;
		  inter_subbanks = inter_subbanks * 4;
		}
	      total_width +=
		(ADDRESS_BITS + BITOUT) * WIREPITCH * (RWP + ERP + EWP) * NSubbanks / 2;
	    }
	}
    }

  result_subbanked->height = total_height;
  result_subbanked->width = total_width;

  temp.width = result_subbanked->height;
  temp.height = result_subbanked->width;

  temp_aspect =
    ((temp.height / temp.width) >
     1.0) ? (temp.height / temp.width) : 1.0 / (temp.height / temp.width);

  aspect_ratio_total = (result_subbanked->height / result_subbanked->width);

  aspect_ratio_total =
    (aspect_ratio_total >
     1.0) ? (aspect_ratio_total) : 1.0 / (aspect_ratio_total);


  if (aspect_ratio_total > temp_aspect)
    {
      result_subbanked->height = temp.height;
      result_subbanked->width = temp.width;
    }

  aspect_ratio_subbank =
    (result_area.dataarray_area.height /
     (result_area.dataarray_area.width + result_area.tagarray_area.width));
  aspect_ratio_subbank =
    (aspect_ratio_subbank >
     1.0) ? (aspect_ratio_subbank) : 1.0 / (aspect_ratio_subbank);
  aspect_ratio_total = (result_subbanked->height / result_subbanked->width);
  aspect_ratio_total =
    (aspect_ratio_total >
     1.0) ? (aspect_ratio_total) : 1.0 / (aspect_ratio_total);

}


int
Cacti4_2::data_organizational_parameters_valid (int B,int A,int C,int Ndwl,int Ndbl,double Nspd,char assoc,double NSubbanks)
{
	int temp = 0;
	double before = 0.0;
	int tagbits = 0;
	int data_rows = 0, data_columns = 0, tag_rows = 0, tag_columns = 0;
  /* don't want more than 8 subarrays for each of data/tag */

  if (assoc == 0) {
		if (Ndwl * Ndbl > MAXSUBARRAYS) {return (FALSE);}
		/* add more constraints here as necessary */
		//v4.1: Number of rows per subarray is (C / (B * A * Ndbl * Nspd),
		//not (C / (8 * B * A * Ndbl * Nspd)
		//if (C / (8 * B * A * Ndbl * Nspd) <= 0) {return (FALSE);}
		if (C / (B * A * Ndbl * Nspd) <= 0) {return (FALSE);}
		if ((8 * B * A * Nspd / Ndwl) <= 0) {return (FALSE);}
		/*dt: Don't want ridicolously small arrays*/
		/*dt: data side: number of rows should be greater than 8 */
		
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
		//the final int value is the correct one 
		//data_rows = C/(B*A*Ndbl*Nspd) ;
		data_rows = (int) (C/(B*A*Ndbl*Nspd) + EPSILON);
		if(8 > data_rows) {return (FALSE);}
		//dt: data side: number of rows should be less than 4k 
		if(2048 < data_rows) {return (FALSE);}
		//dt: data side: number of columns should be greater than 16 
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//data_columns = 8*B*A*Nspd/Ndwl;
		data_columns = (int) (8*B*A*Nspd/Ndwl + EPSILON);
		if(16 > data_columns) {return (FALSE);}
		//dt: data side: number of columns should be less than 1k 
		if(512 < data_columns) {return (FALSE);}

		//Added by Shyam to specify a minimum 8KB mat (subarray) size in the data array
		//if((C <= 8192)&&((Ndwl > 1)||(Ndbl > 1))) return (FALSE);
		//End of Shyam's minimum mat size change
		
  }
  else {
		if (C / (2 * B * Ndbl) <= 0) {return (FALSE);}
		if (Ndbl > MAXDATAN) {return (FALSE);}	
  }

  return (TRUE);
}

int
Cacti4_2::tag_organizational_parameters_valid (int B,int A,int C,int Ntwl,int Ntbl,int Ntspd,char assoc,double NSubbanks)
{
	int temp = 0;
	double before = 0.0;
	int tagbits = 0;
	int data_rows = 0, data_columns = 0, tag_rows = 0, tag_columns = 0;
  /* don't want more than 8 subarrays for each of data/tag */

  if (assoc == 0) {
		if (Ntwl * Ntbl > MAXSUBARRAYS) {return (FALSE);}
		/* add more constraints here as necessary */
		//v4.1: Number of rows per subarray is (C / (B * A * Ntbl * Ntspd)
		//not (C / (8 * B * A * Ntbl * Ntspd)
		//if (C / (8 * B * A * Ntbl * Ntspd) <= 0) {return (FALSE);}
		if (C / (B * A * Ntbl * Ntspd) <= 0) {return (FALSE);}
		if ((8 * B * A * Ntspd / Ntwl) <= 0) {return (FALSE);}
		/*dt: Don't want ridicolously small arrays*/
		/*dt: tag side: number of rows should be greater than 8 */
		
		tag_rows = C/(B*A*Ntbl*Ntspd);
		//Shyam for vX.X: tag_rows = number of sets is max when Ntbl=Ntspd=1. Checking that
		//tagrows is at least 1 instead of 8. FIX THIS LATER
		if(8 > tag_rows) {return (FALSE);}
		//dt: tag side: number of rows should be less than 1k 
		if(512 < tag_rows) {return (FALSE);}
		// dt: tag side: number of column should be greater than 8 
		tagbits = ADDRESS_BITS+EXTRA_TAG_BITS;
		 //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//before = logtwo((double)C);
		before = (int) (logtwo((double)C) + EPSILON);
		temp = (int)(before);
		tagbits -= temp;
		//tagbits += (int)logtwo((double)A);
		//tagbits -= (int)(logtwo(NSubbanks));
		tagbits += (int) (logtwo((double)A) + EPSILON);
		tagbits -= (int)((logtwo(NSubbanks)) + EPSILON);
		tag_columns = tagbits * A * Ntspd/Ntwl;
		if(8 > tag_columns) {return (FALSE);}
		// dt: tag side: number of column should be less than 4k 
		if(512 < tag_columns) {return (FALSE);}
		
		
  }
  else {
		if (Ntbl > MAXTAGN) {return (FALSE);}
		
  }

  return (TRUE);
}

/* from leakage.c */

/*dt: added for windows compatibility */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Box-Mueller Method */

double Cacti4_2::box_mueller(double std_var, double value)
{
	double temp;
	double temp1;
	double random;
	/* dt: drand48 not supported in windows 
			random = drand48();
	*/
	random = rand();
	temp = sqrt((double)(-2.00 * (double)log(random)));
	random = rand();
	
	temp1 = cos(2.00 * M_PI * random);

	return(temp * temp1 * std_var * value);
}


/* ************************************************************************ */

/* Calculating the NMOS I Normalized Leakage From the BSIM Equation.*/
/* Also Using Box-Mueller to Find the Random Samples Due to Any Variation */
/* In any of the parameters like length, Vdd etc. */
/* ************************************************************************ */
double Cacti4_2::nmos_ileakage(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0)
{
	double Tox_Std_Array[No_of_Samples];
	double Vdd_Std_Array[No_of_Samples];
	double Tech_Std_Array[No_of_Samples];
	double Vthn_Std_Array[No_of_Samples];
	double Ileak_Std_Array[No_of_Samples];
	int i;
	double mean =0.0;

	if(Tox_Std || Tech_Std || Vdd_Std || Vthn_Std)
	{
		for(i =0; i<No_of_Samples;i++)
		{
		    Tox_Std_Array[i] = tox0;
		    Vdd_Std_Array[i] = Volt;
		    Tech_Std_Array[i] = tech_length0;
		    Vthn_Std_Array[i] = Vth0;
		}
	}

	if(Tox_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tox_Std_Array[i] = tox0 + box_mueller(Tox_Std,tox0);
	  }
	if(Tech_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tech_Std_Array[i] = tech_length0 + box_mueller(Tech_Std,tech_length0);
	  }
	if(Vdd_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vdd_Std_Array[i] = Volt + box_mueller(Vdd_Std,Volt);
	  }
	if(Vthn_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vthn_Std_Array[i] = Vth0 + box_mueller(Vthn_Std,Vth0);
	  }

	if(Tox_Std || Tech_Std || Vdd_Std || Vthn_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      {
	     Ileak_Std_Array[i] = nmos_ileakage_var(aspect_ratio, Vdd_Std_Array[i], Vthn_Std_Array[i],  Tkelvin,  Tox_Std_Array[i], Tech_Std_Array[i]);

	      }
	  }
	else
	  {
		return(nmos_ileakage_var(aspect_ratio,Volt,Vth0,Tkelvin, tox0, tech_length0));
	  }

	for(i =0; i<No_of_Samples;i++)
		mean += Ileak_Std_Array[i];
	mean = mean/(double)No_of_Samples;

	return mean;


}

double Cacti4_2::nmos_ileakage_var(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0, double tech_length)
{
	double Ileak;
	double Vthermal;
	double Vth,temp , Vnoff;
	double param1,param2,param3, param4,param5,param6;
	double temp1;
	param1 = (aspect_ratio * tech_length0 *M0n*Cox)/tech_length;
	/* Thermal Voltage */
	Vthermal =((Bk*Tkelvin)/Qparam);
	/* Vdd Fitting */
	temp =  Nb*(Volt- Volt0);
	param2 = exp(temp);
	param3 = 1-exp((-Volt/Vthermal));
	Vth =Vth0 + Vnthx * (Tkelvin-300);
	Vnoff = Vnoff0 + Nfix*(Vth0-Vthn);
	param4 = exp(((-fabs(Vth)-Vnoff)/(NEta*Vthermal)));
	temp = (tech_length0 - tech_length) * L_nmos_d ;
	param5 = exp(temp);
	temp1 = (tox0 - Tox) * Tox_nmos_e;
	param6 = exp(temp1);
	Ileak = param1*pow(Vthermal,2.0)*param2*param3*param4*param5*param6;

	return Ileak;
}

/* ************************************************************************ */

/* Calculating the PMOS I Normalized Leakage From the BSIM Equation.*/
/* Also Using Box-Mueller to Find the Random Samples Due to Any Variation */
/* In any of the parameters like length, Vdd etc. */
/* ************************************************************************ */
double Cacti4_2::pmos_ileakage(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0)
{

	double Tox_Std_Array[No_of_Samples];
	double Vdd_Std_Array[No_of_Samples];
	double Tech_Std_Array[No_of_Samples];
	double Vthp_Std_Array[No_of_Samples];
	double Ileak_Std_Array[No_of_Samples];
	int i;
	double mean =0.0;

	if(Tox_Std || Tech_Std || Vdd_Std || Vthp_Std) {
		for(i =0; i<No_of_Samples;i++)
	  	{
	  		Tox_Std_Array[i] = tox0;
	  		Vdd_Std_Array[i] = Volt;
	    	Tech_Std_Array[i] = tech_length0;
	    	Vthp_Std_Array[i] = Vth0;
	  	}
	}

	if(Tox_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tox_Std_Array[i] = tox0 + box_mueller(Tox_Std,tox0);
	  }
	if(Tech_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tech_Std_Array[i] = tech_length0 + box_mueller(Tech_Std,tech_length0);
	  }
	if(Vdd_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vdd_Std_Array[i] = Volt + box_mueller(Vdd_Std,Volt);
	  }
	if(Vthp_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vthp_Std_Array[i] = Vth0 + box_mueller(Vthp_Std,Vth0);
	  }

	if(Tox_Std || Tech_Std || Vdd_Std || Vthp_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
		    Ileak_Std_Array[i] =  pmos_ileakage_var(aspect_ratio, Vdd_Std_Array[i], Vthp_Std_Array[i],  Tkelvin,  Tox_Std_Array[i], Tech_Std_Array[i]);
	  }

	else
	  {
		return (pmos_ileakage_var(aspect_ratio,Volt,  Vth0, Tkelvin, tox0, tech_length0));
	  }

	for(i =0; i<No_of_Samples;i++)
		mean += Ileak_Std_Array[i];
	mean = mean/(double)No_of_Samples;

	return mean;

}

double Cacti4_2::pmos_ileakage_var(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0, double tech_length) {
	double Ileak;
	double Vthermal;
	double Vth, temp ,temp1,Vpoff;
	double param1,param2,param3,param4,param5,param6;
	param1 = (aspect_ratio * tech_length0 *M0p*Cox )/tech_length;
	/* Thermal Voltage */
	Vthermal =((Bk*Tkelvin)/Qparam);
	/* Vdd Fitting */
	temp =  Pb*(Volt- Volt0);
	param2 = exp(temp);
	param3 = 1-exp((-Volt/Vthermal));
	Vth =Vth0 + Vpthx * (Tkelvin-300);
	Vpoff = Vpoff0 + Pfix*(Vth0-Vthp);
	param4 = exp(((-fabs(Vth)-Vpoff)/(PEta*Vthermal)));
	temp = (tech_length0 - tech_length) * L_nmos_d ;
	param5 = exp(temp);
	temp1 = (tox0 - Tox) * Tox_nmos_e;
	param6 = exp(temp1);
	Ileak = param1*pow(Vthermal,2.0)*param2*param3*param4*param5*param6;

	return Ileak;
}

double Cacti4_2::simplified_cmos_leakage(double naspect_ratio,double paspect_ratio, double nVth0, double pVth0,
							   double *norm_nleak, double *norm_pleak)
{
	double sum;
	double pIleak,nIleak;
	double nVth,pVth,Vpoff,Vnoff;
	double nparam1,pparam1,nparam4,pparam4;

	nparam1 = naspect_ratio * precalc_nparamf;
	pparam1 = paspect_ratio * precalc_pparamf;
	
	nVth =nVth0 + precalc_Vnthx;
	pVth =pVth0 + precalc_Vpthx;
	Vnoff = Vnoff0 + Nfix*(nVth0-Vthn);
	Vpoff = Vpoff0 + Pfix*(pVth0-Vthp);
	nparam4 = exp(((-fabs(nVth)-Vnoff)*precalc_inv_nVthermal));
	pparam4 = exp(((-fabs(pVth)-Vpoff)*precalc_inv_pVthermal));

	*norm_nleak = precalc_nparamf*nparam4*precalc_nparaml;
	*norm_pleak = precalc_pparamf*pparam4*precalc_pparaml;
	
	nIleak = nparam1*nparam4*precalc_nparaml;
	pIleak = pparam1*pparam4*precalc_pparaml;

	sum = nIleak + pIleak;
	return sum;

}
double Cacti4_2::optimized_simplified_cmos_leakage(double naspect_ratio,double paspect_ratio, double nVth0, double pVth0,
							   double * nleak, double * pleak)
{
	double sum;
	double pIleak,nIleak;
	double nVth,pVth,Vpoff,Vnoff;
	double nparam1,pparam1,nparam4,pparam4;

	nparam1 = naspect_ratio * precalc_nparamf;
	pparam1 = paspect_ratio * precalc_pparamf;
	
	nVth =nVth0 + precalc_Vnthx;
	pVth =pVth0 + precalc_Vpthx;
	Vnoff = Vnoff0 + Nfix*(nVth0-Vthn);
	Vpoff = Vpoff0 + Pfix*(pVth0-Vthp);
	nparam4 = exp(((-fabs(nVth)-Vnoff)/(NEta*precalc_Vthermal)));
	pparam4 = exp(((-fabs(pVth)-Vpoff)/(PEta*precalc_Vthermal)));
	
	nIleak = nparam1*nparam4*precalc_nparaml;
	pIleak = pparam1*pparam4*precalc_pparaml;
	(*nleak) = precalc_nparamf*nparam4*precalc_nparaml;
	(*pleak) = precalc_pparamf*pparam4*precalc_pparaml;

	sum = nIleak + pIleak;
	return sum;

}
double Cacti4_2::simplified_nmos_leakage(double naspect_ratio, double nVth0)
{
	//double sum;
	double nIleak;
	double nVth,Vnoff;
	double nparam1,nparam4;

	if(have_leakage_params) {
		nparam1 = naspect_ratio * precalc_nparamf;
		nVth =nVth0 + precalc_Vnthx;
		Vnoff = Vnoff0 + Nfix*(nVth0-Vthn);
		nparam4 = exp(((-fabs(nVth)-Vnoff)/(NEta*precalc_Vthermal)));
		nIleak = nparam1*nparam4*precalc_nparaml;
	}
	else {
		nIleak = 0;
	}

	return nIleak;
}

double Cacti4_2::simplified_pmos_leakage(double paspect_ratio, double pVth0)
{
	//double sum;
	double pIleak;
	double pVth,Vpoff;
	double pparam1,pparam4;

	if(have_leakage_params) {
		pparam1 = paspect_ratio * precalc_pparamf;
		pVth =pVth0 + precalc_Vpthx;
		Vpoff = Vpoff0 + Pfix*(pVth0-Vthp);
		pparam4 = exp(((-fabs(pVth)-Vpoff)/(PEta*precalc_Vthermal)));
		pIleak = pparam1*pparam4*precalc_pparaml;
	}
	else {
		pIleak = 0;
	}
		
	return pIleak;
}

void Cacti4_2::precalc_leakage_params(double Volt,double Tkelvin,double tox0, double tech_length) {
	double temp1, temp2, temp3;
	double precalc_param3, precalc_param5, precalc_param6;

	precalc_Vnthx = Vnthx * (Tkelvin-300);
	precalc_Vpthx = Vpthx * (Tkelvin-300);
	/* Thermal Voltage */
	precalc_Vthermal =((Bk*Tkelvin)/Qparam);
	precalc_inv_nVthermal = 1.0/(NEta *precalc_Vthermal);
	precalc_inv_pVthermal = 1.0/(PEta *precalc_Vthermal);
	precalc_nparamf = tech_length0 *M0n*Cox /tech_length;
	precalc_pparamf = tech_length0 *M0p*Cox /tech_length;
	/* Vdd Fitting */
	temp1 =  Nb*(Volt- Volt0);
	precalc_nparam2 = exp(temp1);
	temp1 =  Pb*(Volt- Volt0);
	precalc_pparam2 = exp(temp1);
	precalc_param3 = 1-exp((-Volt/precalc_Vthermal));

	temp2 = (tech_length0 - tech_length) * L_nmos_d ;
	precalc_param5 = exp(temp2);
	temp3 = (tox0 - Tox) * Tox_nmos_e;
	precalc_param6 = exp(temp3);
	precalc_nparaml = pow(precalc_Vthermal,2.0)*precalc_nparam2*precalc_param3*precalc_param5*precalc_param6;
	precalc_pparaml = pow(precalc_Vthermal,2.0)*precalc_pparam2*precalc_param3*precalc_param5*precalc_param6;
}

void Cacti4_2::init_tech_params_default_process()
{
	Cndiffarea = 0.137e-15;
    Cpdiffarea = 0.343e-15;
    Cndiffside = 0.275e-15;
	Cpdiffside = 0.275e-15;
	Cndiffovlp = 0.138e-15;
	Cpdiffovlp = 0.138e-15;
	Cnoxideovlp = 0.263e-15;
    Cpoxideovlp = 0.338e-15;
    Leff = 0.8;
    inv_Leff = 1.25;
    Cgate = 1.95e-15;	
	Cgatepass = 1.45e-15;		
    Cpolywire = 0.25e-15;			 
    Rnchannelstatic = 25800;
    Rpchannelstatic = 61200;
    Rnchannelon = 8751;
    Rpchannelon = 20160;
    Wdecdrivep = 360;
	Wdecdriven = 240;
    Wworddrivemax = 100;
    Wmemcella = 0.9;
    Wmemcellpmos = 0.65;
    Wmemcellnmos = 2.0;
	Wmemcellbscale = 2;
	Wpchmax = 25.0;
    Wcompinvp1 = 10.0;
	Wcompinvn1 = 6.0;
	Wcompinvp2 = 20.0;
	Wcompinvn2 = 12.0;
	Wcompinvp3 = 40.0;
    Wcompinvn3 = 24.0;
    Wevalinvp = 80.0;
    Wevalinvn = 40.0;
    Wfadriven = 50.0;
    Wfadrivep = 100.0;
    Wfadrive2n = 200.0;
	Wfadrive2p = 400.0;
    Wfadecdrive1n = 5.0;
    Wfadecdrive1p = 10.0;
    Wfadecdrive2n = 20.0;
    Wfadecdrive2p = 40.0;
    Wfadecdriven = 50.0;
    Wfadecdrivep = 100.0;
    Wfaprechn = 6.0;
    Wfaprechp = 10.0;
    Wdummyn = 10.0;
    Wdummyinvn = 60.0;
    Wdummyinvp = 80.0;
    Wfainvn = 10.0;
	Wfainvp = 20.0;
	Waddrnandn = 50.0;
	Waddrnandp = 50.0;
	Wfanandn = 20.0;
    Wfanandp = 30.0;
    Wfanorn = 5.0;
	Wfanorp = 10.0;
    Wdecnandn = 10.0;
    Wdecnandp = 30.0;
    Wcompn = 10.0;
    Wcompp = 30.0;
    Wmuxdrv12n = 60.0;
    Wmuxdrv12p = 100.0;
    Wsenseextdrv1p = 80.0;
    Wsenseextdrv1n = 40.0;
    Wsenseextdrv2p = 240.0;
	Wsenseextdrv2n = 160.0;
    krise = 0.4e-9;
    tsensedata = 5.8e-10;
    psensedata = 0.02e-9;
    tsensescale = 0.02e-10;
    tsensetag = 2.6e-10;
    psensetag = 0.016e-9;
    tfalldata = 7e-10;
    tfalltag = 7e-10;
    BitWidth = 7.746*0.8;
    BitHeight = 2*7.746*0.8;
    Cout = 0.5e-12;
	Widthptondiff = 3.2;
    Widthtrack = 3.2*0.8;
    Widthcontact = 1.6;
    Wpoly = 0.8;
    ptocontact = 0.4;
    stitch_ramv = 6.0;
    BitHeight1x1 = 2*7.746*0.8;
    stitch_ramh = 12.0;
    BitWidth1x1 = 7.746*0.8;
    WidthNOR1 = 11.6;
    WidthNOR2 = 13.6;
    WidthNOR3 = 20.8;
    WidthNOR4 = 28.8;
    WidthNOR5 = 34.4;
    WidthNOR6 = 41.6;
    Predec_height1 = 140.8;
    Predec_width1 = 270.4;
    Predec_height2 = 140.8;
    Predec_width2 = 539.2;
    Predec_height3 = 281.6;    
    Predec_width3 = 584.0;
    Predec_height4 = 281.6;  
    Predec_width4 = 628.8; 
    Predec_height5 = 422.4; 
    Predec_width5 = 673.6;
    Predec_height6 = 422.4;
    Predec_width6 = 718.4;
    Wwrite = 1.2;
    SenseampHeight = 152.0;
    OutdriveHeight = 200.0;
    FAOutdriveHeight = 229.2;
    FArowWidth = 382.8;
    CAM2x2Height_1p = 48.8;
    CAM2x2Width_1p = 44.8;
    CAM2x2Height_2p = 80.8;   
    CAM2x2Width_2p = 76.8;
    DatainvHeight = 25.6;
    Wbitdropv = 30.0;
    decNandWidth = 34.4;
    FArowNANDWidth = 71.2;
    FArowNOR_INVWidth = 28.0;

    FAHeightIncrPer_first_rw_or_w_port = 16.0;
    FAHeightIncrPer_later_rw_or_w_port = 16.0;
    FAHeightIncrPer_first_r_port = 12.0;
    FAHeightIncrPer_later_r_port = 12.0;
    FAWidthIncrPer_first_rw_or_w_port = 16.0;
    FAWidthIncrPer_later_rw_or_w_port = 9.6;
    FAWidthIncrPer_first_r_port = 12.0;
    FAWidthIncrPer_later_r_port = 9.6;
}

void Cacti4_2::init_tech_params(double technology) {

	double tech = technology * 1000.0;
	Tkelvin = 383.15;

	have_leakage_params = 1;

	//tech = (int) ceil(technology * 1000.0);

	if (tech < 181 && tech > 179) {
		process_tech = 0.18;
		tech_length0 = 180E-9;
		M0n =   3.5E-2;      /* Zero Bias Mobility for N-Type */
		M0p =  8.0E-3 ;      /* Zero Bias Mobility for P-Type */
		Tox = 3.5E-9;

		Cox = (Eox/Tox);     /* Gate Oxide Capacitance per unit area */
		Vnoff0 = 7.5E-2 ;    /* Empirically Determined Model Parameter for N-Type */
	                         /* FIX ME */
		Vpoff0 = -2.8e-2 ;   /* Empirically Determined Model Parameter for P-Type */
  	  	Nfix =  0.22;        /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.17 ;       /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */

  		Vthn =  0.3979 ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) (original) */
  		Vthp =  0.4659 ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) (original) */

  		Vnthx = -1.0E-3 ;    /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx =  -1.0E-3 ;   /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		Vdd_init=  2.0 ;     /* Default Vdd. Can be Changed in leakage.c */
  		Volt0 =  1.7 ;

  		Na    =  -1.5 ;      /* Empirical param for the Vdd fit */
  		Nb    =   1.2 ;      /* Empirical param for the Vdd fit */
  		Pa    =   5.0 ;      /* Empirical param for the Vdd fit */
  		Pb    =   0.75 ;     /* Empirical param for the Vdd fit */
  		NEta =  1.5 ;        /* Sub-threshold Swing Co-efficient N-Type */
  		PEta =  1.6  ;       /* Sub-threshold Swing Co-efficient P-Type */

		/* gate Vss */
  		Vth0_gate_vss= 0.65;
  		aspect_gate_vss= 5;

		/*drowsy cache*/
  		Vdd_low= 0.6;

		/*RBB*/
  		k1_body_n= 0.5613;
  		k1_body_p= 0.5560;
  		vfi = 0.6;

  		VSB_NMOS= 0.5;
  		VSB_PMOS= 0.5;

		/* dual VT*/
  		Vt_cell_nmos_high= 0.45  ;
  		Vt_cell_pmos_high= 0.5;
  		Vt_bit_nmos_low = 0.35;
  		Vt_bit_pmos_low = 0.4;

  		L_nmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
  		Tox_nmos_e  = 2.6E+9;  /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
  		Tox_pmos_e  = 2.6E+9;  /* Adjusting Factor for Tox */
	}

	/* TECH_POINT130nm */

	else if (tech < 131 && tech > 129)  {

		process_tech = 0.13;
  		tech_length0 = 130E-9;
  		M0n =  1.34E-2;      /* Zero Bias Mobility for N-Type */
  		M0p =  5.2E-3 ;      /* Zero Bias Mobility for P-Type */
  		//Tox =  3.3E-9;
  		Tox = 2.52731e-09;
  		Cox =  (Eox/Tox);    /* Gate Oxide Capacitance per unit area */
  		//Vnoff0 = -6.2E-2 ;   /* Empirically Determined Model Parameter for N-Type */
  		Vnoff0 = -1.68E-1 ;   /* Empirically Determined Model Parameter for N-Type */
  		//Vpoff0 = -0.1;       /* Empirically Determined Model Parameter for P-Type */
  		Vpoff0 = -0.28;       /* Empirically Determined Model Parameter for P-Type */
  		Nfix =   0.16 ;      /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.13 ;       /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vthn =  0.3353;      /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Vthp =  0.3499 ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vnthx = -0.85E-3;    /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx = -0.1E-3;     /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		//Vdd_init =  1.5 ;    /* Default Vdd. */
  		//Vdd_init =  1.08 ;    /* Default Vdd. */
		Vdd_init = 1.3;
  		Volt0 =  1.2;

  		Na  =    3.275 ;     /* Empirical param for the Vdd fit */
  		Nb  =     1.1 ;      /* Empirical param for the Vdd fit */
  		Pa  =     4.65 ;     /* Empirical param for the Vdd fit */
  		Pb  =     2.2 ;      /* Empirical param for the Vdd fit */
  		NEta =  1.6 ;        /* Sub-threshold Swing Co-efficient N-Type */
  		PEta = 1.8   ;       /* Sub-threshold Swing Co-efficient P-Type */


		/* gate Vss */
  		Vth0_gate_vss = 0.55;
  		aspect_gate_vss = 5;

		/*drowsy cache*/
  		Vdd_low =  0.45;

		/*RBB*/
  		k1_body_n = 0.3662;
  		k1_body_p = 0.4087;
  		vfi =  0.55;
  		VSB_NMOS = 0.45;
  		VSB_PMOS = 0.45;

		/* dual VT*/
  		Vt_cell_nmos_high = 0.38  ;
  		Vt_cell_pmos_high=  0.4;
  		Vt_bit_nmos_low = 0.28;
  		Vt_bit_pmos_low = 0.29;


  		L_nmos_d  = 0.285E+9;  /* Adjusting Factor for Length */
  		Tox_nmos_e  = 4.3E+9;  /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.44E+9;   /* Adjusting Factor for Length */
  		Tox_pmos_e  = 5.0E+9;  /* Adjusting Factor for Tox */
	}

	/* TECH_POINT100nm */

     else if (tech < 101 && tech > 99)  {

  		process_tech = 0.10;
  		tech_length0 = 100E-9;
  		M0n = 1.8E-2 ;      /* Zero Bias Mobility for N-Type */
  		M0p = 5.5E-3  ;     /* Zero Bias Mobility for P-Type */
  		Tox = 2.5E-9 ;
  		Cox = (Eox/Tox);    /* Gate Oxide Capacitance per unit area */
  		Vnoff0 = -2.7E-2;   /* Empirically Determined Model Parameter for N-Type */
  		                    /* FIX ME */
  		Vpoff0 = -1.04E-1;  /* Empirically Determined Model Parameter for P-Type */
  		Nfix  = 0.18  ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.14   ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vthn =  0.2607  ;   /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Vthp =  0.3030 ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vnthx = -0.77E-3 ;  /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx =  -0.72E-3;  /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		Vdd_init = 1.2 ;    /* Default Vdd. Can be Changed for different parts in leakage.c */
  		Volt0 =  1.0;
  		Na   =   3.6 ;      /* Empirical param for the Vdd fit */
  		Nb   =   2.5  ;     /* Empirical param for the Vdd fit */
  		Pa   =    4.49 ;    /* Empirical param for the Vdd fit */
  		Pb   =    2.4 ;     /* Empirical param for the Vdd fit */

  		NEta =  1.7 ;       /* Sub-threshold Swing Co-efficient N-Type */
  		PEta =  1.7  ;      /* Sub-threshold Swing Co-efficient P-Type */


		/* gate Vss */
  		Vth0_gate_vss = 0.50;
  		aspect_gate_vss = 5;

		/*drowsy cache*/
  		Vdd_low =  0.36;

		/*RBB*/
  		k1_body_n =  0.395;
  		k1_body_p =  0.391;
  		vfi = 0.5;
  		VSB_NMOS =  0.4;
  		VSB_PMOS =  0.4 ;

		/* dual VT*/
  		Vt_cell_nmos_high =  0.31  ;
  		Vt_cell_pmos_high =  0.35;
  		Vt_bit_nmos_low  = 0.21;
  		Vt_bit_pmos_low = 0.25;


  		L_nmos_d  = 0.102E+9;   /* Adjusting Factor for Length */
  		Tox_nmos_e  = 2.5E+9;   /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.21E+9;    /* Adjusting Factor for Length */
  		Tox_pmos_e  = 3.1E+9;   /* Adjusting Factor for Tox */

 	} else if (tech > 69 && tech < 71)  {

		process_tech = 0.07;
 		tech_length0 = 70E-9;
 		M0n =  3.5E-2 ;     /* Zero Bias Mobility for N-Type */
 		M0p = 8.0E-3 ;      /* Zero Bias Mobility for P-Type */
 		Tox = 1.0E-9;
 		Cox = (Eox/Tox) ;   /* Gate Oxide Capacitance per unit area */
 		Vnoff0 = -3.9E-2 ;  /* Empirically Determined Model Parameter for N-Type */
 		                    /* FIX ME */
 		Vpoff0 = -4.35E-2;  /* Empirically Determined Model Parameter for P-Type */
 		Nfix =  -0.05 ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
 		Pfix =  0.07 ;      /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
 		Vthn =  0.1902  ;   /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
 		Vthp =  0.2130 ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
 		Vnthx = -0.49E-3 ;  /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
 		Vpthx =  -0.54E-3;  /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
 		Vdd_init =  1.0;    /* Default Vdd. Can be Changed in leakage.c */
 		Volt0 =  0.8;
 		Na   =   -2.94 ;    /* Empirical param for the Vdd fit */
 		Nb   =    2.0  ;    /* Empirical param for the Vdd fit */
 		Pa   =    -2.245;   /* Empirical param for the Vdd fit */
 		Pb   =    2.89;     /* Empirical param for the Vdd fit */
 		NEta =  1.3;        /* Sub-threshold Swing Co-efficient N-Type */
 		PEta  = 1.4 ;       /* Sub-threshold Swing Co-efficient P-Type */

		/*gate leakage factor for 70nm*/
 		nmos_unit_leakage = 53e-12;
 		a_nmos_vdd = 7.75;
 		b_nmos_t =  0.15e-12;
 		c_nmos_tox =  11.75e9;

		pmos_unit_leakage = 22.9e-12;
 		a_pmos_vdd = 12;
 		b_pmos_t =  0.086e-12;
 		c_pmos_tox = 11.25e9;

		/* gate Vss */
 		Vth0_gate_vss =  0.45;
 		aspect_gate_vss =  5;

 	 	/*drowsy cache*/
 		Vdd_low =  0.3;

		/*RBB*/
 		k1_body_n = 0.37;
 		k1_body_p = 0.38;
 		vfi = 0.45;
 		VSB_NMOS = 0.35 ;
 		VSB_PMOS = 0.35;

		/* dual VT*/
 		Vt_cell_nmos_high =  0.29 ;
 		Vt_cell_pmos_high =  0.31;
 		Vt_bit_nmos_low = 0.19;
 		Vt_bit_pmos_low = 0.21;

 		L_nmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
 		Tox_nmos_e  = 3.0E+9;  /* Adjusting Factor for Tox */
 		L_pmos_d  = 0.225E+9;  /* Adjusting Factor for Length */
 		Tox_pmos_e  = 4.0E+9;  /* Adjusting Factor for Tox */

	}

	else {
		have_leakage_params = 0;
		//printf("\n ERROR: Technology parameters not known for %3.4f feature size \n    (Parameters available for 0.18u, 0.13, 0.10, and 0.07u technologies)\nExiting...\n\n");
		//exit(0);
	}

	//v4.1: Specifying all resistances, capacitances, device/wire dimensions in terms of input tech node.
      //Presently derived from original 0.8 micron process

	FUDGEFACTOR = 0.8/technology;//Since 0.8 micron is the base process presently

	FEATURESIZE = technology;
	
	WIRESPACING = (1.6*FEATURESIZE);
	WIREWIDTH = (1.6*FEATURESIZE);
	WIREPITCH = (WIRESPACING+WIREWIDTH);


	Cndiffarea = Cndiffarea * FUDGEFACTOR;//Since this is a F/micron2 capacitance, this will scale up by FUDGEFACTOR

	Cpdiffarea = Cpdiffarea * FUDGEFACTOR;//Since this is a F/micron2 capacitance, this will scale up by FUDGEFACTOR
	
      Cndiffside = Cndiffside;//Since this is a F/micron capacitance, this doesn't scale

	Cpdiffside = Cpdiffside;//Since this is a F/micron capacitance, this doesn't scale

	Cndiffovlp = Cndiffovlp;//Since this is a F/micron capacitance, this doesn't scale

	Cpdiffovlp = Cpdiffovlp;//Since this is a F/micron capacitance, this doesn't scale

      Cnoxideovlp = Cnoxideovlp;//Since this is a F/micron capacitance, this doesn't scale

	Cpoxideovlp = Cpoxideovlp;//Since this is a F/micron capacitance, this doesn't scale

      Leff = 0.8/FUDGEFACTOR;//Presently the tech parameters are based off a 0.8 micron process
	//If the technology node changes, then the 0.8 should be replaced appropriately. 

      inv_Leff = inv_Leff * FUDGEFACTOR;//Since this is inverse of transistor length
    
	Cgate = Cgate * FUDGEFACTOR;//Since this is a F/micron2 capacitance, this will scale up by FUDGEFACTOR

	Cgatepass = Cgatepass * FUDGEFACTOR;//Since this is a F/micron2 capacitance, this will scale up by FUDGEFACTOR

      Cpolywire = Cpolywire * FUDGEFACTOR;//Since this is a F/micron2 capacitance, this will scale up by FUDGEFACTOR		 

      Rnchannelstatic	= Rnchannelstatic / FUDGEFACTOR;//Since this is an ohm-micron resistance, this will scale down by FUDGEFACTOR

      Rpchannelstatic	= Rpchannelstatic / FUDGEFACTOR;//Since this is an ohm-micron resistance, this will scale down by FUDGEFACTOR

      Rnchannelon	= Rnchannelon / FUDGEFACTOR;//Since this is an ohm-micron resistance, this will scale down by FUDGEFACTOR

      Rpchannelon	= Rpchannelon / FUDGEFACTOR;//Since this is an ohm-micron resistance, this will scale down by FUDGEFACTOR

	Wdecdrivep = 450 * FEATURESIZE;//this was 360 micron for the 0.8 micron process

      Wdecdriven = 300 * FEATURESIZE;//this was 240 micron for the 0.8 micron process

	  Wworddrivemax = 125 * FEATURESIZE; //this was 100 micron for the 0.8 micron process

	Wmemcella = 1.125 * FEATURESIZE;//this was 0.9 micron for the 0.8 micron process

      Wmemcellpmos = 0.8125 * FEATURESIZE;//this was 0.65 micron for the 0.8 micron process

	Wmemcellnmos = 2.5 * FEATURESIZE;//this was 2 micron for the 0.8 micron process

	Wpchmax = 31.25 * FEATURESIZE;//this was 25 micron for the 0.8 micron process

	Wcompinvp1 = 12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process
	
	Wcompinvn1 = 7.5 * FEATURESIZE;//this was 6 micron for the 0.8 micron process

	Wcompinvp2 = 25 * FEATURESIZE;//this was 20 micron for the 0.8 micron process

	Wcompinvn2 = 15 * FEATURESIZE;//this was 12 micron for the 0.8 micron process

	Wcompinvp3 = 50 * FEATURESIZE;//this was 40 micron for the 0.8 micron process

	Wcompinvn3 = 30 * FEATURESIZE;//this was 24 micron for the 0.8 micron process
	
	Wevalinvp =  100 * FEATURESIZE;//this was 80 micron for the 0.8 micron process

	Wevalinvn = 50 * FEATURESIZE;//this was 40 micron for the 0.8 micron process

	Wfadriven = 62.5 * FEATURESIZE;//this was 50 micron for the 0.8 micron process

	Wfadrivep = 125 * FEATURESIZE;//this was 100 micron for the 0.8 micron process

	Wfadrive2n = 250 * FEATURESIZE;//this was 200 micron for the 0.8 micron process

	Wfadrive2p = 500 * FEATURESIZE;//this was 400 micron for the 0.8 micron process

	Wfadecdrive1n = 6.25 * FEATURESIZE;//this was 5 micron for the 0.8 micron process

	Wfadecdrive1p = 12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process

	Wfadecdrive2n = 25 * FEATURESIZE;//this was 20 micron for the 0.8 micron process

	Wfadecdrive2p = 50 * FEATURESIZE;//this was 40 micron for the 0.8 micron process
	
	Wfadecdriven =  62.5 * FEATURESIZE;//this was 50 micron for the 0.8 micron process

	Wfadecdrivep =  125 * FEATURESIZE;//this was 100 micron for the 0.8 micron process

	Wfaprechn =  7.5 * FEATURESIZE;//this was 6 micron for the 0.8 micron process

	Wfaprechp = 12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process

	Wdummyn =  12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process

	Wdummyinvn = 75 * FEATURESIZE;//this was 60 micron for the 0.8 micron process
	
	Wdummyinvp = 100 * FEATURESIZE;//this was 80 micron for the 0.8 micron process

	Wfainvn = 12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process
	
	Wfainvp = 25 * FEATURESIZE;//this was 20 micron for the 0.8 micron process
	
	Waddrnandn = 62.5 * FEATURESIZE;//this was 50 micron for the 0.8 micron process
	
	Waddrnandp = 62.5 * FEATURESIZE;//this was 50 micron for the 0.8 micron process
	
	Wfanandn = 25 * FEATURESIZE;//this was 20 micron for the 0.8 micron process
	
	Wfanandp = 37.5 * FEATURESIZE;//this was 30 micron for the 0.8 micron process

	Wfanorn = 6.25 * FEATURESIZE;//this was 5 micron for the 0.8 micron process
	
	Wfanorp = 12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process

	Wdecnandn =  12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process
	
	Wdecnandp =  37.5 * FEATURESIZE;//this was 30 micron for the 0.8 micron process

	Wcompn =  12.5 * FEATURESIZE;//this was 10 micron for the 0.8 micron process
	
	Wcompp =  37.5 * FEATURESIZE;//this was 30 micron for the 0.8 micron process

	Wmuxdrv12n = 75 * FEATURESIZE;//this was 60 micron for the 0.8 micron process
	
	Wmuxdrv12p = 125 * FEATURESIZE;//this was 100 micron for the 0.8 micron process

	Wsenseextdrv1p = 100 * FEATURESIZE;//this was 80 micron for the 0.8 micron process
 	
	Wsenseextdrv1n = 50 * FEATURESIZE;//this was 40 micron for the 0.8 micron process

	Wsenseextdrv2p =  300 * FEATURESIZE;//this was 240 micron for the 0.8 micron process
 	
	Wsenseextdrv2n =  200 * FEATURESIZE;//this was 160 micron for the 0.8 micron process

	krise = krise / FUDGEFACTOR; //this was krise for the 0.8 micron process

	tsensedata = tsensedata / FUDGEFACTOR; //this was tsensedata for the 0.8 micron process

//#define psensedata      (0.02e-9)
//#define tsensescale     0.02e-10
	
	tsensetag = tsensetag / FUDGEFACTOR; //this was tsensetag for the 0.8 micron process

//#define psensetag	(0.016e-9)

	tfalldata = tfalldata / FUDGEFACTOR;//this was tfalldata for the 0.8 micron process
	tfalltag = tfalltag / FUDGEFACTOR;//this was tfalltag for the 0.8 micron process

	BitWidth = 7.746*FEATURESIZE;//this was 7.746*0.8 for the 0.8 micron process

	BitHeight = 2*7.746*FEATURESIZE;//this was 2*7.746*0.8 for the 0.8 micron process

	Cout = Cout/ FUDGEFACTOR;//this was 0.5e-12 for the 0.8 micron process


	//From areadef.h; expressing all dimensions in terms of process feauture size

  Widthptondiff = 4*FEATURESIZE; //Was 3.2 micron for the 0.8 micron process

  Widthtrack = 3.2*FEATURESIZE; //Was 3.2*0.8 micron for the 0.8 micron process

  Widthcontact = 2*FEATURESIZE; //Was 1.6 micron for the 0.8 micron process
  Wpoly = FEATURESIZE;//Was 0.8 micron for the 0.8 micron process

  ptocontact = FEATURESIZE/2; //Was 0.4 micron for the 0.8 micron process
  stitch_ramv = 7.5*FEATURESIZE; //Was 6 micron for the 0.8 micron process 
//#define BitHeight16x2 33.6

  BitHeight1x1 = 2*7.746*FEATURESIZE; /* see below */ //Was 2*7.746*0.8 micron for the 0.8 micron process 
  stitch_ramh = 15*FEATURESIZE;  //Was 12 micron for the 0.8 micron process 
//#define BitWidth16x2  192.8
  BitWidth1x1 = 7.746*FEATURESIZE; //Was 7.746*0.8 micron for the 0.8 micron process 
/* dt: Assume that each 6-T SRAM cell is 120F^2 and has an aspect ratio of 1(width) to 2(height), than the width is 2*sqrt(60)*F */
  WidthNOR1 = 14.5*FEATURESIZE; //Was 11.6 micron for the 0.8 micron process 
  WidthNOR2 = 17*FEATURESIZE; //Was 13.6 micron for the 0.8 micron process 
  WidthNOR3 = 26*FEATURESIZE;  //Was 20.8 micron for the 0.8 micron process 
  WidthNOR4 = 36*FEATURESIZE;  //Was 28.8 micron for the 0.8 micron process 
  WidthNOR5 = 43*FEATURESIZE;  //Was 34.4 micron for the 0.8 micron process 
  WidthNOR6 = 52*FEATURESIZE; //Was 41.6 micron for the 0.8 micron process 
  Predec_height1 = 176*FEATURESIZE; //Was 140.8 micron for the 0.8 micron process
  Predec_width1 = 338*FEATURESIZE;  //Was 270.4 micron for the 0.8 micron process
  Predec_height2 = 176*FEATURESIZE; //Was 140.8 micron for the 0.8 micron process
  Predec_width2 = 674*FEATURESIZE; //Was 539.2 micron for the 0.8 micron process
  Predec_height3 = 352*FEATURESIZE;  //Was 281.6 micron for the 0.8 micron process    
  Predec_width3 = 730*FEATURESIZE; //Was 584 micron for the 0.8 micron process
  Predec_height4 = 352*FEATURESIZE; //Was 281.6 micron for the 0.8 micron process
  Predec_width4 = 786*FEATURESIZE; //Was 628.8 micron for the 0.8 micron process
  Predec_height5 = 528*FEATURESIZE; //Was 422.4 micron for the 0.8 micron process
  Predec_width5 = 842*FEATURESIZE; //Was 673.6 micron for the 0.8 micron process
  Predec_height6 = 528*FEATURESIZE; //Was 422.4 micron for the 0.8 micron process
  Predec_width6 = 898*FEATURESIZE; //Was 718.4 micron for the 0.8 micron process
  Wwrite = 1.5*FEATURESIZE; //Was 1.2 micron for the 0.8 micron process
  SenseampHeight = 190*FEATURESIZE; //Was 152 micron for the 0.8 micron process
  OutdriveHeight = 250*FEATURESIZE; //Was 200 micron for the 0.8 micron process
  FAOutdriveHeight = 286.5*FEATURESIZE; //Was 229.2 micron for the 0.8 micron process
  FArowWidth = 478.5*FEATURESIZE; //Was 382.8 micron for the 0.8 micron process
  CAM2x2Height_1p = 61*FEATURESIZE; //Was 48.8 micron for the 0.8 micron process
  CAM2x2Width_1p = 56*FEATURESIZE; //Was 44.8 micron for the 0.8 micron process
  CAM2x2Height_2p = 101*FEATURESIZE; //Was 80.8 micron for the 0.8 micron process
  CAM2x2Width_2p = 96*FEATURESIZE; //Was 76.8 micron for the 0.8 micron process
  DatainvHeight = 32*FEATURESIZE; //Was 25.6 micron for the 0.8 micron process
  Wbitdropv = 37.5*FEATURESIZE; //Was 30 micron for the 0.8 micron process
  decNandWidth = 43*FEATURESIZE; //Was 34.4 micron for the 0.8 micron process
  FArowNANDWidth = 89*FEATURESIZE; //Was 71.2 micron for the 0.8 micron process
  FArowNOR_INVWidth = 35*FEATURESIZE; //Was 28 micron for the 0.8 micron process


  FAHeightIncrPer_first_rw_or_w_port = 20*FEATURESIZE; //Was 16 micron for the 0.8 micron process
  FAHeightIncrPer_later_rw_or_w_port = 20*FEATURESIZE; //Was 16 micron for the 0.8 micron process
  FAHeightIncrPer_first_r_port = 15*FEATURESIZE; //Was 12 micron for the 0.8 micron process
  FAHeightIncrPer_later_r_port = 15*FEATURESIZE; //Was 12 micron for the 0.8 micron process
  FAWidthIncrPer_first_rw_or_w_port = 20*FEATURESIZE; //Was 16 micron for the 0.8 micron process
  FAWidthIncrPer_later_rw_or_w_port = 12*FEATURESIZE; //Was 9.6 micron for the 0.8 micron process
  FAWidthIncrPer_first_r_port = 15*FEATURESIZE; //Was 12 micron for the 0.8 micron process
  FAWidthIncrPer_later_r_port = 12*FEATURESIZE; //Was 9.6 micron for the 0.8 micron process



}



/* from time.c */

void Cacti4_2::reset_tag_device_widths() {

	 Wtdecdrivep_second= 0.0;
	 Wtdecdriven_second= 0.0;
	 Wtdecdrivep_first= 0.0;
	 Wtdecdriven_first= 0.0;
	 Wtdec3to8n = 0.0;
	 Wtdec3to8p = 0.0;
	 WtdecNORn  = 0.0;
	 WtdecNORp  = 0.0;
	 Wtdecinvn  = 0.0;
	 Wtdecinvp  = 0.0;
	 WtwlDrvn = 0.0;
	 WtwlDrvp = 0.0;

	 Wtbitpreequ= 0.0;
	 Wtiso= 0.0;
	 Wtpch= 0.0;
	 Wtiso= 0.0;
	 WtsenseEn= 0.0;
	 WtsenseN= 0.0;
	 WtsenseP= 0.0;
	 WtoBufN = 0.0;
	 WtoBufP = 0.0;
	 WtsPch= 0.0;

	 WtpchDrvp= 0.0; WtpchDrvn= 0.0;
	 WtisoDrvp= 0.0; WtisoDrvn= 0.0;
	 WtspchDrvp= 0.0; WtspchDrvn= 0.0;
	 WtsenseEnDrvp= 0.0; WtsenseEnDrvn= 0.0;

	 WtwrtMuxSelDrvn= 0.0;
	 WtwrtMuxSelDrvp= 0.0;
}
void Cacti4_2::reset_data_device_widths()
{
	 Waddrdrvn1 = 0.0;
	 Waddrdrvp1= 0.0;
	 Waddrdrvn2= 0.0;
	 Waddrdrvp2= 0.0;

	 Wdecdrivep_second = 0.0;
	 Wdecdriven_second = 0.0;
	 Wdecdrivep_first = 0.0;
	 Wdecdriven_first = 0.0;
	 Wdec3to8n = 0.0;
	 Wdec3to8p = 0.0;
	 WdecNORn  = 0.0;
	 WdecNORp  = 0.0;
	 Wdecinvn  = 0.0;
	 Wdecinvp  = 0.0;
	 WwlDrvn = 0.0;
	 WwlDrvp = 0.0;

	 

	 Wbitpreequ= 0.0;

	 Wiso = 0.0;
	 Wpch= 0.0;
	 Wiso= 0.0;
	 WsenseEn= 0.0;
	 WsenseN= 0.0;
	 WsenseP= 0.0;
	 WoBufN = 0.0;
	 WoBufP = 0.0;
	 WsPch= 0.0;

	 WpchDrvp= 0.0; WpchDrvn= 0.0;
	 WisoDrvp= 0.0; WisoDrvn= 0.0;
	 WspchDrvp= 0.0; WspchDrvn= 0.0;
	 WsenseEnDrvp= 0.0; WsenseEnDrvn= 0.0;

	 WwrtMuxSelDrvn= 0.0;
	 WwrtMuxSelDrvp= 0.0;
	 
	 WmuxdrvNANDn    = 0.0;
	 WmuxdrvNANDp    = 0.0;
	 WmuxdrvNORn	= 0.0;
	 WmuxdrvNORp	= 0.0;
	 Wmuxdrv3n	= 0.0;
	 Wmuxdrv3p	= 0.0;
	 Woutdrvseln	= 0.0;
	 Woutdrvselp	= 0.0;

	 Woutdrvnandn= 0.0;
	 Woutdrvnandp= 0.0;
	 Woutdrvnorn	= 0.0;
	 Woutdrvnorp	= 0.0;
	 Woutdrivern	= 0.0;
	 Woutdriverp	= 0.0;
}

void Cacti4_2::compute_device_widths(int C,int B,int A,int fullyassoc, int Ndwl,int Ndbl,double Nspd)
{
	int rows, cols, numstack,l_predec_nor_v,l_predec_nor_h;
	double desiredrisetime, Rpdrive, Cline, Cload;
	double effWdecNORn,effWdecNORp,effWdec3to8n,effWdec3to8p, wire_res, wire_cap;
	int l_outdrv_v,l_outdrv_h;
	//int rows_fa_subarray,cols_fa_subarray, tagbits;
	int tagbits;
	int horizontal_edge = 0;
	int nr_subarrays_left = 0, v_or_h = 0;
	int horizontal_step = 0, vertical_step = 0;
	int h_inv_predecode = 0, v_inv_predecode = 0;

	double previous_ndriveW = 0, previous_pdriveW = 0, current_ndriveW = 0, current_pdriveW = 0;
	int i;

    //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
	//rows = C/(CHUNKSIZE*B*A*Ndbl*Nspd);
	rows = (int) (C/(CHUNKSIZE*B*A*Ndbl*Nspd) + EPSILON);
	//cols = CHUNKSIZE*B*A*Nspd/Ndwl;
	cols = (int) (CHUNKSIZE*B*A*Nspd/Ndwl + EPSILON);
 
	// Wordline capacitance to determine the wordline driver size
	/* Use a first-order approx */
	desiredrisetime = krise*log((double)(cols))/2.0;
	/*dt: I'm changing this back to what's in CACTI (as opposed to eCacti), i.e. counting the short poly connection to the pass transistors*/
	Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ Cwordmetal)*cols;
	Rpdrive = desiredrisetime/(Cline*log(VSINV)*-1.0);
	WwlDrvp = restowidth(Rpdrive,PCH);
	if (WwlDrvp > Wworddrivemax) {
		WwlDrvp = Wworddrivemax;
	}

	/* Now that we have a reasonable psize, do the rest as before */
	/* If we keep the ratio the same as the tag wordline driver,
	   the threshold voltage will be close to VSINV */

	/* assuming that nsize is half the psize */
	WwlDrvn = WwlDrvp/2;

	// Size of wordline
	// Sizing ratio for optimal delay is 3-4.
	Wdecinvn = (WwlDrvp + WwlDrvn) * SizingRatio * 1/3;
	Wdecinvp = (WwlDrvp + WwlDrvn) * SizingRatio * 2/3;

	// determine size of nor and nand gates in the decoder

	// width of NOR driving decInv -
	// effective width (NORp + NORn = Cout/SizingRatio( FANOUT))
	// Cout = Wdecinvn + Wdecinvp; SizingRatio = 3;
	// nsize = effWidth/3; psize = 2*effWidth/3;

	numstack =
	       (int)ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
	if (numstack==0) numstack = 1;

	if (numstack>5) numstack = 5;

	effWdecNORn = (Wdecinvn + Wdecinvp)*SizingRatio/3;
	effWdecNORp = 2*(Wdecinvn + Wdecinvp)*SizingRatio/3;
	WdecNORn = effWdecNORn;
	WdecNORp = effWdecNORp * numstack;

	/* second stage: driving a bunch of nor gates with a nand */
	
	/*dt: The *8 is there because above we mysteriously divide the capacity in BYTES by the number of BITS per wordline */
	l_predec_nor_v = rows*CHUNKSIZE; 
	/*dt: If we follow the original drawings from the TR's, then there is almost no horizontal wires, only the poly for contacting
	the nor gates. The poly part we don't model right now */
	l_predec_nor_h = 0;
	
	//v4.1: Scaling the poly length to the input tech node.
	//Cline = gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0))*rows/8 +
	  			//GlobalCbitmetal*(l_predec_nor_v)+GlobalCwordmetal*(l_predec_nor_h);

	Cline = gatecap(WdecNORn+WdecNORp,((numstack*40 / FUDGEFACTOR)+20.0 / FUDGEFACTOR))*rows/8 +
	  			GlobalCbitmetal*(l_predec_nor_v)+GlobalCwordmetal*(l_predec_nor_h);

	Cload = Cline / gatecap(1.0,0.0);

	effWdec3to8n = Cload*SizingRatio/3;
	effWdec3to8p = 2*Cload*SizingRatio/3;

	Wdec3to8n = effWdec3to8n * 3; // nand3 gate
	Wdec3to8p = effWdec3to8p;

	// size of address drivers before decoders
	/* First stage: driving the decoders */
	//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
	//horizontal_edge = CHUNKSIZE*B*A*Nspd;
	horizontal_edge = (int) (CHUNKSIZE*B*A*Nspd + EPSILON);

	previous_ndriveW = Wdec3to8n;
	previous_pdriveW = Wdec3to8p;

	if(Ndwl*Ndbl==1 ) {
	    wire_cap = GlobalCwordmetal*horizontal_edge;
	    wire_res = 0.5*GlobalRwordmetal*horizontal_edge;
		Cdectreesegments[0] = GlobalCwordmetal*horizontal_edge;
		Rdectreesegments[0] = 0.5*GlobalRwordmetal*horizontal_edge;

		Cline = 4*gatecap(previous_ndriveW+previous_pdriveW,10.0 / FUDGEFACTOR)+GlobalCwordmetal*horizontal_edge;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		nr_dectreesegments = 0;
	}
	else if(Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
	    wire_cap = GlobalCwordmetal*horizontal_edge;
	    wire_res = 0.5*GlobalRwordmetal*horizontal_edge;
		Cdectreesegments[0] = GlobalCwordmetal*horizontal_edge;
		Rdectreesegments[0] = 0.5*GlobalRwordmetal*horizontal_edge;

		Cline = 4*gatecap(previous_ndriveW+previous_pdriveW,10.0 / FUDGEFACTOR)+GlobalCwordmetal*horizontal_edge;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		nr_dectreesegments = 0;
	}
	else {
		/*dt: For the critical path  in an H-Tree, the metal */

		nr_subarrays_left = Ndwl* Ndbl;
		/*all the wires go to quads of subarrays where they get predecoded*/
		nr_subarrays_left /= 4;
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//horizontal_step = CHUNKSIZE*B*A*Nspd/Ndwl;
		horizontal_step = (int) (CHUNKSIZE*B*A*Nspd/Ndwl + EPSILON);
		//vertical_step = C/(B*A*Ndbl*Nspd);
		vertical_step = (int) (C/(B*A*Ndbl*Nspd) + EPSILON);
		h_inv_predecode = horizontal_step;

		Cdectreesegments[0] = GlobalCwordmetal*horizontal_step;
		Rdectreesegments[0] = 0.5*GlobalRwordmetal*horizontal_step;
		Cline = 4*gatecap(previous_ndriveW+previous_pdriveW,10.0 / FUDGEFACTOR)+GlobalCwordmetal*horizontal_step;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;
		WdecdrivetreeN[0] = current_ndriveW;
		nr_dectreesegments = 1;

		horizontal_step *= 2;
		v_or_h = 1; // next step is vertical
		
		while(nr_subarrays_left > 1) {
			previous_ndriveW = current_ndriveW;
			previous_pdriveW = current_pdriveW;
			nr_dectreesegments++;
			if(v_or_h) {
				v_inv_predecode += vertical_step;
				Cdectreesegments[nr_dectreesegments-1] = GlobalCbitmetal*vertical_step;
				Rdectreesegments[nr_dectreesegments-1] = 0.5*GlobalRbitmetal*vertical_step;
				Cline = gatecap(previous_ndriveW+previous_pdriveW,0)+GlobalCbitmetal*vertical_step;
				v_or_h = 0;
				vertical_step *= 2;
				nr_subarrays_left /= 2;
			}
			else {
				h_inv_predecode += horizontal_step;
				Cdectreesegments[nr_dectreesegments-1] = GlobalCwordmetal*horizontal_step;
				Rdectreesegments[nr_dectreesegments-1] = 0.5*GlobalRwordmetal*horizontal_step;
				Cline = gatecap(previous_ndriveW+previous_pdriveW,0)+GlobalCwordmetal*horizontal_step;
				v_or_h = 1;
				horizontal_step *= 2;
				nr_subarrays_left /= 2;
			}
			Cload = Cline / gatecap(1.0,0.0);

			current_ndriveW = Cload*SizingRatio/3;
			current_pdriveW = 2*Cload*SizingRatio/3;

			WdecdrivetreeN[nr_dectreesegments-1] = current_ndriveW;
		}

		if(nr_dectreesegments >= 10) {
			printf("Too many segments in the data decoder H-tree. Overflowing the preallocated array!");
			exit(1);
		}
		wire_cap = GlobalCbitmetal*v_inv_predecode + GlobalCwordmetal*h_inv_predecode;
		wire_res = 0.5*(GlobalRbitmetal*v_inv_predecode + GlobalRwordmetal*h_inv_predecode);


	}

	Wdecdriven_second = current_ndriveW;
	Wdecdrivep_second = current_pdriveW;

	// Size of second driver

	Wdecdriven_first = (Wdecdriven_second + Wdecdrivep_second)*SizingRatio/3;
	Wdecdrivep_first = 2*(Wdecdriven_second + Wdecdrivep_second)*SizingRatio/3;

	// these are the widths of the devices of dataoutput devices
	// will be used in the data_senseamplifier_data and dataoutput_data functions

	l_outdrv_v = 0;
	l_outdrv_h = 0;

	if(!fullyassoc) {

		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//rows =  (C/(B*A*Ndbl*Nspd));
		//cols = (CHUNKSIZE*B*A*Nspd/Ndwl);
		rows =  (int) (C/(B*A*Ndbl*Nspd) + EPSILON);
		cols = (int) (CHUNKSIZE*B*A*Nspd/Ndwl + EPSILON);
	}
	else {
		rows = (C/(B*Ndbl));
		if(!force_tag) {
    		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)B);
			tagbits = ADDRESS_BITS + EXTRA_TAG_BITS-(int)(logtwo((double)B) + EPSILON);
		}
		else {
			tagbits = force_tag_size;
		}
        cols = (CHUNKSIZE*B)+tagbits;
	}

	    /* calculate some layout info */
		

	if(Ndwl*Ndbl==1) {
	    l_outdrv_v= 0;
	    l_outdrv_h= cols;

		Coutdrvtreesegments[0] = GlobalCwordmetal*cols;
		Routdrvtreesegments[0] = 0.5*GlobalRwordmetal*cols;

		Cline = gatecap(Wsenseextdrv1n+Wsenseextdrv1p,10.0 / FUDGEFACTOR)+GlobalCwordmetal*cols;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		nr_outdrvtreesegments = 0;
	}
	else if(Ndwl*Ndbl==2) {
	    l_outdrv_v= 0;
	    l_outdrv_h= 2*cols;

		Coutdrvtreesegments[0] = GlobalCwordmetal*2*cols;
		Routdrvtreesegments[0] = 0.5*GlobalRwordmetal*2*cols;

		Cline = gatecap(Wsenseextdrv1n+Wsenseextdrv1p,10.0/ FUDGEFACTOR)+GlobalCwordmetal*2*cols;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		nr_outdrvtreesegments = 0;
	}
	else if(Ndwl*Ndbl>2) {
		nr_subarrays_left = Ndwl* Ndbl;
		nr_subarrays_left /= 2;
		/*dt: assuming the sense amps are in the middle of each subarray */
		horizontal_step = cols/2;
		vertical_step = rows/2;
		l_outdrv_h = horizontal_step;

		Coutdrvtreesegments[0] = GlobalCwordmetal*horizontal_step;
		Routdrvtreesegments[0] = 0.5*GlobalRwordmetal*horizontal_step;
		nr_outdrvtreesegments = 1;

		horizontal_step *= 2;
		v_or_h = 1; // next step is vertical
		
		while(nr_subarrays_left > 1) {
			nr_outdrvtreesegments++;
			if(v_or_h) {
				l_outdrv_v += vertical_step;

				Coutdrvtreesegments[nr_outdrvtreesegments-1] = GlobalCbitmetal*vertical_step;
				Routdrvtreesegments[nr_outdrvtreesegments-1] = 0.5*GlobalRbitmetal*vertical_step;

				v_or_h = 0;
				vertical_step *= 2;
				nr_subarrays_left /= 2;
			}
			else {
				l_outdrv_h += horizontal_step;

				Coutdrvtreesegments[nr_outdrvtreesegments-1] = GlobalCwordmetal*horizontal_step;
				Routdrvtreesegments[nr_outdrvtreesegments-1] = 0.5*GlobalRwordmetal*horizontal_step;

				v_or_h = 1;
				horizontal_step *= 2;
				nr_subarrays_left /= 2;
			}
		}

		/*dt: Now that we have all the H-tree segments for the output tree, 
		we can walk it in reverse and calc the gate widths*/

		previous_ndriveW = Wsenseextdrv1n;
		previous_pdriveW = Wsenseextdrv1p;
		for(i = nr_outdrvtreesegments-1;i>0;i--) {
			Cline = gatecap(previous_ndriveW+previous_pdriveW,0)+Coutdrvtreesegments[i];
			Cload = Cline / gatecap(1.0,0.0);

			current_ndriveW = Cload*SizingRatio/3;
			current_pdriveW = 2*Cload*SizingRatio/3;

			WoutdrvtreeN[i] = current_ndriveW;

			previous_ndriveW = current_ndriveW;
			previous_pdriveW = current_pdriveW;
		}
	}
	
  	if(nr_outdrvtreesegments >= 20) {
			printf("Too many segments in the output H-tree. Overflowing the preallocated array!");
			exit(1);
		}
	// typical width of gate considered for the estimating val of draincap.
	Cload = gatecap(previous_ndriveW+previous_pdriveW,0) + Coutdrvtreesegments[0] + 
		   (draincap(5.0/ FUDGEFACTOR,NCH,1)+draincap(5.0/ FUDGEFACTOR,PCH,1))*A*muxover;

	Woutdrivern = 	(Cload/gatecap(1.0,0.0))*SizingRatio/3;
	Woutdriverp = 	(Cload/gatecap(1.0,0.0))*SizingRatio*2/3;

	// eff load for nor gate = gatecap(drv_p);
	// factor of 2 is needed to account for series nmos transistors in nor2
	Woutdrvnorp = 2*Woutdriverp*SizingRatio*2/3;
	Woutdrvnorn = Woutdriverp*SizingRatio/3;

	// factor of 2 is needed to account for series nmos transistors in nand2
	Woutdrvnandp = Woutdrivern*SizingRatio*2/3;
	Woutdrvnandn = 2*Woutdrivern*SizingRatio/3;

	Woutdrvselp = (Woutdrvnandp + Woutdrvnandn) * SizingRatio*2/3;
	Woutdrvseln = (Woutdrvnandp + Woutdrvnandn) * SizingRatio/3;

}


void Cacti4_2::compute_tag_device_widths(int C,int B,int A,int Ntspd,int Ntwl,int Ntbl,double NSubbanks)
{

	int rows,cols, tagbits, numstack,l_predec_nor_v,l_predec_nor_h;
	double Cline, Cload, Rpdrive,desiredrisetime;
	double effWtdecNORn,effWtdecNORp,effWtdec3to8n,effWtdec3to8p, wire_res, wire_cap;
	int horizontal_edge = 0;
	int nr_subarrays_left = 0, v_or_h = 0;
	int horizontal_step = 0, vertical_step = 0;
	int h_inv_predecode = 0, v_inv_predecode = 0;

	double previous_ndriveW = 0, previous_pdriveW = 0, current_ndriveW = 0, current_pdriveW = 0;

	rows = C/(CHUNKSIZE*B*A*Ntbl*Ntspd);
	if(!force_tag) {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
		//the final int value is the correct one 
		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
	}
	else {
		tagbits = force_tag_size;
	}
	cols = tagbits * A * Ntspd/Ntwl;

    // capacitive load on the wordline - C_int + C_memCellLoad * NCells
	Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
			 gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ TagCwordmetal)*cols;

	/*dt: changing the calculations for the tag wordline to be the same as for the data wordline*/
	/* Use a first-order approx */
	desiredrisetime = krise*log((double)(cols))/2.0;

	Rpdrive = desiredrisetime/(Cline*log(VSINV)*-1.0);
	WtwlDrvp = restowidth(Rpdrive,PCH);
	if (WtwlDrvp > Wworddrivemax) {
		WtwlDrvp = Wworddrivemax;
	}

	WtwlDrvn = WtwlDrvp/2;

	Wtdecinvn = (WtwlDrvn + WtwlDrvp)*SizingRatio*1/3;
	Wtdecinvp = (WtwlDrvn + WtwlDrvp)*SizingRatio*2/3;

	// determine widths of nand, nor gates in the tag decoder
	// width of NOR driving decInv -
	// effective width (NORp + NORn = Cout/SizingRatio( FANOUT))
	// Cout = Wdecinvn + Wdecinvp; SizingRatio = 3;
	// nsize = effWidth/3; psize = 2*effWidth/3;
	numstack =
	    (int)ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
	if (numstack==0) numstack = 1;
	if (numstack>5) numstack = 5;

	effWtdecNORn = (Wtdecinvn + Wtdecinvp)*SizingRatio/3;
	effWtdecNORp = 2*(Wtdecinvn + Wtdecinvp)*SizingRatio/3;
	WtdecNORn = effWtdecNORn;
	WtdecNORp = effWtdecNORp * numstack;

	/*dt: The *8 is there because above we mysteriously divide the capacity in BYTES by the number of BITS per wordline */
	l_predec_nor_v = rows*8; 
	/*dt: If we follow the original drawings from the TR's, then there is almost no horizontal wires, only the poly for contacting
	the nor gates. The poly part we don't model right now */
	l_predec_nor_h = 0;

	// find width of the nand gates in the 3-8 decoders
	Cline = gatecap(WtdecNORn+WtdecNORp,((numstack*40)/ FUDGEFACTOR + 20.0 / FUDGEFACTOR))*rows/8 +
           GlobalCbitmetal*(l_predec_nor_v) + GlobalCwordmetal*(l_predec_nor_h);

	Cload = Cline / gatecap(1.0,0.0);

    effWtdec3to8n = Cload*SizingRatio/3;
	effWtdec3to8p = 2*Cload*SizingRatio/3;

	Wtdec3to8n = effWtdec3to8n * 3; // nand3 gate
	Wtdec3to8p = effWtdec3to8p;

	horizontal_edge = cols*Ntwl;

	previous_ndriveW = Wtdec3to8n;
	previous_pdriveW = Wtdec3to8p;

    if(Ntwl*Ntbl==1 ) {
        wire_cap = GlobalCwordmetal*horizontal_edge;
        wire_res = 0.5*GlobalRwordmetal*horizontal_edge;

		Ctdectreesegments[0] = GlobalCwordmetal*horizontal_edge;
		Rtdectreesegments[0] = 0.5*GlobalRwordmetal*horizontal_edge;

		Cline = 4*gatecap(previous_ndriveW+previous_pdriveW,10.0 / FUDGEFACTOR)+GlobalCwordmetal*horizontal_edge;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		nr_tdectreesegments = 0;
    }
    else if(Ntwl*Ntbl==2 || Ntwl*Ntbl==4) {
        wire_cap = GlobalCwordmetal*0.5*horizontal_edge;
        wire_res = 0.5*GlobalRwordmetal*0.5*horizontal_edge;

		Ctdectreesegments[0] = GlobalCwordmetal*horizontal_edge;
		Rtdectreesegments[0] = 0.5*GlobalRwordmetal*horizontal_edge;

		Cline = 4*gatecap(previous_ndriveW+previous_pdriveW,10.0 / FUDGEFACTOR)+GlobalCwordmetal*horizontal_edge;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		nr_tdectreesegments = 0;
    }
	else {
		nr_subarrays_left = Ntwl*Ntbl;
		nr_subarrays_left /= 4;
		horizontal_step = cols;
		vertical_step = C/(B*A*Ntbl*Ntspd);
		h_inv_predecode = horizontal_step;

		Ctdectreesegments[0] = GlobalCwordmetal*horizontal_step;
		Rtdectreesegments[0] = 0.5*GlobalRwordmetal*horizontal_step;
		Cline = 4*gatecap(previous_ndriveW+previous_pdriveW,10.0 / FUDGEFACTOR)+GlobalCwordmetal*horizontal_step;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;
		WtdecdrivetreeN[0] = current_ndriveW;
		nr_tdectreesegments = 1;

		horizontal_step *= 2;
		v_or_h = 1; // next step is vertical
		
		while(nr_subarrays_left > 1) {
			previous_ndriveW = current_ndriveW;
			previous_pdriveW = current_pdriveW;
			nr_tdectreesegments++;
			if(v_or_h) {
				v_inv_predecode += vertical_step;
				Ctdectreesegments[nr_tdectreesegments-1] = GlobalCbitmetal*vertical_step;
				Rtdectreesegments[nr_tdectreesegments-1] = 0.5*GlobalRbitmetal*vertical_step;
				Cline = gatecap(previous_ndriveW+previous_pdriveW,0)+GlobalCbitmetal*vertical_step;
				v_or_h = 0;
				vertical_step *= 2;
				nr_subarrays_left /= 2;
			}
			else {
				h_inv_predecode += horizontal_step;
				Ctdectreesegments[nr_tdectreesegments-1] = GlobalCwordmetal*horizontal_step;
				Rtdectreesegments[nr_tdectreesegments-1] = 0.5*GlobalRwordmetal*horizontal_step;
				Cline = gatecap(previous_ndriveW+previous_pdriveW,0)+GlobalCwordmetal*horizontal_step;
				v_or_h = 1;
				horizontal_step *= 2;
				nr_subarrays_left /= 2;
			}
			Cload = Cline / gatecap(1.0,0.0);

			current_ndriveW = Cload*SizingRatio/3;
			current_pdriveW = 2*Cload*SizingRatio/3;

			WtdecdrivetreeN[nr_tdectreesegments-1] = current_ndriveW;
		}

		if(nr_tdectreesegments >= 10) {
			printf("Too many segments in the tag decoder H-tree. Overflowing the preallocated array!");
			exit(1);
		}

		wire_cap = GlobalCbitmetal*v_inv_predecode + GlobalCwordmetal*h_inv_predecode;
		wire_res = 0.5*(GlobalRbitmetal*v_inv_predecode + GlobalRwordmetal*h_inv_predecode);
	}

	Cline = 4*gatecap(Wtdec3to8n+Wtdec3to8p,10.0 / FUDGEFACTOR) + wire_cap;
	Cload = Cline / gatecap(1.0,0.0);

	Wtdecdriven_second = current_ndriveW;
	Wtdecdrivep_second = current_pdriveW;

	// Size of second driver

	Wtdecdriven_first = (Wtdecdriven_second + Wtdecdrivep_second)*SizingRatio/3;
	Wtdecdrivep_first = 2*(Wtdecdriven_second + Wtdecdrivep_second)*SizingRatio/3;

}
double Cacti4_2::cmos_ileakage(double nWidth, double pWidth,
					 double nVthresh_dual, double nVthreshold, double pVthresh_dual, double pVthreshold) {
	double leakage = 0.0;
	static int valid_cache = 0;
	static double cached_nmos_thresh = 0;
	static double cached_pmos_thresh = 0;

	static double norm_nmos_leakage = 0;
	static double norm_pmos_leakage = 0;

	if(have_leakage_params) {

		if (dualVt == TRUE) {
			
			if((cached_nmos_thresh == nVthresh_dual) && (cached_pmos_thresh == pVthresh_dual) && valid_cache) {
				leakage = nWidth*norm_nmos_leakage + pWidth*norm_pmos_leakage;
			}
			else {
				leakage = simplified_cmos_leakage(nWidth*inv_Leff,pWidth*inv_Leff,nVthresh_dual,pVthresh_dual,&norm_nmos_leakage,&norm_pmos_leakage);
				cached_nmos_thresh = nVthresh_dual;
				cached_pmos_thresh = pVthresh_dual;
				norm_nmos_leakage = inv_Leff*norm_nmos_leakage;
				norm_pmos_leakage = inv_Leff*norm_pmos_leakage;
				valid_cache = 1;
			}
		}
		else {
			
			if((cached_nmos_thresh == nVthreshold) && (cached_pmos_thresh == pVthreshold) && valid_cache) {
				leakage = nWidth*norm_nmos_leakage + pWidth*norm_pmos_leakage;
			}
			else {
				leakage = simplified_cmos_leakage(nWidth*inv_Leff,pWidth*inv_Leff,nVthreshold,pVthreshold,&norm_nmos_leakage,&norm_pmos_leakage);
				cached_nmos_thresh = nVthreshold;
				cached_pmos_thresh = pVthreshold;
				norm_nmos_leakage = inv_Leff*norm_nmos_leakage;
				norm_pmos_leakage = inv_Leff*norm_pmos_leakage;
				valid_cache = 1;
			}
		}
	}
	else {
		leakage = 0;
	}
	return leakage;
}
void Cacti4_2::reset_powerDef(powerDef *power) {
	power->readOp.dynamic = 0.0;
	power->readOp.leakage = 0.0;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = 0.0;
}
void Cacti4_2::mult_powerDef(powerDef *power, int val) {
	power->readOp.dynamic *= val;
	power->readOp.leakage *= val;

	power->writeOp.dynamic *= val;
	power->writeOp.leakage *= val;
}
void Cacti4_2::mac_powerDef(powerDef *sum,powerDef *mult, int val) {
	sum->readOp.dynamic += mult->readOp.dynamic*val;
	sum->readOp.leakage += mult->readOp.leakage*val;

	sum->writeOp.dynamic += mult->writeOp.dynamic*val;
	sum->writeOp.leakage += mult->writeOp.leakage*val;
}
void Cacti4_2::copy_powerDef(powerDef *dest, powerDef source) {
	dest->readOp.dynamic = source.readOp.dynamic;
	dest->readOp.leakage = source.readOp.leakage;

	dest->writeOp.dynamic = source.writeOp.dynamic;
	dest->writeOp.leakage = source.writeOp.leakage;
}
void Cacti4_2::copy_and_div_powerDef(powerDef *dest, powerDef source, double val) {

	// only dynamic power needs to be scaled.
	dest->readOp.dynamic = source.readOp.dynamic / val;
	dest->readOp.leakage = source.readOp.leakage;

	dest->writeOp.dynamic = source.writeOp.dynamic / val;
	dest->writeOp.leakage = source.writeOp.leakage;
}

void Cacti4_2::add_powerDef(powerDef *sum, powerDef a, powerDef b) {

	sum->readOp.dynamic = a.readOp.dynamic + b.readOp.dynamic;
	sum->readOp.leakage = a.readOp.leakage + b.readOp.leakage;

	sum->writeOp.dynamic = a.writeOp.dynamic + b.writeOp.dynamic;
	sum->writeOp.leakage = a.writeOp.leakage + b.writeOp.leakage;
}

double Cacti4_2::objective_function(double delay_weight, double area_weight, double power_weight,
						  double delay,double area,double power)
{
   return
      (double)(area_weight*area + delay_weight*delay + power_weight*power);
}



/*======================================================================*/



/* 
 * This part of the code contains routines for each section as
 * described in the tech report.  See the tech report for more details
 * and explanations */

/*----------------------------------------------------------------------*/

void Cacti4_2::subbank_routing_length (int C,int B,int A,
						char fullyassoc,
						int Ndbl,double Nspd,int Ndwl,int Ntbl,int Ntwl,int Ntspd,
						double NSubbanks,
						double *subbank_v,double *subbank_h)
{
  double htree;
  int htree_int, tagbits;
  int cols_data_subarray, rows_data_subarray, cols_tag_subarray,
    rows_tag_subarray;
  double inter_v, inter_h, sub_h, sub_v;
  int inter_subbanks;
  int cols_fa_subarray, rows_fa_subarray;

  if (!fullyassoc)
    {

	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
      //cols_data_subarray = (8 * B * A * Nspd / Ndwl);
      //rows_data_subarray = (C / (B * A * Ndbl * Nspd));
      cols_data_subarray = (int) ((8 * B * A * Nspd / Ndwl) + EPSILON);
      rows_data_subarray = (int) (C / (B * A * Ndbl * Nspd) + EPSILON);

      if (Ndwl * Ndbl == 1)
	{
	  sub_v = rows_data_subarray;
	  sub_h = cols_data_subarray;
	}
      else if (Ndwl * Ndbl == 2)
	{
	  sub_v = rows_data_subarray;
	  sub_h = 2 * cols_data_subarray;
	}
      else if (Ndwl * Ndbl > 2)
	{
	  htree = logtwo ((double) (Ndwl * Ndbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ndwl * Ndbl) * rows_data_subarray;
	      sub_h = sqrt (Ndwl * Ndbl) * cols_data_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ndwl * Ndbl / 2) * rows_data_subarray;
	      sub_h = 2 * sqrt (Ndwl * Ndbl / 2) * cols_data_subarray;
	    }
	}
      inter_v = sub_v;
      inter_h = sub_h;

      rows_tag_subarray = C / (B * A * Ntbl * Ntspd);
	  if(!force_tag) {
	    //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
		//the final int value is the correct one 
		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) + 
		  //(int) logtwo ((double) A) - (int) (logtwo (NSubbanks));
		tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) + 
		  (int) logtwo ((double) A) - (int) (logtwo (NSubbanks)) + EPSILON);
	  }
	  else {
		  tagbits = force_tag_size;
	  }

      cols_tag_subarray = tagbits * A * Ntspd / Ntwl;

      if (Ntwl * Ntbl == 1)
	{
	  sub_v = rows_tag_subarray;
	  sub_h = cols_tag_subarray;
	}
      if (Ntwl * Ntbl == 2)
	{
	  sub_v = rows_tag_subarray;
	  sub_h = 2 * cols_tag_subarray;
	}

      if (Ntwl * Ntbl > 2)
	{
	  htree = logtwo ((double) (Ntwl * Ntbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ntwl * Ntbl) * rows_tag_subarray;
	      sub_h = sqrt (Ntwl * Ntbl) * cols_tag_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ntwl * Ntbl / 2) * rows_tag_subarray;
	      sub_h = 2 * sqrt (Ntwl * Ntbl / 2) * cols_tag_subarray;
	    }
	}


      inter_v = MAX (sub_v, inter_v);
      inter_h += sub_h;

      sub_v = 0;
      sub_h = 0;

      if (NSubbanks == 1.0 || NSubbanks == 2.0)
	{
	  sub_h = 0;
	  sub_v = 0;
	}
      if (NSubbanks == 4.0)
	{
	  sub_h = 0;
	  sub_v = inter_v;
	}

      inter_subbanks = (int)NSubbanks;

      while ((inter_subbanks > 2) && (NSubbanks > 4))
	{

	  sub_v += inter_v;
	  sub_h += inter_h;

	  inter_v = 2 * inter_v;
	  inter_h = 2 * inter_h;
	  inter_subbanks = inter_subbanks / 4;

	  if (inter_subbanks == 4.0)
	    {
	      inter_h = 0;
	    }

	}
      *subbank_v = sub_v;
      *subbank_h = sub_h;
    }
  else
    {
      rows_fa_subarray = (C / (B * Ndbl));
	  if(!force_tag) {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) B);
		tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) (logtwo ((double) B) + EPSILON);
	  }
	  else {
		  tagbits = force_tag_size;
	  }
      cols_fa_subarray = (8 * B) + tagbits;

      if (Ndbl == 1)
	{
	  sub_v = rows_fa_subarray;
	  sub_h = cols_fa_subarray;
	}
      if (Ndbl == 2)
	{
	  sub_v = rows_fa_subarray;
	  sub_h = 2 * cols_fa_subarray;
	}

      if (Ndbl > 2)
	{
	  htree = logtwo ((double) (Ndbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ndbl) * rows_fa_subarray;
	      sub_h = sqrt (Ndbl) * cols_fa_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ndbl / 2) * rows_fa_subarray;
	      sub_h = 2 * sqrt (Ndbl / 2) * cols_fa_subarray;
	    }
	}
      inter_v = sub_v;
      inter_h = sub_h;

      sub_v = 0;
      sub_h = 0;

      if (NSubbanks == 1.0 || NSubbanks == 2.0)
	{
	  sub_h = 0;
	  sub_v = 0;
	}
      if (NSubbanks == 4.0)
	{
	  sub_h = 0;
	  sub_v = inter_v;
	}

      inter_subbanks = (int)NSubbanks;

      while ((inter_subbanks > 2) && (NSubbanks > 4))
	{

	  sub_v += inter_v;
	  sub_h += inter_h;

	  inter_v = 2 * inter_v;
	  inter_h = 2 * inter_h;
	  inter_subbanks = inter_subbanks / 4;

	  if (inter_subbanks == 4.0)
	    {
	      inter_h = 0;
	    }

	}
      *subbank_v = sub_v;
      *subbank_h = sub_h;
    }
}

void Cacti4_2::subbank_dim (int C,int B,int A,
			 char fullyassoc,
			 int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,
			 double NSubbanks,
			 double *subbank_h,double *subbank_v)
{
  double htree;
  int htree_int, tagbits;
  int cols_data_subarray, rows_data_subarray, cols_tag_subarray,
    rows_tag_subarray;
  double sub_h, sub_v, inter_v, inter_h;
  int cols_fa_subarray, rows_fa_subarray;

  if (!fullyassoc)
    {

      /* calculation of subbank dimensions */
      //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
      //cols_data_subarray = (8 * B * A * Nspd / Ndwl);
      //rows_data_subarray = (C / (B * A * Ndbl * Nspd));
	  cols_data_subarray = (int)(8 * B * A * Nspd / Ndwl + EPSILON);
      rows_data_subarray = (int) (C / (B * A * Ndbl * Nspd) + EPSILON);

      if (Ndwl * Ndbl == 1)
	{
	  sub_v = rows_data_subarray;
	  sub_h = cols_data_subarray;
	}
      if (Ndwl * Ndbl == 2)
	{
	  sub_v = rows_data_subarray;
	  sub_h = 2 * cols_data_subarray;
	}
      if (Ndwl * Ndbl > 2)
	{
	  htree = logtwo ((double) (Ndwl * Ndbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ndwl * Ndbl) * rows_data_subarray;
	      sub_h = sqrt (Ndwl * Ndbl) * cols_data_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ndwl * Ndbl / 2) * rows_data_subarray;
	      sub_h = 2 * sqrt (Ndwl * Ndbl / 2) * cols_data_subarray;
	    }
	}
      inter_v = sub_v;
      inter_h = sub_h;

      rows_tag_subarray = C / (B * A * Ntbl * Ntspd);

	  if(!force_tag) {
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
	  //the final int value is the correct one 
      //tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) +
				//(int) logtwo ((double) A) - (int) (logtwo (NSubbanks));
	   tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) +
				(int) logtwo ((double) A) - (int) (logtwo (NSubbanks)) + EPSILON);
	  }
	  else {
		  tagbits = force_tag_size;
	  }
      cols_tag_subarray = tagbits * A * Ntspd / Ntwl;

      if (Ntwl * Ntbl == 1)
	{
	  sub_v = rows_tag_subarray;
	  sub_h = cols_tag_subarray;
	}
      if (Ntwl * Ntbl == 2)
	{
	  sub_v = rows_tag_subarray;
	  sub_h = 2 * cols_tag_subarray;
	}

      if (Ntwl * Ntbl > 2)
	{
	  htree = logtwo ((double) (Ntwl * Ntbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ntwl * Ntbl) * rows_tag_subarray;
	      sub_h = sqrt (Ntwl * Ntbl) * cols_tag_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ntwl * Ntbl / 2) * rows_tag_subarray;
	      sub_h = 2 * sqrt (Ntwl * Ntbl / 2) * cols_tag_subarray;
	    }
	}

      inter_v = MAX (sub_v, inter_v);
      inter_h += sub_h;

      *subbank_v = inter_v;
      *subbank_h = inter_h;
    }

  else
    {
      rows_fa_subarray = (C / (B * Ndbl));
	  if(!force_tag) {
		 //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
		 //the final int value is the correct one 
		 //tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) B);
		 tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) (logtwo ((double) B) + EPSILON);
	  }
	  else {
		  tagbits = force_tag_size;
	  }
      cols_fa_subarray = (8 * B) + tagbits;

      if (Ndbl == 1)
	{
	  sub_v = rows_fa_subarray;
	  sub_h = cols_fa_subarray;
	}
      if (Ndbl == 2)
	{
	  sub_v = rows_fa_subarray;
	  sub_h = 2 * cols_fa_subarray;
	}

      if (Ndbl > 2)
	{
	  htree = logtwo ((double) (Ndbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ndbl) * rows_fa_subarray;
	      sub_h = sqrt (Ndbl) * cols_fa_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ndbl / 2) * rows_fa_subarray;
	      sub_h = 2 * sqrt (Ndbl / 2) * cols_fa_subarray;
	    }
	}
      inter_v = sub_v;
      inter_h = sub_h;

      *subbank_v = inter_v;
      *subbank_h = inter_h;
    }
}


void Cacti4_2::subbanks_routing_power (char fullyassoc,
						int A,
						double NSubbanks,
						double *subbank_h,double *subbank_v,powerDef *power)
{
  double Ceq, Ceq_outdrv;
  int i, blocks, htree_int, subbank_mod;
  double htree, wire_cap, wire_cap_outdrv;
  double start_h, start_v, line_h, line_v;
  double dynPower = 0.0;

  //*power = 0.0;

  if (fullyassoc)
    {
      A = 1;
    }

  if (NSubbanks == 1.0 || NSubbanks == 2.0)
    {
      //*power = 0.0;
	  power->writeOp.dynamic = 0.0;
	  power->writeOp.leakage = 0.0;

	  power->readOp.dynamic = 0.0;
	  power->readOp.leakage = 0.0;
    }

  if (NSubbanks == 4.0)
    {
      /* calculation of address routing power */
      wire_cap = GlobalCbitmetal * (*subbank_v);
      Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
	gatecap (Wdecdrivep + Wdecdriven, 0.0);
      dynPower += 2 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;
      Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
	wire_cap;
      dynPower += 2 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;

      /* calculation of out driver power */
      wire_cap_outdrv = Cbitmetal * (*subbank_v);
      Ceq_outdrv =
	draincap (Wsenseextdrv1p, PCH, 1) + draincap (Wsenseextdrv1n, NCH,
						      1) +
	gatecap (Wsenseextdrv2n + Wsenseextdrv2p, 10.0 / FUDGEFACTOR);
      dynPower += 2 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;
      Ceq_outdrv =
	draincap (Wsenseextdrv2p, PCH, 1) + draincap (Wsenseextdrv2n, NCH,
						      1) + wire_cap_outdrv;
      dynPower += 2 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;

    }

  if (NSubbanks == 8.0)
    {
      wire_cap = GlobalCbitmetal * (*subbank_v) + GlobalCwordmetal * (*subbank_h);
      /* buffer stage */
      Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
	gatecap (Wdecdrivep + Wdecdriven, 0.0);
      dynPower += 6 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;
      Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
	wire_cap;
      dynPower += 4 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;
      Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
	(wire_cap - Cbitmetal * (*subbank_v));
      dynPower += 2 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;

      /* calculation of out driver power */
      wire_cap_outdrv = Cbitmetal * (*subbank_v) + Cwordmetal * (*subbank_h);
      Ceq_outdrv =
	draincap (Wsenseextdrv1p, PCH, 1) + draincap (Wsenseextdrv1n, NCH,
						      1) +
	gatecap (Wsenseextdrv2n + Wsenseextdrv2p, 10.0 / FUDGEFACTOR);
      dynPower += 6 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;
      Ceq_outdrv =
	draincap (Wsenseextdrv2p, PCH, 1) + draincap (Wsenseextdrv2n, NCH,
						      1) + wire_cap_outdrv;
      dynPower += 4 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;
      Ceq_outdrv =
	draincap (Wsenseextdrv2p, PCH, 1) + draincap (Wsenseextdrv2n, NCH,
						      1) + (wire_cap_outdrv -
							    Cbitmetal *
							    (*subbank_v));
      dynPower += 2 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;

    }

  if (NSubbanks > 8.0)
    {
      blocks = (int) (NSubbanks / 8.0);
      htree = logtwo ((double) (blocks));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
      //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
      if (htree_int % 2 == 0)
	{
	  subbank_mod = htree_int;
	  start_h =
	    (*subbank_h * (powers (2, ((int) (logtwo (htree))) + 1) - 1));
	  start_v = *subbank_v;
	}
      else
	{
	  subbank_mod = htree_int - 1;
	  start_h = (*subbank_h * (powers (2, (htree_int + 1) / 2) - 1));
	  start_v = *subbank_v;
	}

      if (subbank_mod == 0)
	{
	  subbank_mod = 1;
	}

      line_v = start_v;
      line_h = start_h;

      for (i = 1; i <= blocks; i++)
	{
	  wire_cap = line_v * GlobalCbitmetal + line_h * GlobalCwordmetal;

	  Ceq =
	    draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH,
						      1) +
	    gatecap (Wdecdrivep + Wdecdriven, 0.0);
	  dynPower += 6 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;
	  Ceq =
	    draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH,
						      1) + wire_cap;
	  dynPower += 4 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;
	  Ceq =
	    draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH,
						      1) + (wire_cap -
							    Cbitmetal *
							    (*subbank_v));
	  dynPower += 2 * ADDRESS_BITS * Ceq * .5 * VddPow * VddPow;

	  /* calculation of out driver power */
	  wire_cap_outdrv = line_v * GlobalCbitmetal + line_h * GlobalCwordmetal;

	  Ceq_outdrv =
	    draincap (Wsenseextdrv1p, PCH, 1) + draincap (Wsenseextdrv1n, NCH,
							  1) +
	    gatecap (Wsenseextdrv2n + Wsenseextdrv2p, 10.0 / FUDGEFACTOR);
	  dynPower +=
	    6 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;
	  Ceq_outdrv =
	    draincap (Wsenseextdrv2p, PCH, 1) + draincap (Wsenseextdrv2n, NCH,
							  1) +
	    wire_cap_outdrv;
	  dynPower +=
	    4 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;
	  Ceq_outdrv =
	    draincap (Wsenseextdrv2p, PCH, 1) + draincap (Wsenseextdrv2n, NCH,
							  1) +
	    (wire_cap_outdrv - Cbitmetal * (*subbank_v));
	  dynPower +=
	    2 * Ceq_outdrv * VddPow * VddPow * .5 * BITOUT * A * muxover;

	  if (i % subbank_mod == 0)
	    {
	      line_v += 2 * (*subbank_v);
	    }
	}
    }

	power->writeOp.dynamic = dynPower;
	/*dt: still have to add leakage current*/
	power->readOp.dynamic = dynPower;
	/*dt: still have to add leakage current*/
}

double Cacti4_2::address_routing_delay (int C,int B,int A,
					   char fullyassoc,
					   int Ndwl,int Ndbl,double Nspd,int Ntwl,int Ntbl,int Ntspd,
					   double *NSubbanks,double *outrisetime,powerDef *power)
{
	double Ceq,tf,nextinputtime,delay_stage1,delay_stage2;
    double addr_h,addr_v;
    double wire_cap, wire_res;
    double lkgCurrent = 0.0;
    double dynEnergy = 0.0;
    double Cline, Cload;
	//powerDef *thisPower;

  /* Calculate rise time.  Consider two inverters */

  Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
    gatecap (Wdecdrivep + Wdecdriven, 0.0);
  tf = Ceq * transreson (Wdecdriven, NCH, 1);
  nextinputtime = horowitz (0.0, tf, VTHINV360x240, VTHINV360x240, FALL) /
    (VTHINV360x240);

  Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
    gatecap (Wdecdrivep + Wdecdriven, 0.0);
  tf = Ceq * transreson (Wdecdriven, NCH, 1);
  nextinputtime = horowitz (nextinputtime, tf, VTHINV360x240, VTHINV360x240,
			    RISE) / (1.0 - VTHINV360x240);
  addr_h = 0;
  addr_v = 0;
  subbank_routing_length (C, B, A, fullyassoc, Ndbl, Nspd, Ndwl, Ntbl, Ntwl,
			  Ntspd, *NSubbanks, &addr_v, &addr_h);
  wire_cap = GlobalCbitmetal * addr_v + addr_h * GlobalCwordmetal;
  wire_res = (GlobalRwordmetal * addr_h + GlobalRbitmetal * addr_v) / 2.0;

  /* buffer stage */
  /*dt: added gate width calc and leakage from eCACTI */
    Cline = *NSubbanks * ( gatecap(Wdecdrivep_first + Wdecdriven_first,0.0) + gatecap(Wtdecdrivep_first + Wtdecdriven_first,0.0) ) + wire_cap;
	Cload = Cline / gatecap(1.0,0.0);
	Waddrdrvn1 = Cload * SizingRatio /3;
	Waddrdrvp1 = Cload * SizingRatio * 2/3;

	Waddrdrvn2 = (Waddrdrvn1 + Waddrdrvp1) * SizingRatio * 1/3 ;
	Waddrdrvp2 = (Waddrdrvn1 + Waddrdrvp1) * SizingRatio * 2/3 ;

    Ceq = draincap(Waddrdrvp2,PCH,1)+draincap(Waddrdrvn2,NCH,1) +
    		gatecap(Waddrdrvn1+Waddrdrvp1,0.0);
    tf = Ceq*transreson(Waddrdrvn2,NCH,1);
	delay_stage1 = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,FALL);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,FALL)/(VTHINV360x240);

    dynEnergy = ADDRESS_BITS*Ceq*.5*VddPow*VddPow;

	lkgCurrent += ADDRESS_BITS*0.5*cmos_ileakage(Waddrdrvn2,Waddrdrvp2,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

    Ceq = draincap(Waddrdrvp1,PCH,1)+draincap(Waddrdrvn1,NCH,1) + wire_cap
    		+ *NSubbanks * (gatecap(Wdecdrivep_first+Wdecdriven_first,0.0) + gatecap(Wtdecdrivep_first+Wtdecdriven_first,0.0));
    tf = Ceq*(transreson(Waddrdrvn1,NCH,1)+wire_res);
	delay_stage2=horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,RISE);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,RISE)/(1.0-VTHINV360x240);

    dynEnergy += ADDRESS_BITS*Ceq*.5*VddPow*VddPow;

   	lkgCurrent += ADDRESS_BITS*0.5*cmos_ileakage(Waddrdrvn1,Waddrdrvp1,Vthn,Vt_bit_nmos_low,Vt_bit_pmos_low,Vthp);

	*outrisetime = nextinputtime;

	power->readOp.dynamic = dynEnergy;
	power->readOp.leakage = lkgCurrent * VddPow;

	// power for write op same as read op for address routing
	power->writeOp.dynamic = dynEnergy;
	power->writeOp.leakage = lkgCurrent * VddPow;

	return(delay_stage1+delay_stage2);

}



/* Decoder delay:  (see section 6.1 of tech report) */

/*dt: this is integrated from eCACTI. But we use want Energy per operation, not some measure of power, so no FREQ. */
double Cacti4_2::decoder_delay(int C, int B,int A,int Ndwl,int Ndbl,double Nspd,double NSubbanks,
			double *Tdecdrive,double *Tdecoder1,double *Tdecoder2,double inrisetime,double *outrisetime, int *nor_inputs,powerDef *power)
{
  	int numstack, Nact;
    //double Ceq,Req,Rwire,tf,nextinputtime,vth,tstep;
	double Ceq,Req,Rwire,tf,nextinputtime,vth;
  	int l_predec_nor_v, l_predec_nor_h,rows,cols;
	//double wire_cap, wire_res;
	double lkgCurrent = 0.0, dynPower = 0.0;
	int horizontal_edge = 0;
	int nr_subarrays_left = 0, v_or_h = 0;
	int horizontal_step = 0, vertical_step = 0;
	int h_inv_predecode = 0, v_inv_predecode = 0;
	int addr_bits_routed;
	int i;
	double this_delay;

	//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
	//int addr_bits=(int)logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd)));
	//addr_bits_routed = (int)logtwo( (double)((double)C/(double)B));
	//int addr_bits=(int)logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd)));
	//addr_bits_routed = (int)logtwo( (double)((double)C/(double)B));

	int addr_bits=(int) (logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))) + EPSILON);
	addr_bits_routed = (int) (logtwo( (double)((double)C/(double)B)) + EPSILON);

	//rows = C/(8*B*A*Ndbl*Nspd);
	//cols = CHUNKSIZE*B*A*Nspd/Ndwl;

	rows = (int) (C/(8*B*A*Ndbl*Nspd) + EPSILON);
	cols = (int) (CHUNKSIZE*B*A*Nspd/Ndwl + EPSILON);

	numstack =
	      (int) ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
	if (numstack==0) numstack = 1;

	if (numstack>5) numstack = 5;
	
	/*dt: The *8 is there because above we mysteriously divide the capacity in BYTES by the number of BITS per wordline */
	l_predec_nor_v = rows*8; 
	/*dt: If we follow the original drawings from the TR's, then there is almost no horizontal wires, only the poly for contacting
	the nor gates. The poly part we don't model right now */
	l_predec_nor_h = 0;

	
    /* Calculate rise time.  Consider two inverters */


    if (NSubbanks > 2) {
		Ceq = draincap(Waddrdrvp1,PCH,1)+draincap(Waddrdrvn1,NCH,1) +
    			gatecap(Wdecdrivep_first+Wdecdriven_first,0.0);

    	tf = Ceq*transreson(Waddrdrvn1,NCH,1);
    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	}
	else {
		Ceq = draincap(Wdecdrivep_first,PCH,1)+draincap(Wdecdriven_first,NCH,1) +
    			gatecap(Wdecdrivep_first+Wdecdriven_first,0.0);

    	tf = Ceq*transreson(Wdecdriven_first,NCH,1);
    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	}


	lkgCurrent += addr_bits_routed*0.5*cmos_ileakage(Wdecdriven_first,Wdecdrivep_first,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp)
		*1.0/(Ndwl*Ndbl);

	*Tdecdrive = 0;

	/*dt: the first inverter driving a bigger inverter*/
    Ceq = draincap(Wdecdrivep_first,PCH,1)+draincap(Wdecdriven_first,NCH,1) +
    		  gatecap(Wdecdrivep_second+Wdecdriven_second,0.0);

    tf = Ceq*transreson(Wdecdriven_first,NCH,1);
	
     this_delay = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,RISE);

	*Tdecdrive += this_delay;
	inrisetime = this_delay/(1.0-VTHINV360x240);

	
	if(nr_dectreesegments) {
		Ceq = draincap(Wdecdrivep_second,PCH,1)+draincap(Wdecdriven_second,NCH,1) + 
			  gatecap(3*WdecdrivetreeN[nr_dectreesegments-1],0) + Cdectreesegments[nr_dectreesegments-1];
		Req = transreson(Wdecdriven_second,NCH,1) + Rdectreesegments[nr_dectreesegments-1]; 

		tf = Ceq*Req;
		this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);

		*Tdecdrive += this_delay;
		inrisetime = this_delay/(1.0-VTHINV360x240);

		dynPower+=addr_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += addr_bits_routed*0.5*cmos_ileakage(Wdecdriven_second,Wdecdrivep_second,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp)
			          *1.0/(Ndwl*Ndbl);
	}
	


	/*dt: doing all the H-tree segments*/
	
	for(i=nr_dectreesegments; i>2;i--) {
		/*dt: this too should alternate...*/
		Ceq = (Cdectreesegments[i-2] + draincap(2*WdecdrivetreeN[i-1],PCH,1)+ draincap(WdecdrivetreeN[i-1],NCH,1) + 
			  gatecap(3*WdecdrivetreeN[i-2],0.0));
		Req = (Rdectreesegments[i-2] + transreson(WdecdrivetreeN[i-1],NCH,1));
		tf = Req*Ceq;
		/*dt: This shouldn't be all falling, but interleaved. Have to fix that at some point.*/
        this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);
		*Tdecdrive += this_delay;
		inrisetime = this_delay/(1.0 - VTHINV360x240);

		dynPower+=addr_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += 1.0/(Ndwl*Ndbl)*pow(2,nr_dectreesegments - i)*addr_bits_routed*0.5*
			          cmos_ileakage(WdecdrivetreeN[i-1],2*WdecdrivetreeN[i-1],Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}

	if(nr_dectreesegments) {
		Ceq = 4*gatecap(Wdec3to8n+Wdec3to8p,10.0 /FUDGEFACTOR) + Cdectreesegments[0] + 
			    draincap(2*WdecdrivetreeN[0],PCH,1)+ draincap(WdecdrivetreeN[0],NCH,1); 
		Rwire = Rdectreesegments[0];
		tf = (Rwire + transreson(2*WdecdrivetreeN[0],PCH,1))*Ceq;

		dynPower+=addr_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += 1.0/(Ndwl*Ndbl)*pow(2,nr_dectreesegments)*addr_bits_routed*0.5*cmos_ileakage(WdecdrivetreeN[0],2*WdecdrivetreeN[0],Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}
	else {
		Ceq = 4*gatecap(Wdec3to8n+Wdec3to8p,10.0 /FUDGEFACTOR) + Cdectreesegments[0] + 
			    draincap(Wdecdrivep_second,PCH,1)+ draincap(Wdecdriven_second,NCH,1); 
		Rwire = Rdectreesegments[0];
		tf = (Rwire + transreson(Wdecdrivep_second,PCH,1))*Ceq;

		dynPower+=addr_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += 1.0/(Ndwl*Ndbl)*addr_bits_routed*0.5*cmos_ileakage(Wdecdriven_second,Wdecdrivep_second,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}

	// there are 8 nand gates in each 3-8 decoder. since these transistors are
	// stacked, we use a stacking factor of 1/5 (0.2). 0.5 signifies that we
	// are taking the average of both nmos and pmos transistors.

	

     this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHNAND60x120,FALL);
	 *Tdecdrive += this_delay;

	lkgCurrent += 8*0.2*0.5*cmos_ileakage(Wdec3to8n,Wdec3to8p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp)*
		// For the all the 3-8 decoders per quad:
		ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))))
		/*for all the quads*/
		*0.25;

    nextinputtime = this_delay/VTHNAND60x120;

    Ceq = 3*draincap(Wdec3to8p,PCH,1) + draincap(Wdec3to8n,NCH,3) +
	 			gatecap(WdecNORn+WdecNORp,((numstack*40 / FUDGEFACTOR)+20.0 / FUDGEFACTOR))*rows/8 +
	  			GlobalCbitmetal*(l_predec_nor_v)+ GlobalCwordmetal*(l_predec_nor_h);
    Rwire = GlobalRbitmetal*(l_predec_nor_v)/2 + GlobalRwordmetal*(l_predec_nor_h)/2;

	tf = Ceq*(Rwire+transreson(Wdec3to8n,NCH,3));

	// 0.2 is the stacking factor, 0.5 for averging of nmos and pmos leakage
	// and since there are rows number of nor gates:

	lkgCurrent += 0.5*0.2* rows * cmos_ileakage(WdecNORn,WdecNORp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp); 

	// number of active blocks among Ndwl modules
	if (Ndwl/Nspd < 1) {
		Nact = 1;
	}
	else {
		 //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
         //the final int value is the correct one 
		//Nact = Ndwl/Nspd;
		Nact = (int) (Ndwl/Nspd + EPSILON);
	}

	//dynPower+=Ndwl*Ndbl*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
	dynPower+=0.5*Nact*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack) {
       	case 1: vth = VTHNOR12x4x1; break;
       	case 2: vth = VTHNOR12x4x2; break;
       	case 3: vth = VTHNOR12x4x3; break;
       	case 4: vth = VTHNOR12x4x4; break;
       	case 5: vth = VTHNOR12x4x4; break;
	    default: printf("error:numstack=%d\n",numstack);
		printf("Cacti does not support a series stack of %d transistors !\n",numstack);
		exit(0);
		break;
	}

    *Tdecoder1 = horowitz(nextinputtime,tf,VTHNAND60x120,vth,RISE);

    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = transreson(WdecNORp,PCH,numstack);

    Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0 / FUDGEFACTOR)+
          numstack * draincap(WdecNORn,NCH,1)+draincap(WdecNORp,PCH,numstack));

	
	lkgCurrent += 0.5* rows * cmos_ileakage(Wdecinvn,Wdecinvp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
    tf = Req*Ceq;

    *Tdecoder2 = horowitz(nextinputtime,tf,vth,VSINV,FALL);

    *outrisetime = *Tdecoder2/(VSINV);
	*nor_inputs=numstack;
	dynPower+=Ceq*VddPow*VddPow;

	//printf("%g %g %g %d %d %d\n",*Tdecdrive,*Tdecoder1,*Tdecoder2,Ndwl, Ndbl,Nspd);

	//fprintf(stderr, "%f %f %f %f %d %d %d\n", (*Tdecdrive+*Tdecoder1+*Tdecoder2)*1e3, *Tdecdrive*1e3, *Tdecoder1*1e3, *Tdecoder2*1e3, Ndwl, Ndbl, Nspd);
	power->readOp.dynamic = dynPower;
	power->readOp.leakage = (lkgCurrent * VddPow) * Ndwl * Ndbl;

	power->writeOp.dynamic = dynPower;
	power->writeOp.leakage = (lkgCurrent * VddPow) * Ndwl * Ndbl;


    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}

/*----------------------------------------------------------------------*/

/* Decoder delay in the tag array (see section 6.1 of tech report) */
/*dt: incorporating leakage code from eCacti, see decoder_delay for more comments */
double Cacti4_2::decoder_tag_delay(int C, int B,int A,int Ntwl,int Ntbl, int Ntspd,double NSubbanks,
             double *Tdecdrive, double *Tdecoder1, double *Tdecoder2,double inrisetime,double *outrisetime, int *nor_inputs,powerDef *power)
{
    //double Ceq,Req,Rwire,tf,nextinputtime,vth,tstep;
	double Ceq,Req,Rwire,tf,nextinputtime,vth;
	int numstack,tagbits, Nact;
	int rows, cols;
	//int l_inv_predecode,l_predec_nor_v,l_predec_nor_h;
	int l_predec_nor_v,l_predec_nor_h;
	//double wire_cap, wire_res;
	double lkgCurrent=0.0, dynPower = 0.0;
	//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
	//int addr_bits=(int)logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd)));
	int addr_bits=(int) (logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))) + EPSILON);
	int horizontal_edge = 0;
	int nr_subarrays_left = 0, v_or_h = 0;
	int horizontal_step = 0, vertical_step = 0;
	int h_inv_predecode = 0, v_inv_predecode = 0;
	double this_delay;
	int i = 0;

	//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
	//the final int value is the correct one 
	//int routing_bits = (int)logtwo( (double)((double)C/(double)B));
	int routing_bits = (int) (logtwo( (double)((double)C/(double)B)) + EPSILON);
	int tag_bits_routed;

    rows = C/(8*B*A*Ntbl*Ntspd);
	if(!force_tag) {
		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
	}
	else {
		tagbits = force_tag_size;
	}
	tag_bits_routed = routing_bits + tagbits;
	

	cols = tagbits*A*Ntspd/Ntwl ;

	numstack =
	    (int)ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
	if (numstack==0) numstack = 1;
	if (numstack>5) numstack = 5;

	/*dt: see comments in compute_device_widths*/
	/*dt: The *8 is there because above we mysteriously divide the capacity in BYTES by the number of BITS per wordline */
	l_predec_nor_v = rows*8; 
	/*dt: If we follow the original drawings from the TR's, then there is almost no horizontal wires, only the poly for contacting
	the nor gates. The poly part we don't model right now */
	l_predec_nor_h = 0;

	
    /* Calculate rise time.  Consider two inverters */
    if (NSubbanks > 2) {
		Ceq = draincap(Waddrdrvp1,PCH,1)+draincap(Waddrdrvn1,NCH,1) +
		    		gatecap(Wtdecdrivep_first+Wtdecdriven_first,0.0);

    	tf = Ceq*transreson(Waddrdrvn1,NCH,1);

    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	} 
	else {
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		    		gatecap(Wtdecdrivep_first+Wtdecdriven_first,0.0);

    	tf = Ceq*transreson(Wdecdriven_first,NCH,1);

    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	}

	lkgCurrent = 0.5*cmos_ileakage(Wtdecdriven_first,Wtdecdrivep_first,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp)*1.0/(Ntwl*Ntbl);

    *Tdecdrive = 0;

	/*dt: the first inverter driving a bigger inverter*/
    Ceq = draincap(Wtdecdrivep_first,PCH,1)+draincap(Wtdecdriven_first,NCH,1) +
    		  gatecap(Wtdecdrivep_second+Wtdecdriven_second,0.0);

    tf = Ceq*transreson(Wtdecdriven_first,NCH,1);
	
     this_delay = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,RISE);

	*Tdecdrive += this_delay;
	inrisetime = this_delay/(1.0-VTHINV360x240);

	
	if(nr_tdectreesegments) {
		Ceq = draincap(Wtdecdrivep_second,PCH,1)+draincap(Wtdecdriven_second,NCH,1) + 
			  gatecap(3*WtdecdrivetreeN[nr_tdectreesegments-1],0) + Ctdectreesegments[nr_tdectreesegments-1];
		Req = transreson(Wtdecdriven_second,NCH,1) + Rtdectreesegments[nr_tdectreesegments-1]; 

		tf = Ceq*Req;
		this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);

		*Tdecdrive += this_delay;
		inrisetime = this_delay/(1.0-VTHINV360x240);

		dynPower+= tag_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += tag_bits_routed*0.5*cmos_ileakage(Wtdecdriven_second,Wtdecdrivep_second,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp)
			          *1.0/(Ntwl*Ntbl);
	}
	


	/*dt: doing all the H-tree segments*/
	
	for(i=nr_tdectreesegments; i>2;i--) {
		/*dt: this too should alternate...*/
		Ceq = (Ctdectreesegments[i-2] + draincap(2*WtdecdrivetreeN[i-1],PCH,1)+ draincap(WtdecdrivetreeN[i-1],NCH,1) + 
			  gatecap(3*WtdecdrivetreeN[i-2],0.0));
		Req = (Rtdectreesegments[i-2] + transreson(WtdecdrivetreeN[i-1],NCH,1));
		tf = Req*Ceq;
		/*dt: This shouldn't be all falling, but interleaved. Have to fix that at some point.*/
        this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);
		*Tdecdrive += this_delay;
		inrisetime = this_delay/(1.0 - VTHINV360x240);

		dynPower+= tag_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += 1.0/(Ntwl*Ntbl)*pow(2,nr_tdectreesegments - i)*tag_bits_routed*0.5*cmos_ileakage(WtdecdrivetreeN[i-1],2*WtdecdrivetreeN[i-1],Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

	}

	if(nr_tdectreesegments) {
		//v4.1: Change below, gatecap(Wtdec3to8n+Wdec3to8p,10.0) -> gatecap(Wtdec3to8n+Wtdec3to8p,10.0)
		//Ceq = 4*gatecap(Wtdec3to8n+Wdec3to8p,10.0) + Ctdectreesegments[0] + 
			    //draincap(2*WtdecdrivetreeN[0],PCH,1)+ draincap(WtdecdrivetreeN[0],NCH,1); 
		Ceq = 4*gatecap(Wtdec3to8n+Wtdec3to8p,10.0 / FUDGEFACTOR) + Ctdectreesegments[0] + 
			    draincap(2*WtdecdrivetreeN[0],PCH,1)+ draincap(WtdecdrivetreeN[0],NCH,1); 
		Rwire = Rtdectreesegments[0];
		tf = (Rwire + transreson(2*WtdecdrivetreeN[0],PCH,1))*Ceq;

		dynPower+= tag_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += 1.0/(Ntwl*Ntbl)*pow(2,nr_tdectreesegments)*tag_bits_routed*0.5*cmos_ileakage(WtdecdrivetreeN[0],2*WtdecdrivetreeN[0],Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}
	else {
		//v4.1: Change below, gatecap(Wtdec3to8n+Wdec3to8p,10.0) -> gatecap(Wtdec3to8n+Wtdec3to8p,10.0)
		//Ceq = 4*gatecap(Wtdec3to8n+Wdec3to8p,10.0) + Ctdectreesegments[0] + 
			    //draincap(Wtdecdrivep_second,PCH,1)+ draincap(Wtdecdriven_second,NCH,1); 
		Ceq = 4*gatecap(Wtdec3to8n+Wtdec3to8p,10.0 / FUDGEFACTOR) + Ctdectreesegments[0] + 
			    draincap(Wtdecdrivep_second,PCH,1)+ draincap(Wtdecdriven_second,NCH,1); 
		Rwire = Rtdectreesegments[0];
		tf = (Rwire + transreson(Wtdecdrivep_second,PCH,1))*Ceq;

		dynPower+= tag_bits_routed*Ceq*.5*VddPow*VddPow;
		lkgCurrent += 1.0/(Ntwl*Ntbl)*tag_bits_routed*0.5*cmos_ileakage(Wtdecdriven_second,Wtdecdrivep_second,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}
	 this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHNAND60x120,FALL);
	 *Tdecdrive += this_delay;
	 nextinputtime = this_delay/VTHNAND60x120;

    // there are 8 nand gates in each 3-8 decoder. since these transistors are
	// stacked, we use a stacking factor of 1/5 (0.2). 0.5 signifies that we
	// are taking the average of both nmos and pmos transistors.

	lkgCurrent += 8*0.2*0.5* cmos_ileakage(Wtdec3to8n,Wtdec3to8p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp)*
		// For the all the 3-8 decoders per quad:
		ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))))
		/*for all the quads*/
		*0.25;


    /* second stage: driving a bunch of nor gates with a nand */


    Ceq = 3*draincap(Wtdec3to8p,PCH,1) +draincap(Wtdec3to8n,NCH,3) +
           gatecap(WtdecNORn+WtdecNORp,((numstack*40) / FUDGEFACTOR + 20.0 / FUDGEFACTOR))*rows +
           GlobalCbitmetal*(l_predec_nor_v) + GlobalCwordmetal*(l_predec_nor_h);

    Rwire = GlobalRbitmetal*(l_predec_nor_v)/2 + GlobalRwordmetal*(l_predec_nor_h)/2;

    tf = Ceq*(Rwire+transreson(Wtdec3to8n,NCH,3));

    // 0.2 is the stacking factor, 0.5 for averging of nmos and pmos leakage
	// and since there are rows number of nor gates:

	lkgCurrent += 0.5*0.2* rows * cmos_ileakage(WtdecNORn,WtdecNORp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

	// Number of active blocks in Ntwl modules
	if (Ntwl/Ntspd < 1) {
		Nact = 1;
	}
	else {
		Nact = Ntwl/Ntspd;
	}

	dynPower+=0.5*Nact*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
	//dynPower+=Ntwl*Ntbl*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack) {
	    case 1: vth = VTHNOR12x4x1; break;
        case 2: vth = VTHNOR12x4x2; break;
        case 3: vth = VTHNOR12x4x3; break;
        case 4: vth = VTHNOR12x4x4; break;
        case 5: vth = VTHNOR12x4x4; break;
        case 6: vth = VTHNOR12x4x4; break;
        default: printf("error:numstack=%d\n",numstack);
                   printf("Cacti does not support a series stack of %d transistors !\n",numstack);
		exit(0);
        break;
	}

    *Tdecoder1 = horowitz(nextinputtime,tf,VTHNAND60x120,vth,RISE);
    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = transreson(WtdecNORp,PCH,numstack);
    Ceq = (gatecap(Wtdecinvn+Wtdecinvp,20.0 / FUDGEFACTOR)+numstack*draincap(WtdecNORn,NCH,1)+
               draincap(WtdecNORp,PCH,numstack));

    tf = Req*Ceq;
    *Tdecoder2 = horowitz(nextinputtime,tf,vth,VSINV,FALL);
    *outrisetime = *Tdecoder2/(VSINV);
	*nor_inputs=numstack;

	dynPower+=Ceq*VddPow*VddPow;

	lkgCurrent += 0.5* rows * cmos_ileakage(Wtdecinvn,Wtdecinvp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

	power->readOp.dynamic = dynPower;
	power->readOp.leakage = (lkgCurrent * VddPow) * Ntwl * Ntbl;

	power->writeOp.dynamic = dynPower;
	power->writeOp.leakage = (lkgCurrent * VddPow) * Ntwl * Ntbl;

    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}

/*dt: still have to add leakage and gate width calc to this function */
double Cacti4_2::fa_tag_delay (int C,int B,int Ntwl,int Ntbl,int Ntspd,
			  double *Tagdrive, double *Tag1, double *Tag2, double *Tag3, double *Tag4, double *Tag5, double *outrisetime,
			  int *nor_inputs,
			  powerDef *power)
{
  //double Ceq, Req, Rwire, rows, tf, nextinputtime, vth, tstep;
  double Ceq, Req, Rwire, rows, tf, nextinputtime;
  //int numstack;
  double Tagdrive1 = 0, Tagdrive2 = 0;
  double Tagdrive0a = 0, Tagdrive0b = 0;
  double TagdriveA = 0, TagdriveB = 0;
  double TagdriveA1 = 0, TagdriveB1 = 0;
  double TagdriveA2 = 0, TagdriveB2 = 0;
  
  double dynPower = 0.0;

  rows = C / (B * Ntbl);

  /* Calculate rise time.  Consider two inverters */

  Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
    gatecap (Wdecdrivep + Wdecdriven, 0.0);
  tf = Ceq * transreson (Wdecdriven, NCH, 1);
  nextinputtime = horowitz (0.0, tf, VTHINV360x240, VTHINV360x240, FALL) /
    (VTHINV360x240);

  Ceq = draincap (Wdecdrivep, PCH, 1) + draincap (Wdecdriven, NCH, 1) +
    gatecap (Wdecdrivep + Wdecdriven, 0.0);
  tf = Ceq * transreson (Wdecdrivep, PCH, 1);
  nextinputtime = horowitz (nextinputtime, tf, VTHINV360x240, VTHINV360x240,
			    RISE) / (1.0 - VTHINV360x240);

  // If tag bitline divisions, add extra driver

  if (Ntbl > 1)
    {
      Ceq = draincap (Wfadrivep, PCH, 1) + draincap (Wfadriven, NCH, 1) +
	gatecap (Wfadrive2p + Wfadrive2n, 0.0);
      tf = Ceq * transreson (Wfadriven, NCH, 1);
      TagdriveA = horowitz (nextinputtime, tf, VSINV, VSINV, FALL);
      nextinputtime = TagdriveA / (VSINV);
      dynPower += .5 * Ceq * VddPow * VddPow * ADDRESS_BITS;

      if (Ntbl <= 4)
	{
	  Ceq =
	    draincap (Wfadrive2p, PCH, 1) + draincap (Wfadrive2n, NCH,
						      1) +
	    gatecap (Wfadecdrive1p + Wfadecdrive1n,
		     10.0 / FUDGEFACTOR) * 2 + +FACwordmetal * sqrt ((rows +
							1) * Ntbl) / 2 +
	    FACbitmetal * sqrt ((rows + 1) * Ntbl) / 2;
	  Rwire =
	    FARwordmetal * sqrt ((rows + 1) * Ntbl) * .5 / 2 +
	    FARbitmetal * sqrt ((rows + 1) * Ntbl) * .5 / 2;
	  tf = Ceq * (transreson (Wfadrive2p, PCH, 1) + Rwire);
	  TagdriveB = horowitz (nextinputtime, tf, VSINV, VSINV, RISE);
	  nextinputtime = TagdriveB / (1.0 - VSINV);
	  dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * .5 * 2;
	}
      else
	{
	  Ceq =
	    draincap (Wfadrive2p, PCH, 1) + draincap (Wfadrive2n, NCH,
						      1) +
	    gatecap (Wfadrivep + Wfadriven,
		     10.0 / FUDGEFACTOR) * 2 + +FACwordmetal * sqrt ((rows +
							1) * Ntbl) / 2 +
	    FACbitmetal * sqrt ((rows + 1) * Ntbl) / 2;
	  Rwire =
	    FARwordmetal * sqrt ((rows + 1) * Ntbl) * .5 / 2 +
	    FARbitmetal * sqrt ((rows + 1) * Ntbl) * .5 / 2;
	  tf = Ceq * (transreson (Wfadrive2p, PCH, 1) + Rwire);
	  TagdriveB = horowitz (nextinputtime, tf, VSINV, VSINV, RISE);
	  nextinputtime = TagdriveB / (1.0 - VSINV);
	  dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * .5 * 4;

	  Ceq = draincap (Wfadrivep, PCH, 1) + draincap (Wfadriven, NCH, 1) +
	    gatecap (Wfadrive2p + Wfadrive2n, 10.0 / FUDGEFACTOR);
	  tf = Ceq * transreson (Wfadriven, NCH, 1);
	  TagdriveA1 = horowitz (nextinputtime, tf, VSINV, VSINV, FALL);
	  nextinputtime = TagdriveA1 / (VSINV);
	  dynPower += .5 * Ceq * VddPow * VddPow * ADDRESS_BITS;

	  if (Ntbl <= 16)
	    {
	      Ceq =
		draincap (Wfadrive2p, PCH, 1) + draincap (Wfadrive2n, NCH,
							  1) +
		gatecap (Wfadecdrive1p + Wfadecdrive1n,
			 10.0 / FUDGEFACTOR) * 2 + +FACwordmetal * sqrt ((rows +
							    1) * Ntbl) / 4 +
		FACbitmetal * sqrt ((rows + 1) * Ntbl) / 4;
	      Rwire =
		FARwordmetal * sqrt ((rows + 1) * Ntbl) * .5 / 4 +
		FARbitmetal * sqrt ((rows + 1) * Ntbl) * .5 / 4;
	      tf = Ceq * (transreson (Wfadrive2p, PCH, 1) + Rwire);
	      TagdriveB1 = horowitz (nextinputtime, tf, VSINV, VSINV, RISE);
	      nextinputtime = TagdriveB1 / (1.0 - VSINV);
	      dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * .5 * 8;
	    }
	  else
	    {
	      Ceq =
		draincap (Wfadrive2p, PCH, 1) + draincap (Wfadrive2n, NCH,
							  1) +
		gatecap (Wfadrivep + Wfadriven,
			 10.0 / FUDGEFACTOR) * 2 + +FACwordmetal * sqrt ((rows +
							    1) * Ntbl) / 4 +
		FACbitmetal * sqrt ((rows + 1) * Ntbl) / 4;
	      Rwire =
		FARwordmetal * sqrt ((rows + 1) * Ntbl) * .5 / 4 +
		FARbitmetal * sqrt ((rows + 1) * Ntbl) * .5 / 4;
	      tf = Ceq * (transreson (Wfadrive2p, PCH, 1) + Rwire);
	      TagdriveB1 = horowitz (nextinputtime, tf, VSINV, VSINV, RISE);
	      nextinputtime = TagdriveB1 / (1.0 - VSINV);
	      dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * .5 * 8;

	      Ceq =
		draincap (Wfadrivep, PCH, 1) + draincap (Wfadriven, NCH,
							 1) +
		gatecap (Wfadrive2p + Wfadrive2n, 10.0 / FUDGEFACTOR);
	      tf = Ceq * transreson (Wfadriven, NCH, 1);
	      TagdriveA2 = horowitz (nextinputtime, tf, VSINV, VSINV, FALL);
	      nextinputtime = TagdriveA2 / (VSINV);
	      dynPower += .5 * Ceq * VddPow * VddPow * ADDRESS_BITS;

	      Ceq =
		draincap (Wfadrive2p, PCH, 1) + draincap (Wfadrive2n, NCH,
							  1) +
		gatecap (Wfadecdrive1p + Wfadecdrive1n,
			 10.0 / FUDGEFACTOR) * 2 + +FACwordmetal * sqrt ((rows +
							    1) * Ntbl) / 8 +
		FACbitmetal * sqrt ((rows + 1) * Ntbl) / 8;
	      Rwire =
		FARwordmetal * sqrt ((rows + 1) * Ntbl) * .5 / 8 +
		FARbitmetal * sqrt ((rows + 1) * Ntbl) * .5 / 8;
	      tf = Ceq * (transreson (Wfadrive2p, PCH, 1) + Rwire);
	      TagdriveB2 = horowitz (nextinputtime, tf, VSINV, VSINV, RISE);
	      nextinputtime = TagdriveB2 / (1.0 - VSINV);
	      dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * .5 * 16;
	    }
	}
    }

  /* Two more inverters for enable delay */

  Ceq = draincap (Wfadecdrive1p, PCH, 1) + draincap (Wfadecdrive1n, NCH, 1)
    + gatecap (Wfadecdrive2p + Wfadecdrive2n, 0.0);
  tf = Ceq * transreson (Wfadecdrive1n, NCH, 1);
  Tagdrive0a = horowitz (nextinputtime, tf, VSINV, VSINV, FALL);
  nextinputtime = Tagdrive0a / (VSINV);
  dynPower += .5 * Ceq * VddPow * VddPow * ADDRESS_BITS * Ntbl;

  Ceq = draincap (Wfadecdrive2p, PCH, 1) + draincap (Wfadecdrive2n, NCH, 1) +
    +gatecap (Wfadecdrivep + Wfadecdriven, 10.0 / FUDGEFACTOR)
    + gatecap (Wfadecdrive2p + Wfadecdrive2n, 10.0 / FUDGEFACTOR);
  tf = Ceq * transreson (Wfadecdrive2p, PCH, 1);
  Tagdrive0b = horowitz (nextinputtime, tf, VSINV, VSINV, RISE);
  nextinputtime = Tagdrive0b / (VSINV);
  dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * .5 * Ntbl;

  /* First stage */

  Ceq = draincap (Wfadecdrive2p, PCH, 1) + draincap (Wfadecdrive2n, NCH, 1) +
    gatecap (Wfadecdrivep + Wfadecdriven, 10.0 / FUDGEFACTOR);
  tf = Ceq * transresswitch (Wfadecdrive2n, NCH, 1);
  Tagdrive1 = horowitz (nextinputtime, tf, VSINV, VTHFA1, FALL);
  nextinputtime = Tagdrive1 / VTHFA1;
  dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * .5 * Ntbl;

  Ceq = draincap (Wfadecdrivep, PCH, 2) + draincap (Wfadecdriven, NCH, 2)
    + draincap (Wfaprechn, NCH, 1)
    + gatecap (Wdummyn, 10.0 / FUDGEFACTOR) * (rows + 1) + FACbitmetal * (rows + 1);

  Rwire = FARbitmetal * (rows + 1) * .5;
  tf = (Rwire + transreson (Wfadecdrivep, PCH, 1) +
	transresswitch (Wfadecdrivep, PCH, 1)) * Ceq;
  Tagdrive2 = horowitz (nextinputtime, tf, VTHFA1, VTHFA2, RISE);
  nextinputtime = Tagdrive2 / (1 - VTHFA2);

  *Tagdrive =
    Tagdrive1 + Tagdrive2 + TagdriveA + TagdriveB + TagdriveA1 + TagdriveA2 +
    TagdriveB1 + TagdriveB2 + Tagdrive0a + Tagdrive0b;
  dynPower += Ceq * VddPow * VddPow * ADDRESS_BITS * Ntbl;

  /* second stage */

  Ceq = .5 * ADDRESS_BITS * 2 * draincap (Wdummyn, NCH, 2)
    + draincap (Wfaprechp, PCH, 1)
    + gatecap (Waddrnandn + Waddrnandp, 10.0 / FUDGEFACTOR) + FACwordmetal * ADDRESS_BITS;
  Rwire = FARwordmetal * ADDRESS_BITS * .5;
  tf =
    Ceq * (Rwire + transreson (Wdummyn, NCH, 1) +
	   transreson (Wdummyn, NCH, 1));
  *Tag1 = horowitz (nextinputtime, tf, VTHFA2, VTHFA3, FALL);
  nextinputtime = *Tag1 / VTHFA3;
  dynPower += Ceq * VddPow * VddPow * rows * Ntbl;

  /* third stage */

  Ceq = draincap (Waddrnandn, NCH, 2)
    + 2 * draincap (Waddrnandp, PCH, 1)
    + gatecap (Wdummyinvn + Wdummyinvp, 10.0 / FUDGEFACTOR);
  tf = Ceq * (transresswitch (Waddrnandp, PCH, 1));
  *Tag2 = horowitz (nextinputtime, tf, VTHFA3, VTHFA4, RISE);
  nextinputtime = *Tag2 / (1 - VTHFA4);
  dynPower += Ceq * VddPow * VddPow * rows * Ntbl;

  /* fourth stage */

  Ceq = (rows) * (gatecap (Wfanorn + Wfanorp, 10.0 / FUDGEFACTOR))
    + draincap (Wdummyinvn, NCH, 1)
    + draincap (Wdummyinvp, PCH, 1) + FACbitmetal * rows;
  Rwire = FARbitmetal * rows * .5;
  Req = Rwire + transreson (Wdummyinvn, NCH, 1);
  tf = Req * Ceq;
  *Tag3 = horowitz (nextinputtime, tf, VTHFA4, VTHFA5, FALL);
  *outrisetime = *Tag3 / VTHFA5;
  dynPower += Ceq * VddPow * VddPow * Ntbl;

  /* fifth stage */

  Ceq = draincap (Wfanorp, PCH, 2)
    + 2 * draincap (Wfanorn, NCH, 1) + gatecap (Wfainvn + Wfainvp, 10.0 / FUDGEFACTOR);
  tf =
    Ceq * (transresswitch (Wfanorp, PCH, 1) + transreson (Wfanorp, PCH, 1));
  *Tag4 = horowitz (nextinputtime, tf, VTHFA5, VTHFA6, RISE);
  nextinputtime = *Tag4 / (1 - VTHFA6);
  dynPower += Ceq * VddPow * VddPow;

  /* final stage */

  Ceq = (gatecap (Wdecinvn + Wdecinvp, 20.0 / FUDGEFACTOR) +
	 +draincap (Wfainvn, NCH, 1) + draincap (Wfainvp, PCH, 1));
  Req = transresswitch (Wfainvn, NCH, 1);
  tf = Req * Ceq;
  *Tag5 = horowitz (nextinputtime, tf, VTHFA6, VSINV, FALL);
  *outrisetime = *Tag5 / VSINV;
  dynPower += Ceq * VddPow * VddPow;

  //      if (Ntbl==32)
  //        fprintf(stderr," 1st - %f %f\n 2nd - %f %f\n 3rd - %f %f\n 4th - %f %f\n 5th - %f %f\nPD : %f\nNAND : %f\n INV : %f\n NOR : %f\n INV : %f\n", TagdriveA*1e9, TagdriveB*1e9, TagdriveA1*1e9, TagdriveB1*1e9, TagdriveA2*1e9, TagdriveB2*1e9, Tagdrive0a*1e9,Tagdrive0b*1e9,Tagdrive1*1e9, Tagdrive2*1e9, *Tag1*1e9, *Tag2*1e9, *Tag3*1e9, *Tag4*1e9, *Tag5*1e9);
  power->writeOp.dynamic = dynPower;
  power->readOp.dynamic = dynPower;
  return (*Tagdrive + *Tag1 + *Tag2 + *Tag3 + *Tag4 + *Tag5);
}


/*----------------------------------------------------------------------*/

/* Data array wordline delay (see section 6.2 of tech report) */

/*dt: incorporated leakage calc from eCacti*/
double Cacti4_2::wordline_delay (int C, int B,int A,int Ndwl, int Ndbl, double Nspd,
				double inrisetime,
				double *outrisetime,powerDef *power)
{
  //double tf, nextinputtime, Ceq, Req, Rline, Cline;
  double tf, nextinputtime, Ceq, Rline, Cline;
  int cols, Nact;
  double lkgCurrent=0.0, dynPower=0.0;
  double Tworddrivedel,Twordchargedel;

  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  
  //cols = CHUNKSIZE*B*A*Nspd/Ndwl;
  cols = (int) (CHUNKSIZE*B*A*Nspd/Ndwl + EPSILON);

    /* Choose a transistor size that makes sense */
    /* Use a first-order approx */

    Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
             gatecap(WwlDrvp+WwlDrvn,20.0 / FUDGEFACTOR);

    tf = transreson(Wdecinvp,PCH,1)*Ceq;

    // atleast one wordline driver in each block switches
    lkgCurrent = 0.5 * cmos_ileakage(WwlDrvn,WwlDrvp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp); 

	dynPower+=Ceq*VddPow*VddPow;

    Tworddrivedel = horowitz(inrisetime,tf,VSINV,VSINV,RISE);
    nextinputtime = Tworddrivedel/(1.0-VSINV);

    Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             Cwordmetal)*cols+
            draincap(WwlDrvn,NCH,1) + draincap(WwlDrvp,PCH,1);
    Rline = Rwordmetal*cols/2;

    tf = (transreson(WwlDrvp,PCH,1)+Rline)*Cline;

    Twordchargedel = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
    *outrisetime = Twordchargedel/VSINV;

	dynPower+=Cline*VddPow*VddPow;

	//	fprintf(stderr, "%d %f %f\n", cols, Tworddrivedel*1e3, Twordchargedel*1e3);
	// Number of active blocks in Ndwl modules
	if (Ndwl/Nspd < 1) {
		Nact = 1;
	}
	else {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//Nact = Ndwl/Nspd;
		Nact = (int) (Ndwl/Nspd + EPSILON);
	}

	power->readOp.dynamic = dynPower*Nact;
	power->readOp.leakage = lkgCurrent*Ndwl*Ndbl * VddPow;

	power->writeOp.dynamic = dynPower*Nact;
	power->writeOp.leakage = lkgCurrent*Ndwl*Ndbl * VddPow;
	// leakage power for the wordline driver to be accounted in the decoder module..
    return(Tworddrivedel+Twordchargedel);
}
/*----------------------------------------------------------------------*/

/* Tag array wordline delay (see section 6.3 of tech report) */

/*dt: incorporated leakage calc from eCacti*/
double Cacti4_2::wordline_tag_delay (int C,int B,int A,
						   int Ntspd,int Ntwl,int Ntbl,double NSubbanks,
						   double inrisetime,double *outrisetime,powerDef *power)
{
	double tf;
    double Cline,Rline,Ceq,nextinputtime;
    int tagbits, Nact;
    double lkgCurrent=0.0, dynPower=0.0;
    double Tworddrivedel,Twordchargedel;
    int cols;
    double Cload;


	if(!force_tag) {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
		//the final int value is the correct one 
		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
	}
	else {
		tagbits = force_tag_size;
	}
	cols = tagbits * A * Ntspd/Ntwl;

    // capacitive load on the wordline - C_int + C_memCellLoad * NCells
	Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
			 gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ TagCwordmetal)*cols;

	Cload = Cline / gatecap(1.0,0.0);
	// apprx width of the driver for optimal delay

	Wtdecinvn = Cload*SizingRatio/3; Wtdecinvp = 2*Cload*SizingRatio/3;

    /* first stage */

    Ceq = draincap(Wtdecinvn,NCH,1) + draincap(Wtdecinvp,PCH,1) +
              gatecap(WtwlDrvn+WtwlDrvp,20.0 / FUDGEFACTOR);
    tf = transreson(Wtdecinvn,NCH,1)*Ceq;

    Tworddrivedel = horowitz(inrisetime,tf,VSINV,VSINV,RISE);
    nextinputtime = Tworddrivedel/(1.0-VSINV);

    dynPower+=Ceq*VddPow*VddPow;

    /* second stage */
    Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             TagCwordmetal)*tagbits*A*Ntspd/Ntwl+
            draincap(WtwlDrvn,NCH,1) + draincap(WtwlDrvp,PCH,1);
    Rline = TagRwordmetal*tagbits*A*Ntspd/(2*Ntwl);
    tf = (transreson(WtwlDrvp,PCH,1)+Rline)*Cline;

	lkgCurrent = 0.5 * cmos_ileakage(WtwlDrvn,WtwlDrvp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

    Twordchargedel = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
    *outrisetime = Twordchargedel/VSINV;

	dynPower+=Cline*VddPow*VddPow;

	// Number of active blocks in Ntwl modules
	if (Ntwl/Ntspd < 1) {
		Nact = 1;
	}
	else {
		Nact = Ntwl/Ntspd;
	}


	power->readOp.dynamic = dynPower*Nact;
	power->readOp.leakage = lkgCurrent *Ntwl*Ntbl* VddPow;

	power->writeOp.dynamic = dynPower*Nact;
	power->writeOp.leakage = lkgCurrent *Ntwl*Ntbl* VddPow;

	// leakage power for the wordline drivers to be accounted in the decoder module..
    return(Tworddrivedel+Twordchargedel);

}

/*----------------------------------------------------------------------*/

/* Data array bitline: (see section 6.4 in tech report) */

/*dt: integrated width calc and leakage from eCacti */
/* Tpre is precharge delay */
double Cacti4_2::bitline_delay (int C,int A,int B,int Ndwl,int Ndbl,double Nspd,
			   double inrisetime,
			   double *outrisetime,powerDef *power,double Tpre)
{
  //double Tbit, Cline, Ccolmux, Rlineb, r1, r2, c1, c2, a, b, c;
  double Tbit, Cline, Ccolmux, Rlineb, r1, r2, c1, c2;
  double m, tstep, Rpdrive;
  double Cbitrow;		/* bitline capacitance due to access transistor */
  int rows, cols, Nact;
  int muxway;
  double dynWrtEnergy = 0.0, dynRdEnergy = 0.0;
  double Icell, Iport;

  // leakage current of a memory bit-cell
  /*dt: access port transistors only NMOS of course, hence pWidth = 0 */
  Iport = cmos_ileakage(Wmemcella   ,0           ,Vt_cell_nmos_high,Vthn,Vt_cell_pmos_high,Vthp); 
  Icell = cmos_ileakage(Wmemcellnmos,Wmemcellpmos,Vt_cell_nmos_high,Vthn,Vt_cell_pmos_high,Vthp);

  // number of bitcells = Cache size in bytes * (8 
  // ECC and other overhead)  = C*CHUNKSIZE
  // leakage current for the whole memory core -
  //
  Icell *= C*CHUNKSIZE;
  Iport *= C*CHUNKSIZE;

  Cbitrow = draincap (Wmemcella, NCH, 1) / 2.0;	/* due to shared contact */
  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
  //the final int value is the correct one 
  //rows = C / (B * A * Ndbl * Nspd);
  //cols = CHUNKSIZE * B * A * Nspd / Ndwl;
  rows = (int) (C / (B * A * Ndbl * Nspd) + EPSILON);
  cols = (int) (CHUNKSIZE * B * A * Nspd / Ndwl + EPSILON);

  // Determine equivalent width of bitline precharge transistors
   	// we assume that Precharge time = decoder delay + wordlineDriver delay time.
   	// This is because decoder delay + wordline driver delay contributes to most
   	// of the read/write access times.

	Cline = rows*(Cbitrow+Cbitmetal);
	Rpdrive = Tpre/(Cline*log(VSINV)*-1.0);
	Wbitpreequ = restowidth(Rpdrive,PCH);

	// Note that Wbitpreequ is the equiv. pch transistor width
	Wpch = 2.0/3.0*Wbitpreequ;

    if (Wpch > Wpchmax) {
           Wpch = Wpchmax;
	}

	//v4.1: Expressing all widths in terms of FEATURESIZE of input tech node

	// Isolation transistor width is set to 10 (typical). Its usually wide to reduce
	// resistance offered for transfer of bitline voltage to sense-amp.
	Wiso = 12.5*FEATURESIZE;//was 10 micron for the 0.8 micron process

	// width of sense precharge
	// (depends on width of sense-amp transistors and output driver size)
	// ToDo: calculate the width of sense-amplifier as a function of access times...

	WsPch = 6.25*FEATURESIZE;   // ToDo: Determine widths of these devices analytically; was 5 micron for the 0.8 micron process
	WsenseN = 3.75*FEATURESIZE; // sense amplifier N-trans; was 3 micron for the 0.8 micron process
	WsenseP = 7.5*FEATURESIZE; // sense amplifier P-trans; was 6 micron for the 0.8 micron process
	WsenseEn = 5*FEATURESIZE; // Sense enable transistor of the sense amplifier; was 4 micron for the 0.8 micron process
	WoBufP = 10*FEATURESIZE; // output buffer corresponding to the sense amplifier; was 8 micron for the 0.8 micron process
	WoBufN = 5*FEATURESIZE; //was 4 micron for the 0.8 micron process

	// ToDo: detemine the size of colmux (Wbitmux)

  if (8 * B / BITOUT == 1 && Ndbl * Nspd == 1)
    {
      Cline = rows*(Cbitrow+Cbitmetal)+2*draincap(Wbitpreequ,PCH,1);
      Ccolmux = gatecap(WsenseN+WsenseP,10.0 / FUDGEFACTOR);
      Rlineb = Rbitmetal*rows/2.0;
      r1 = Rlineb;
	  // muxover=1;
    }
  else
    {
      if (Nspd > MAX_COL_MUX)
	{
	  //muxover=8*B/BITOUT;
	  muxway = MAX_COL_MUX;
	}
      else if (8 * B * Nspd / BITOUT > MAX_COL_MUX)
	{
	  muxway = MAX_COL_MUX;
	  // muxover=(8*B/BITOUT)/(MAX_COL_MUX/(Ndbl*Nspd));
	}
      else
	{
	  muxway = 8 * B * Nspd / BITOUT;
	  // muxover=1;
	}

      Cline = rows * (Cbitrow + Cbitmetal) + 2 * draincap (Wbitpreequ, PCH,
						     1) + draincap(Wiso,PCH,1);
      Ccolmux = muxway*(draincap(Wiso,PCH,1))+gatecap(WsenseN+WsenseP,10.0 / FUDGEFACTOR);

      Rlineb = Rbitmetal * rows / 2.0;
      r1 = Rlineb + transreson(Wiso,PCH,1);
    }
  r2 = transreson (Wmemcella, NCH, 1) +
    transreson (Wmemcella * Wmemcellbscale, NCH, 1);
  c1 = Ccolmux;
  c2 = Cline;

  /*dt: If a wordline segment is shorter than a set, then multiple segments have to be activated */
  if (Ndwl < Nspd) {
		Nact = 1;
  }
  else {
	   //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
       //the final int value is the correct one 
	   //Nact = Ndwl/Nspd;
	   Nact = (int) (Ndwl/Nspd + EPSILON);
  }
  
  dynRdEnergy += c1 * VddPow * VddPow * BITOUT * A * muxover;
  /*
  was: *power += c2 * VddPow * VbitprePow * cols;
  now: we only have limited bitline swing thanks to pulsed wordlines. From looking at papers from industry
  I'm assuming that bitline swing can be limited to Vbitswing 100mV */
  dynRdEnergy += c2 * VddPow * VbitprePow * cols * Nact;

  /*dt: assuming full swing on the bitlines for writes, but only BITOUT bitlines are activated.*/
  dynWrtEnergy += c2*VddPow*VddPow*BITOUT;

  //fprintf(stderr, "Pow %f %f\n", c1*VddPow*VddPow*BITOUT*A*muxover*1e9, c2*VddPow*VbitprePow*cols*1e9);
  tstep =
    (r2 * c2 + (r1 + r2) * c1) * log ((Vbitpre) / (Vbitpre - Vbitsense));

  /* take input rise time into account */

  m = Vdd / inrisetime;
  if (tstep <= (0.5 * (Vdd - Vt) / m))
  //v4.1: Using the CACTI journal paper version equation under this condition
    {
      /*a = m;
      b = 2 * ((Vdd * 0.5) - Vt);
      c = -2 * tstep * (Vdd - Vt) + 1 / m * ((Vdd * 0.5) - Vt) *
	((Vdd * 0.5) - Vt);
      Tbit = (-b + sqrt (b * b - 4 * a * c)) / (2 * a);*/

	 Tbit = sqrt(2*tstep*(Vdd-Vt)/m);
    }
  else
    {
      Tbit = tstep + (Vdd + Vt) / (2 * m) - (Vdd * 0.5) / m;
    }

  *outrisetime = Tbit / (log ((Vbitpre - Vbitsense) / Vdd));


  power->writeOp.dynamic = dynWrtEnergy;
  power->writeOp.leakage = (Icell + Iport) * VddPow;

  power->readOp.dynamic = dynRdEnergy;
  power->readOp.leakage = (Icell + Iport) * VddPow;

  return (Tbit);
}




/*----------------------------------------------------------------------*/

/* Tag array bitline: (see section 6.4 in tech report) */

/*dt: added leakage from eCacti, fixed bitline swing being too large*/
double Cacti4_2::bitline_tag_delay (int C,int A,int B,int Ntwl,int Ntbl,int Ntspd,
				   double NSubbanks,double inrisetime,
				   double *outrisetime,powerDef *power,double Tpre)
{
  //double Tbit, Cline, Ccolmux, Rlineb, r1, r2, c1, c2, a, b, c;
  double Tbit, Cline, Ccolmux, Rlineb, r1, r2, c1, c2;
  double m, tstep, Rpdrive;
  double Cbitrow;		/* bitline capacitance due to access transistor */
  int tagbits;
  int rows, cols, Nact;
  double lkgCell=0.0;
  double lkgPort = 0.0;
  double dynRdEnergy=0.0, dynWrtEnergy=0.0;

  // leakage current of a memory bit-cell

  lkgPort = cmos_ileakage(Wmemcella   ,0           ,Vt_cell_nmos_high,Vthn,Vt_cell_pmos_high,Vthp);
  lkgCell = cmos_ileakage(Wmemcellnmos,Wmemcellpmos,Vt_cell_nmos_high,Vthn,Vt_cell_pmos_high,Vthp);

  if(!force_tag) {
    //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
	//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) + (int) logtwo ((double) A) -
				//(int) (logtwo (NSubbanks));
	tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) + (int) logtwo ((double) A) -
				(int) (logtwo (NSubbanks)) + EPSILON);
  }
  else {
	  tagbits = force_tag_size;
  }

  Cbitrow = draincap (Wmemcella, NCH, 1) / 2.0;	/* due to shared contact */
  rows = C / (B * A * Ntbl * Ntspd);
  cols = tagbits * A * Ntspd / Ntwl;

  // leakage current for the the whole memory core -
  lkgCell *= C/B*tagbits;
  lkgPort *= C/B*tagbits;

	// Determine equivalent width of bitline precharge transistors
   	// we assume that Precharge time = decoder delay + wordlineDriver delay time.
   	// This is because decoder delay + wordline driver delay contributes to most
   	// of the read/write access times.

	Cline = rows*(Cbitrow+Cbitmetal);
	Rpdrive = Tpre/(Cline*log(VSINV)*-1.0);
	Wtbitpreequ = restowidth(Rpdrive,PCH);

	// Note that Wbitpreequ is the equiv. pch transistor width
    Wtpch = 2.0/3.0*Wtbitpreequ;

    if (Wtpch > Wpchmax) {
           Wtpch = Wpchmax;
    }

	// width of sense precharge
	// (depends on width of sense-amp transistors and output driver size)
	// ToDo: calculate the width of sense-amplifier as a function of access times...

     //v4.1: Expressing all widths in terms of FEATURESIZE of input tech node

	WtsPch = 6.25*FEATURESIZE;   // ToDo: Determine widths of these devices analytically; was 5 micron for the 0.8 micron process
	WtsenseN = 3.75*FEATURESIZE; // sense amplifier N-trans; was 3 micron for the 0.8 micron process
	WtsenseP = 7.5*FEATURESIZE; // sense amplifier P-trans; was 6 micron for the 0.8 micron process
	WtsenseEn = 5*FEATURESIZE; // Sense enable transistor of the sense amplifier; was 4 micron for the 0.8 micron process
	WtoBufP = 10*FEATURESIZE; // output buffer corresponding to the sense amplifier; was 8 micron for the 0.8 micron process
	WtoBufN = 5*FEATURESIZE; //was 4 micron for the 0.8 micron process
      Wtiso = 12.5*FEATURESIZE;//was 10 micron for the 0.8 micron process

  //v4.1: Ntspd*Ntbl in the colmux of the else condition does not make sense and overestimates
  //tag bitline delay by a huge amount when Ntspd and Ndbl are large. Fixing this and fixing the if
  //condition as well
  /*if (Ntbl * Ntspd == 1)
    {
      Cline = rows*(Cbitrow+TagCbitmetal)+2*draincap(Wtbitpreequ,PCH,1);
       Ccolmux = gatecap(WtsenseN+WtsenseP,10.0 / FUDGEFACTOR);
       Rlineb = TagRbitmetal*rows/2.0;
       r1 = Rlineb;
    }
  else
    {
      Cline = rows*(Cbitrow+TagCbitmetal) + 2*draincap(Wtbitpreequ,PCH,1) +
                 draincap(Wtiso,PCH,1);
       Ccolmux = Ntspd*Ntbl*(draincap(Wtiso,PCH,1))+gatecap(WtsenseN+WtsenseP,10.0 /FUDGEFACTOR);
       Rlineb = TagRbitmetal*rows/2.0;
 	   r1 = Rlineb + transreson(Wtiso,PCH,1);
    }*/

   if(Ntspd/Ntwl < 1){ //Ntspd/Ntwl = Tag subarray columns / Tag array output width =
		//= (A * tagbits * Ntspd / Ntwl) / A * tagbits = Degree of column muxing in a tag subarray.
		//When it's less than 1, it means that more than one tag subarray is required to produce the 
		//tag array output width.
	    Cline = rows*(Cbitrow+TagCbitmetal)+2*draincap(Wtbitpreequ,PCH,1);
        Ccolmux = gatecap(WtsenseN+WtsenseP,10.0 / FUDGEFACTOR);
        Rlineb = TagRbitmetal*rows/2.0;
        r1 = Rlineb;
	}
	else{//Tag array output width comes from one tag subarray
			Cline = rows*(Cbitrow+TagCbitmetal) + 2*draincap(Wtbitpreequ,PCH,1) + draincap(Wtiso,PCH,1);
			Ccolmux = Ntspd/Ntwl*(draincap(Wtiso,PCH,1))+gatecap(WtsenseN+WtsenseP,10.0 / FUDGEFACTOR);
			Rlineb = TagRbitmetal*rows/2.0;
 			r1 = Rlineb + transreson(Wtiso,PCH,1);
	}

  r2 = transreson (Wmemcella, NCH, 1) +
    transreson (Wmemcella * Wmemcellbscale, NCH, 1);

  c1 = Ccolmux;
  c2 = Cline;
  //v4.1: The tag energy calculation did not make use of the computed Nact below. Fixing that.	


  //dynRdEnergy += c1 * VddPow * VddPow;

  // Number of active Ntwl blocks
  if (Ntwl/Ntspd < 1) {
	  Nact = 1; }
  else {
	  Nact = Ntwl/Ntspd; }

  //dynRdEnergy += c2 * VddPow * VbitprePow * cols;

  dynRdEnergy += c1 * VddPow * VddPow * Nact;

  dynRdEnergy += c2 * VddPow * VbitprePow * cols * Nact;

  if (cols > tagbits)
  {
		dynWrtEnergy += c2*VddPow*VddPow*tagbits;
		dynWrtEnergy += c2*VddPow*VbitprePow*(cols-tagbits);
  }
  else {
	dynWrtEnergy += c2*VddPow*VddPow*tagbits;
  }

  //fprintf(stderr, "Pow %f %f\n", c1*VddPow*VddPow*1e9, c2*VddPow*VbitprePow*cols*1e9);

  tstep =
    (r2 * c2 + (r1 + r2) * c1) * log ((Vbitpre) / (Vbitpre - Vbitsense));

  /* take into account input rise time */

  m = Vdd / inrisetime;
  if (tstep <= (0.5 * (Vdd - Vt) / m))
    //v4.1: Using the CACTI journal paper version equation under this condition
    {
      /*a = m;
      b = 2 * ((Vdd * 0.5) - Vt);
      c = -2 * tstep * (Vdd - Vt) + 1 / m * ((Vdd * 0.5) - Vt) *
	((Vdd * 0.5) - Vt);
      Tbit = (-b + sqrt (b * b - 4 * a * c)) / (2 * a);*/

	  Tbit = sqrt(2*tstep*(Vdd-Vt)/m);
    }
  else
    {
      Tbit = tstep + (Vdd + Vt) / (2 * m) - (Vdd * 0.5) / m;
    }

  *outrisetime = Tbit / (log ((Vbitpre - Vbitsense) / Vdd));

  power->readOp.dynamic = dynRdEnergy;
  power->readOp.leakage = (lkgCell + lkgPort) * VddPow;

  power->writeOp.dynamic = dynWrtEnergy;
  power->writeOp.leakage = (lkgCell + lkgPort) * VddPow;

  return (Tbit);
}



/*----------------------------------------------------------------------*/

/* It is assumed the sense amps have a constant delay
   (see section 6.5) */

double Cacti4_2::sense_amp_delay (int C,int B,int A,int Ndwl,int Ndbl,double Nspd, double inrisetime,double *outrisetime, powerDef *power)
{
  double Cload;
	int cols,Nact;
	double IsenseEn, IsenseN, IsenseP, IoBufP, IoBufN, Iiso, Ipch, IsPch;
	double lkgIdlePh, lkgReadPh, lkgWritePh;
	double lkgRead, lkgWrite, lkgIdle;

	//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
 	//cols = CHUNKSIZE*B*A*Nspd/Ndwl;
	cols = (int) (CHUNKSIZE*B*A*Nspd/Ndwl + EPSILON);

	// Number of active blocks in Ntwl modules during a read op
	if (Ndwl/Nspd < 1) {
		Nact = 1; }
	else {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
		//the final int value is the correct one 
		//Nact = Ndwl/Nspd; 
	    Nact = (int) (Ndwl/Nspd + EPSILON);}

	if (dualVt == 1)
	{
		IsenseEn = simplified_nmos_leakage(WsenseEn*inv_Leff,Vt_cell_nmos_high);
		IsenseN  = simplified_nmos_leakage(WsenseN*inv_Leff,Vt_cell_nmos_high);
		IsenseP  = simplified_pmos_leakage(WsenseP*inv_Leff,Vt_cell_pmos_high);
		IoBufN   = simplified_nmos_leakage(WoBufN*inv_Leff,Vt_cell_nmos_high);
		IoBufP   = simplified_pmos_leakage(WoBufP*inv_Leff,Vt_cell_pmos_high);
		Iiso     = simplified_nmos_leakage(Wiso*inv_Leff,Vt_cell_nmos_high);
		Ipch     = simplified_pmos_leakage(Wpch*inv_Leff,Vt_cell_pmos_high);
		IsPch    = simplified_pmos_leakage(WsPch*inv_Leff,Vt_cell_pmos_high);
	}
	else {
		IsenseEn = simplified_nmos_leakage(WsenseEn*inv_Leff,Vthn);
		IsenseN  = simplified_nmos_leakage(WsenseN*inv_Leff,Vthn);
		IsenseP  = simplified_pmos_leakage(WsenseP*inv_Leff,Vthp);
		IoBufN   = simplified_nmos_leakage(WoBufN*inv_Leff,Vthn);
		IoBufP   = simplified_pmos_leakage(WoBufP*inv_Leff,Vthp);
		Iiso     = simplified_nmos_leakage(Wiso*inv_Leff,Vthn);
		Ipch     = simplified_pmos_leakage(Wpch*inv_Leff,Vthp);
		IsPch    = simplified_pmos_leakage(WsPch*inv_Leff,Vthp);
	}

	lkgIdlePh = IsenseEn + 2*IoBufP;
	lkgWritePh = 2*Ipch + Iiso + IsenseEn + 2*IoBufP;
	lkgReadPh = 2*IsPch + Iiso + IsenseN + IsenseP + IoBufN + IoBufP;

	// read cols in the inactive blocks would be in idle phase
	lkgRead = lkgReadPh * cols * Nact + lkgIdlePh * cols * (Nact - 1);
	// only the cols in which data is written into are in write ph
	// all the remaining cols are considered to be in idle phase
	lkgWrite = lkgWritePh * BITOUT + lkgIdlePh * (cols*Ndwl - BITOUT);
	lkgIdle = lkgIdlePh * cols * Ndwl;

	// sense amplifier has to drive logic in "data out driver" and sense precharge load.
	// load seen by sense amp

	Cload = gatecap(WsenseP+WsenseN,5.0 / FUDGEFACTOR) + draincap(WsPch,PCH,1) +
			gatecap(Woutdrvnandn+Woutdrvnandp,1.0 / FUDGEFACTOR) + gatecap(Woutdrvnorn+Woutdrvnorp,1.0 / FUDGEFACTOR);

   	*outrisetime = tfalldata;
   	power->readOp.dynamic = 0.5* Cload * VddPow * VddPow *BITOUT * A * muxover;
   	power->readOp.leakage = ((lkgRead + lkgIdle)/2 ) * VddPow;

   	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = ((lkgWrite + lkgIdle)/2 ) * VddPow;

   	return(tsensedata+2.5e-10/FUDGEFACTOR); //v4.1: Computing sense amp delay component for input tech node itself instead
	//of dividing by FUDGEFACTOR at the end.
}

/*--------------------------------------------------------------*/

double Cacti4_2::sense_amp_tag_delay (int C,int B,int A,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,double inrisetime, double *outrisetime, powerDef *power)
{ 
    double Cload;
	int cols, tagbits, Nact;
	double IsenseEn, IsenseN, IsenseP, IoBufP, IoBufN, Iiso, Ipch, IsPch;
	double lkgIdlePh, lkgReadPh, lkgWritePh;
	double lkgRead, lkgWrite, lkgIdle;

	if(!force_tag) {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
		//the final int value is the correct one 
		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS -(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS -(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
	}
	else {
		tagbits = force_tag_size;
	}
	cols = tagbits*A*Ntspd/Ntwl;

	// Number of active blocks in Ntwl modules during a read op
	if (Ntwl/Ntspd < 1) {
		Nact = 1; }
	else {
		Nact = Ntwl/Ntspd; }

	if (dualVt == 1)
	{
		IsenseEn = simplified_nmos_leakage(WtsenseEn*inv_Leff,Vt_cell_nmos_high);
		IsenseN = simplified_nmos_leakage(WtsenseN*inv_Leff,Vt_cell_nmos_high);
		IsenseP = simplified_pmos_leakage(WtsenseP*inv_Leff,Vt_cell_pmos_high);
		IoBufN = simplified_nmos_leakage(WtoBufN*inv_Leff,Vt_cell_nmos_high);
		IoBufP = simplified_pmos_leakage(WtoBufP*inv_Leff,Vt_cell_pmos_high);
		Iiso = simplified_nmos_leakage(Wtiso*inv_Leff,Vt_cell_nmos_high);
		Ipch = simplified_pmos_leakage(Wtpch*inv_Leff,Vt_cell_pmos_high);
		IsPch = simplified_pmos_leakage(WtsPch*inv_Leff,Vt_cell_pmos_high);
	}
	else
	{
		IsenseEn = simplified_nmos_leakage(WtsenseEn*inv_Leff,Vthn);
		IsenseN = simplified_nmos_leakage(WtsenseN*inv_Leff,Vthn);
		IsenseP = simplified_pmos_leakage(WtsenseP*inv_Leff,Vthp);
		IoBufN = simplified_nmos_leakage(WtoBufN*inv_Leff,Vthn);
		IoBufP = simplified_pmos_leakage(WtoBufP*inv_Leff,Vthp);
		Iiso = simplified_nmos_leakage(Wtiso*inv_Leff,Vthn);
		Ipch = simplified_pmos_leakage(Wtpch*inv_Leff,Vthp);
		IsPch = simplified_pmos_leakage(WtsPch*inv_Leff,Vthp);
	}

	lkgIdlePh = IsenseEn + 2*IoBufP;
	lkgWritePh = 2*Ipch + Iiso + IsenseEn + 2*IoBufP;
	lkgReadPh = 2*IsPch + Iiso + IsenseN + IsenseP + IoBufN + IoBufP;

	// read cols in the inactive blocks would be in idle phase
	lkgRead = lkgReadPh * cols * Nact + lkgIdlePh * cols * (Nact - 1);
	// only the cols in which data is written into are in write ph
	// all the remaining cols are considered to be in idle phase
	lkgWrite = lkgWritePh * tagbits + lkgIdlePh * (cols*Ntwl - tagbits);
	lkgIdle = lkgIdlePh * cols * Ntwl;

	// sense amplifier has to drive logic in "data out driver" and sense precharge load.
	// load seen by sense amp

	Cload = gatecap(WtsenseP+WtsenseN,5.0 / FUDGEFACTOR) + draincap(WtsPch,PCH,1);
				//gatecap(Woutdrvnandn+Woutdrvnandp,1.0) + gatecap(Woutdrvnorn+Woutdrvnorp,1.0);

	*outrisetime = tfalltag;
	power->readOp.dynamic = 0.5* Cload * VddPow * VddPow *tagbits * A;
	power->readOp.leakage = ((lkgRead + lkgIdle)/2 ) * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = ((lkgWrite + lkgIdle)/2 ) * VddPow;

   	return(tsensetag+2.5e-10/FUDGEFACTOR); //v4.1: Computing sense amp delay component for input tech node itself instead
	//of dividing by FUDGEFACTOR at the end.
}

/*----------------------------------------------------------------------*/

/* Comparator Delay (see section 6.6) */


double Cacti4_2::compare_time (int C,int A,int Ntbl,int Ntspd,double NSubbanks,double inputtime,double *outputtime,powerDef *power)
{
	double Req,Ceq,tf,st1del,st2del,st3del,nextinputtime,m;
   	double c1,c2,r1,r2,tstep,a,b,c;
   	double Tcomparatorni;
   	int cols,tagbits;
   	double lkgCurrent=0.0, dynPower=0.0;

   	/* First Inverter */

   	Ceq = gatecap(Wcompinvn2+Wcompinvp2,10.0 / FUDGEFACTOR) +
   	      draincap(Wcompinvp1,PCH,1) + draincap(Wcompinvn1,NCH,1);
   	Req = transreson(Wcompinvp1,PCH,1);
   	tf = Req*Ceq;
   	st1del = horowitz(inputtime,tf,VTHCOMPINV,VTHCOMPINV,FALL);
   	nextinputtime = st1del/VTHCOMPINV;
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	lkgCurrent = 0.5 * A * cmos_ileakage(Wcompinvn1,Wcompinvp1,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
   	/* Second Inverter */

   	Ceq = gatecap(Wcompinvn3+Wcompinvp3,10.0 / FUDGEFACTOR) +
   	      draincap(Wcompinvp2,PCH,1) + draincap(Wcompinvn2,NCH,1);
   	Req = transreson(Wcompinvn2,NCH,1);
   	tf = Req*Ceq;
   	st2del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHCOMPINV,RISE);
   	nextinputtime = st2del/(1.0-VTHCOMPINV);
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	lkgCurrent += 0.5 * A * cmos_ileakage(Wcompinvn2,Wcompinvp2,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

   	/* Third Inverter */

   	Ceq = gatecap(Wevalinvn+Wevalinvp,10.0 / FUDGEFACTOR) +
   	      draincap(Wcompinvp3,PCH,1) + draincap(Wcompinvn3,NCH,1);
   	Req = transreson(Wcompinvp3,PCH,1);
   	tf = Req*Ceq;
   	st3del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHEVALINV,FALL);
   	nextinputtime = st3del/(VTHEVALINV);
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	lkgCurrent += 0.5 * A * cmos_ileakage(Wcompinvn3,Wcompinvp3,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

   	/* Final Inverter (virtual ground driver) discharging compare part */

	if(!force_tag) {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
   		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
	}
	else {
		tagbits = force_tag_size;
	}
   	tagbits=tagbits/2;
   	cols = tagbits*Ntbl*Ntspd;

   	r1 = transreson(Wcompn,NCH,2);
   	r2 = transreson(Wevalinvn,NCH,1); /* was switch */
   	c2 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
   	      draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);
   	c1 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))
   	     +draincap(Wcompp,PCH,1) + gatecap(WmuxdrvNANDn+WmuxdrvNANDp,20.0 / FUDGEFACTOR) +
   	     cols*Cwordmetal;
   	dynPower+=c2*VddPow*VddPow*2*A;
   	dynPower+=c1*VddPow*VddPow*(A-1);

	lkgCurrent += 0.5 * A * cmos_ileakage(Wevalinvn,Wevalinvp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
		// double the tag_bits as the delay is being computed for only
		// half the number. The leakage is still for the total tag bits

		// the comparator compares cols number of bits. Since these transistors are
		// stacked, a stacking factor of 0.2 is used
    lkgCurrent += 2 * cols * 0.2 * 0.5 * A * cmos_ileakage(Wcompn,Wcompp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
		// leakage due to the mux driver
	lkgCurrent += 0.5 * A * cmos_ileakage(Wmuxdrv12n,Wmuxdrv12p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);	

   	/* time to go to threshold of mux driver */

   	tstep = (r2*c2+(r1+r2)*c1)*log(1.0/VTHMUXNAND);

   	/* take into account non-zero input rise time */

   	m = Vdd/nextinputtime;

   	if ((tstep) <= (0.5*(Vdd-Vt)/m)) {
  		a = m;
	    b = 2*((Vdd*VTHEVALINV)-Vt);
        c = -2*(tstep)*(Vdd-Vt)+1/m*((Vdd*VTHEVALINV)-Vt)*((Vdd*VTHEVALINV)-Vt);
 	    Tcomparatorni = (-b+sqrt(b*b-4*a*c))/(2*a);
   	} else {
		Tcomparatorni = (tstep) + (Vdd+Vt)/(2*m) - (Vdd*VTHEVALINV)/m;
   	}
   	*outputtime = Tcomparatorni/(1.0-VTHMUXNAND);

	power->readOp.dynamic = dynPower;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = lkgCurrent * VddPow;

   	return(Tcomparatorni+st1del+st2del+st3del);
}




/*----------------------------------------------------------------------*/

/* Delay of the multiplexor Driver (see section 6.7) */


double Cacti4_2::mux_driver_delay (int Ntbl,int Ntspd,double inputtime,
		  double *outputtime,double wirelength)
{
  double Ceq, Req, tf, nextinputtime;
  double Tst1, Tst2, Tst3;

  /* first driver stage - Inverte "match" to produce "matchb" */
  /* the critical path is the DESELECTED case, so consider what
     happens when the address bit is true, but match goes low */

  Ceq = gatecap (WmuxdrvNORn + WmuxdrvNORp, 15.0 / FUDGEFACTOR) * muxover +
    draincap (Wmuxdrv12n, NCH, 1) + draincap (Wmuxdrv12p, PCH, 1);
  Req = transreson (Wmuxdrv12p, PCH, 1);
  tf = Ceq * Req;
  Tst1 = horowitz (inputtime, tf, VTHMUXDRV1, VTHMUXDRV2, FALL);
  nextinputtime = Tst1 / VTHMUXDRV2;

  /* second driver stage - NOR "matchb" with address bits to produce sel */

  Ceq =
    gatecap (Wmuxdrv3n + Wmuxdrv3p, 15.0 / FUDGEFACTOR) + 2 * draincap (WmuxdrvNORn, NCH,
							  1) +
    draincap (WmuxdrvNORp, PCH, 2);
  Req = transreson (WmuxdrvNORn, NCH, 1);
  tf = Ceq * Req;
  Tst2 = horowitz (nextinputtime, tf, VTHMUXDRV2, VTHMUXDRV3, RISE);
  nextinputtime = Tst2 / (1 - VTHMUXDRV3);

  /* third driver stage - invert "select" to produce "select bar" */


  Ceq =
    BITOUT * gatecap (Woutdrvseln + Woutdrvselp + Woutdrvnorn + Woutdrvnorp,20.0)
	+ draincap (Wmuxdrv3p, PCH,1) + draincap (Wmuxdrv3n, NCH,1) +
    GlobalCwordmetal * wirelength;
  Req = (GlobalRwordmetal * wirelength) / 2.0 + transreson (Wmuxdrv3p, PCH, 1);
  tf = Ceq * Req;
  Tst3 = horowitz (nextinputtime, tf, VTHMUXDRV3, VTHOUTDRINV, FALL);
  *outputtime = Tst3 / (VTHOUTDRINV);

  return (Tst1 + Tst2 + Tst3);

}
void Cacti4_2::precalc_muxdrv_widths(int C,int B,int A,int Ndwl,int Ndbl,double Nspd,double * wirelength_v,double * wirelength_h)
{
	int l_muxdrv_v = 0, l_muxdrv_h = 0, cols_subarray, rows_subarray, nr_subarrays_left, horizontal_step, vertical_step, v_or_h;
	//double wirelength, Ceq, Cload, Cline;
	double Ceq, Cload, Cline;
	int wire_v = 0, wire_h = 0;
	double current_ndriveW,current_pdriveW, previous_ndriveW,previous_pdriveW;

    //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
    //cols_subarray = (CHUNKSIZE * B * A * Nspd / Ndwl);
    //rows_subarray = (C /(B * A * Ndbl * Nspd));
	cols_subarray = (int) (CHUNKSIZE * B * A * Nspd / Ndwl + EPSILON);
    rows_subarray = (int) (C /(B * A * Ndbl * Nspd) + EPSILON);

    if (Ndwl * Ndbl == 1) {
        l_muxdrv_v = 0;
        l_muxdrv_h = cols_subarray;

		Cmuxdrvtreesegments[0] = GlobalCwordmetal*cols_subarray;
		Rmuxdrvtreesegments[0] = 0.5*GlobalRwordmetal*cols_subarray;

		Cline = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0 / FUDGEFACTOR) + 
			    GlobalCwordmetal*cols_subarray;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		nr_muxdrvtreesegments = 0;

    }
    else if (Ndwl * Ndbl == 2
        || Ndwl * Ndbl == 4) {
        l_muxdrv_v = 0;
        l_muxdrv_h = 2*cols_subarray;

		Cmuxdrvtreesegments[0] = GlobalCwordmetal*cols_subarray;
		Rmuxdrvtreesegments[0] = 0.5*GlobalRwordmetal*cols_subarray;

		Cline = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0 / FUDGEFACTOR) + 
			    GlobalCwordmetal*cols_subarray;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;
		
		nr_muxdrvtreesegments = 0;
    }
    else if (Ndwl * Ndbl > 4) {
		nr_subarrays_left = Ndwl* Ndbl;
		nr_subarrays_left /= 2;
		horizontal_step = cols_subarray;
		vertical_step = rows_subarray;
		l_muxdrv_h = horizontal_step;

		Cmuxdrvtreesegments[0] = GlobalCwordmetal*horizontal_step;
		Rmuxdrvtreesegments[0] = 0.5*GlobalRwordmetal*horizontal_step;

		Cline = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0 / FUDGEFACTOR) + 
			    GlobalCwordmetal*horizontal_step;
		Cload = Cline / gatecap(1.0,0.0);

		current_ndriveW = Cload*SizingRatio/3;
		current_pdriveW = 2*Cload*SizingRatio/3;

		WmuxdrvtreeN[0] = current_ndriveW;
		nr_muxdrvtreesegments = 1;

		horizontal_step *= 2;
		v_or_h = 1; // next step is vertical

		while(nr_subarrays_left > 1) {
			previous_ndriveW = current_ndriveW;
			previous_pdriveW = current_pdriveW;
			nr_muxdrvtreesegments++;
			if(v_or_h) {
				l_muxdrv_v += vertical_step;
				v_or_h = 0;
				
				Cmuxdrvtreesegments[nr_muxdrvtreesegments-1] = GlobalCbitmetal*vertical_step;
				Rmuxdrvtreesegments[nr_muxdrvtreesegments-1] = 0.5*GlobalRbitmetal*vertical_step;
				Cline = gatecap(previous_ndriveW+previous_pdriveW,0)+GlobalCbitmetal*vertical_step;

				vertical_step *= 2;
				nr_subarrays_left /= 2;
			}
			else {
				l_muxdrv_h += horizontal_step;
				v_or_h = 1;

				Cmuxdrvtreesegments[nr_muxdrvtreesegments-1] = GlobalCwordmetal*horizontal_step;
				Rmuxdrvtreesegments[nr_muxdrvtreesegments-1] = 0.5*GlobalRwordmetal*horizontal_step;
				Cline = gatecap(previous_ndriveW+previous_pdriveW,0)+GlobalCwordmetal*horizontal_step;

				horizontal_step *= 2;
				nr_subarrays_left /= 2;
			}

			Cload = Cline / gatecap(1.0,0.0);

			current_ndriveW = Cload*SizingRatio/3;
			current_pdriveW = 2*Cload*SizingRatio/3;

			WmuxdrvtreeN[nr_muxdrvtreesegments-1] = current_ndriveW;
		}
    }

    wire_v = (l_muxdrv_v);
    wire_h = (l_muxdrv_h);

	

	Ceq = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0 / FUDGEFACTOR);
	/*dt: adding an extra layer of drivers, so  that we don't have these biiiig gates */
	Ceq += GlobalCwordmetal*wire_h + GlobalCbitmetal*wire_v;
   	Cload = Ceq / gatecap(1.0,0.0);
   	Wmuxdrv3p = Cload * SizingRatio * 2/3;
   	Wmuxdrv3n = Cload * SizingRatio /3;


   	WmuxdrvNORn = (Wmuxdrv3n + Wmuxdrv3p) * SizingRatio /3;

   	// 2 to account for series pmos in NOR
   	WmuxdrvNORp = 2* (Wmuxdrv3n + Wmuxdrv3p) * SizingRatio * 2/3;

   	WmuxdrvNANDn = 2*(WmuxdrvNORn+WmuxdrvNORp)*muxover*SizingRatio*1/3;
   	WmuxdrvNANDp = (WmuxdrvNORn+WmuxdrvNORp)*muxover*SizingRatio*2/3;

	*wirelength_h = wire_h;
	*wirelength_v = wire_v;

}
/*dt: incorporated leakage and resizing from eCacti*/
double Cacti4_2::mux_driver_delay_dualin (int C,int B,int A,int Ntbl,int Ntspd,double inputtime1,
			 double *outputtime,double wirelength_v,double wirelength_h,powerDef *power)
{
	//double Ceq,Req,Rwire,tf,nextinputtime, Cload;
	double Ceq,Req,Rwire,tf,nextinputtime;
   	//double Tst1,Tst2,Tst2a,Tst3,Tst4;
	double Tst1,Tst2,Tst3;
   	double lkgCurrent=0.0, dynPower = 0.0;
	//double Wwiredrv1n, Wwiredrv1p, Wmuxdrv4n, Wmuxdrv4p;
	double overall_delay = 0;
	int i = 0;
	double inrisetime = 0, this_delay = 0;
	
   	/* first driver stage - Inverte "match" to produce "matchb" */
   	/* the critical path is the DESELECTED case, so consider what
   	   happens when the address bit is true, but match goes low */

   	Ceq = gatecap(WmuxdrvNORn+WmuxdrvNORp,15.0 / FUDGEFACTOR)*muxover
   	  +draincap(WmuxdrvNANDn,NCH,2) + 2*draincap(WmuxdrvNANDp,PCH,1);
   	Req = transreson(WmuxdrvNANDp,PCH,1);
   	tf = Ceq*Req;
   	Tst1 = horowitz(inputtime1,tf,VTHMUXNAND,VTHMUXDRV2,FALL);
   	nextinputtime = Tst1/VTHMUXDRV2;
   	dynPower+=Ceq*VddPow*VddPow*(A-1);

	lkgCurrent = 0.5 * 0.2 * muxover * cmos_ileakage(WmuxdrvNORn,WmuxdrvNORp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	lkgCurrent += 0.5 * 0.2 * muxover * cmos_ileakage(WmuxdrvNANDn,WmuxdrvNANDp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);


   	/* second driver stage - NOR "matchb" with address bits to produce sel */
   	Ceq = gatecap(Wmuxdrv3n+Wmuxdrv3p,15.0 / FUDGEFACTOR) + 2*draincap(WmuxdrvNORn,NCH,1) +
   	      draincap(WmuxdrvNORp,PCH,2);
   	Req = transreson(WmuxdrvNORn,NCH,1);
   	tf = Ceq*Req;
   	Tst2 = horowitz(nextinputtime,tf,VTHMUXDRV2,VTHMUXDRV3,RISE);
   	nextinputtime = Tst2/(1-VTHMUXDRV3);
   	dynPower+=Ceq*VddPow*VddPow;

	lkgCurrent += 0.5 *  muxover * cmos_ileakage(Wmuxdrv3n,Wmuxdrv3p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

	/*dt: doing all the H-tree segments*/
	inrisetime = nextinputtime;
	Tst3 = 0;

	if(nr_muxdrvtreesegments) {
		Ceq = draincap(Wmuxdrv3p,PCH,1)+draincap(Wmuxdrv3n,NCH,1) + 
			  gatecap(3*WmuxdrvtreeN[nr_muxdrvtreesegments-1],0) + Cmuxdrvtreesegments[nr_muxdrvtreesegments-1];
		Req = transreson(Wmuxdrv3n,NCH,1) + Rmuxdrvtreesegments[nr_muxdrvtreesegments-1]; 

		tf = Ceq*Req;
		this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);

		Tst3 += this_delay;
		inrisetime = this_delay/(1.0-VTHINV360x240);

		dynPower+=A*Ceq*.5*VddPow*VddPow;
		lkgCurrent += A*0.5*cmos_ileakage(Wmuxdrv3n,Wmuxdrv3p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}

	for(i=nr_muxdrvtreesegments; i>2;i--) {
		/*dt: this too should alternate...*/
		Ceq = (Cmuxdrvtreesegments[i-2] + draincap(2*WmuxdrvtreeN[i-1],PCH,1)+ draincap(WmuxdrvtreeN[i-1],NCH,1) + 
			  gatecap(3*WmuxdrvtreeN[i-2],0.0));
		Req = (Rmuxdrvtreesegments[i-2] + transreson(WmuxdrvtreeN[i-1],NCH,1));
		tf = Req*Ceq;
		/*dt: This shouldn't be all falling, but interleaved. Have to fix that at some point.*/
        this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);
		Tst3 += this_delay;
		inrisetime = this_delay/(1.0 - VTHINV360x240);

		dynPower+=A*Ceq*.5*VddPow*VddPow;
		lkgCurrent += pow(2,nr_dectreesegments - i)*A*0.5*
			          cmos_ileakage(WmuxdrvtreeN[i-1],2*WmuxdrvtreeN[i-1],Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}

	if(nr_muxdrvtreesegments) {
		Ceq = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0 / FUDGEFACTOR) +
			  + Cdectreesegments[0] + draincap(2*WmuxdrvtreeN[0],PCH,1)+ draincap(WmuxdrvtreeN[0],NCH,1); 
		Rwire = Rmuxdrvtreesegments[0];
		tf = (Rwire + transreson(2*WmuxdrvtreeN[0],PCH,1))*Ceq;
		this_delay = horowitz(inrisetime,tf,VTHINV360x240,VTHOUTDRINV,FALL);;
        Tst3 += this_delay;
		inrisetime = this_delay/VTHOUTDRINV;
		dynPower+=A*Ceq*.5*VddPow*VddPow;
		lkgCurrent += pow(2,nr_muxdrvtreesegments)*A*0.5*cmos_ileakage(WmuxdrvtreeN[0],2*WmuxdrvtreeN[0],Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	
	}
	else {
		Ceq  = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0 / FUDGEFACTOR);
		Ceq += draincap(Wmuxdrv3p,PCH,1) + draincap(Wmuxdrv3n,NCH,1);
		Ceq += GlobalCwordmetal*wirelength_h+ GlobalCbitmetal*wirelength_v;

   		Req  = (GlobalRwordmetal*wirelength_h+GlobalRbitmetal*wirelength_v)/2.0;
		Req += transreson(Wmuxdrv3p,PCH,1);
   		tf = Ceq*Req;
   		Tst3 = horowitz(inrisetime,tf,VTHMUXDRV3,VTHOUTDRINV,FALL);
		//nextinputtime = Tst3/VSINV;
   		*outputtime = Tst3/(VTHOUTDRINV);
   		dynPower+= A*Ceq*VddPow*VddPow;
	}
	
	power->readOp.dynamic = dynPower;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = lkgCurrent * VddPow;


	overall_delay = (Tst1 + Tst2 + Tst3);
	return overall_delay;
}

double Cacti4_2::senseext_driver_delay(int A,char fullyassoc,
				double inputtime,double *outputtime, double wirelength_sense_v,double wirelength_sense_h, powerDef *power)
{
   	double Ceq,Req,tf,nextinputtime;
   	//double Tst1,Tst2,Tst3;
	double Tst1,Tst2;
	double lkgCurrent = 0.0, dynPower=0.0;

   	if(fullyassoc) {
   	     A=1;
   	}

   	/* first driver stage */

   	Ceq = draincap(Wsenseextdrv1p,PCH,1) + draincap(Wsenseextdrv1n,NCH,1) +
   		  gatecap(Wsenseextdrv2n+Wsenseextdrv2p,10.0 / FUDGEFACTOR);
   	Req = transreson(Wsenseextdrv1n,NCH,1);
   	tf = Ceq*Req;
   	Tst1 = horowitz(inputtime,tf,VTHSENSEEXTDRV,VTHSENSEEXTDRV,FALL);
   	nextinputtime = Tst1/VTHSENSEEXTDRV;
   	dynPower+=Ceq*VddPow*VddPow*.5* BITOUT * A * muxover;

	lkgCurrent = 0.5 * BITOUT * A * muxover *cmos_ileakage(Wsenseextdrv1n,Wsenseextdrv1p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
   	/* second driver stage */

   	Ceq = draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) +
   	      GlobalCwordmetal*wirelength_sense_h + GlobalCbitmetal*wirelength_sense_v;

   	Req = (GlobalRwordmetal*wirelength_sense_h + GlobalRbitmetal*wirelength_sense_v)/2.0 + transreson(Wsenseextdrv2p,PCH,1);

   	tf = Ceq*Req;
   	Tst2 = horowitz(nextinputtime,tf,VTHSENSEEXTDRV,VTHOUTDRNAND,RISE);

   	*outputtime = Tst2/(1-VTHOUTDRNAND);
   	dynPower+=Ceq*VddPow*VddPow*.5* BITOUT * A * muxover;

	lkgCurrent = 0.5 * BITOUT * A * muxover  * cmos_ileakage(Wsenseextdrv2n,Wsenseextdrv2p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp); 
   	//   fprintf(stderr, "Pow %f %f\n", Ceq*VddPow*VddPow*.5*BITOUT*A*muxover*1e3, Ceq*VddPow*VddPow*.5*BITOUT*A*muxover*1e3);

	power->readOp.dynamic = dynPower;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = lkgCurrent * VddPow;

   	return(Tst1 + Tst2);

}



double Cacti4_2::total_out_driver_delay (int C,int B,int A,char fullyassoc,int Ndbl,double Nspd,int Ndwl,int Ntbl,int Ntwl,
			int Ntspd,double NSubbanks,double inputtime,double *outputtime,powerDef *power)
{
  powerDef single_power;
  double total_senseext_driver, single_senseext_driver;
  int cols_data_subarray, rows_data_subarray, cols_tag_subarray,
    rows_tag_subarray;
  double subbank_v, subbank_h, sub_v, sub_h, inter_v, inter_h, htree;
  int htree_int, tagbits;
  int cols_fa_subarray, rows_fa_subarray;

  reset_powerDef(&single_power);
  total_senseext_driver = 0.0;
  single_senseext_driver = 0.0;

  if (!fullyassoc)
    {
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
	  //the final int value is the correct one 
      //cols_data_subarray = (8 * B * A * Nspd / Ndwl);
      //rows_data_subarray = (C / (B * A * Ndbl * Nspd));
	  cols_data_subarray = (int)(8 * B * A * Nspd / Ndwl + EPSILON);
      rows_data_subarray = (int)(C / (B * A * Ndbl * Nspd) + EPSILON);

      if (Ndwl * Ndbl == 1)
	{
	  sub_v = rows_data_subarray;
	  sub_h = cols_data_subarray;
	}
      if (Ndwl * Ndbl == 2)
	{
	  sub_v = rows_data_subarray;
	  sub_h = 2 * cols_data_subarray;
	}

      if (Ndwl * Ndbl > 2)
	{
	  htree = logtwo ((double) (Ndwl * Ndbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ndwl * Ndbl) * rows_data_subarray;
	      sub_h = sqrt (Ndwl * Ndbl) * cols_data_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ndwl * Ndbl / 2) * rows_data_subarray;
	      sub_h = 2 * sqrt (Ndwl * Ndbl / 2) * cols_data_subarray;
	    }
	}
      inter_v = sub_v;
      inter_h = sub_h;

      rows_tag_subarray = C / (B * A * Ntbl * Ntspd);

	  if(!force_tag) {
	     //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
         //the final int value is the correct one 
		 //tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) +
			//(	int) logtwo ((double) A) - (int) (logtwo (NSubbanks));
		 tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) +
			(int) logtwo ((double) A) - (int) (logtwo (NSubbanks)) + EPSILON);
	  }
	  else {
		  tagbits = force_tag_size;
	  }
      cols_tag_subarray = tagbits * A * Ntspd / Ntwl;

      if (Ntwl * Ntbl == 1)
	{
	  sub_v = rows_tag_subarray;
	  sub_h = cols_tag_subarray;
	}
      if (Ntwl * Ntbl == 2)
	{
	  sub_v = rows_tag_subarray;
	  sub_h = 2 * cols_tag_subarray;
	}

      if (Ntwl * Ntbl > 2)
	{
	  htree = logtwo ((double) (Ntwl * Ntbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ntwl * Ntbl) * rows_tag_subarray;
	      sub_h = sqrt (Ntwl * Ntbl) * cols_tag_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ntwl * Ntbl / 2) * rows_tag_subarray;
	      sub_h = 2 * sqrt (Ntwl * Ntbl / 2) * cols_tag_subarray;
	    }
	}

      inter_v = MAX (sub_v, inter_v);
      inter_h += sub_h;
      subbank_h = inter_h;
      subbank_v = inter_v;
    }
  else
    {
      rows_fa_subarray = (C / (B * Ndbl));
	  if(!force_tag) {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) B);
		tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) B) + EPSILON);
	  }
	  else {
		  tagbits = force_tag_size;
	  }
      cols_tag_subarray = tagbits;
      cols_fa_subarray = (8 * B) + cols_tag_subarray;

      if (Ndbl == 1)
	{
	  sub_v = rows_fa_subarray;
	  sub_h = cols_fa_subarray;
	}
      if (Ndbl == 2)
	{
	  sub_v = rows_fa_subarray;
	  sub_h = 2 * cols_fa_subarray;
	}

      if (Ndbl > 2)
	{
	  htree = logtwo ((double) (Ndbl));
	  //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
      //the final int value is the correct one 
	  //htree_int = (int) htree;
	  htree_int = (int) (htree + EPSILON);
	  if (htree_int % 2 == 0)
	    {
	      sub_v = sqrt (Ndbl) * rows_fa_subarray;
	      sub_h = sqrt (Ndbl) * cols_fa_subarray;
	    }
	  else
	    {
	      sub_v = sqrt (Ndbl / 2) * rows_fa_subarray;
	      sub_h = 2 * sqrt (Ndbl / 2) * cols_fa_subarray;
	    }
	}
      inter_v = sub_v;
      inter_h = sub_h;

      subbank_v = inter_v;
      subbank_h = inter_h;
    }


  if (NSubbanks == 1.0 || NSubbanks == 2.0)
    {
      subbank_h = 0;
      subbank_v = 0;
      single_senseext_driver =
	senseext_driver_delay (A, fullyassoc,inputtime,outputtime,
							subbank_v, subbank_h, &single_power);
      total_senseext_driver += single_senseext_driver;
      add_powerDef(power,single_power,*power);
    }

  while (NSubbanks > 2.0)
    {
      if (NSubbanks == 4.0)
	{
	  subbank_h = 0;
	}

      single_senseext_driver =
	senseext_driver_delay (A, fullyassoc, inputtime, outputtime, subbank_v,
			       subbank_h, &single_power);

      NSubbanks = NSubbanks / 4.0;
      subbank_v = 2 * subbank_v;
      subbank_h = 2 * subbank_h;
      inputtime = *outputtime;
      total_senseext_driver += single_senseext_driver;
      add_powerDef(power,single_power,*power);
    }
  return (total_senseext_driver);

}

/* Valid driver (see section 6.9 of tech report)
   Note that this will only be called for a direct mapped cache */

double Cacti4_2::valid_driver_delay (int C,int B,int A,char fullyassoc,int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,
		    double *NSubbanks,double inputtime,powerDef *power)
{
  double Ceq, Tst1, tf;
  int rows, tagbits, cols, l_valdrv_v = 0, l_valdrv_h = 0;
  double wire_cap, wire_res;
  double subbank_v, subbank_h;
  int nr_subarrays_left = 0, v_or_h = 0;
  int horizontal_step = 0, vertical_step = 0;

  rows = C / (8 * B * A * Ntbl * Ntspd);

  if(!force_tag) {
    //v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
    //the final int value is the correct one 
	//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) + (int) logtwo ((double) A) -
			//(int) (logtwo (*NSubbanks));
	tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS - (int) logtwo ((double) C) + (int) logtwo ((double) A) -
			(int) (logtwo (*NSubbanks)) + EPSILON);
  }
  else {
	  tagbits = force_tag_size;
  }
  cols = tagbits * A * Ntspd / Ntwl;

  /* calculate some layout info */
  
  if (Ntwl * Ntbl == 1)
    {
      l_valdrv_v = 0;
      l_valdrv_h = cols;
    }
  if (Ntwl * Ntbl == 2 || Ntwl * Ntbl == 4)
    {
      l_valdrv_v = 0;
      l_valdrv_h = cols;
    }
	else {
		nr_subarrays_left = Ntwl*Ntbl;
		nr_subarrays_left /= 4;
		horizontal_step = cols;
		vertical_step = C/(B*A*Ntbl*Ntspd);
		l_valdrv_h = horizontal_step;
		horizontal_step *= 2;
		v_or_h = 1; // next step is vertical
		
		while(nr_subarrays_left > 1) {
			if(v_or_h) {
				l_valdrv_v += vertical_step;
				v_or_h = 0;
				vertical_step *= 2;
				nr_subarrays_left /= 2;
			}
			else {
				l_valdrv_h += horizontal_step;
				v_or_h = 1;
				horizontal_step *= 2;
				nr_subarrays_left /= 2;
			}
		}
  }

  subbank_routing_length (C, B, A, fullyassoc, Ndbl, Nspd, Ndwl, Ntbl, Ntwl,
			  Ntspd, *NSubbanks, &subbank_v, &subbank_h);

  wire_cap =
    GlobalCbitmetal * (l_valdrv_v + subbank_v) + GlobalCwordmetal * (l_valdrv_h +
							 subbank_h);
  wire_res =
    GlobalRwordmetal * (l_valdrv_h + subbank_h) * 0.5 + GlobalRbitmetal * (l_valdrv_v +
							       subbank_v) * 0.5;

  Ceq =
    draincap (Wmuxdrv12n, NCH, 1) + draincap (Wmuxdrv12p, PCH,
					      1) + wire_cap + Cout;
  tf = Ceq * (transreson (Wmuxdrv12p, PCH, 1) + wire_res);
  Tst1 = horowitz (inputtime, tf, VTHMUXDRV1, 0.5, FALL);
  power->readOp.dynamic += Ceq * VddPow * VddPow;

  return (Tst1);
}

double Cacti4_2::half_compare_delay(int C,int B,int A,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,double inputtime,double *outputtime,powerDef *power)
{
   	double Req,Ceq,tf,st1del,st2del,st3del,nextinputtime,m;
   	double c1,c2,r1,r2,tstep,a,b,c;
   	double Tcomparatorni;
   	int cols,tagbits;
   	double lkgCurrent=0.0, dynPower=0.0;
	int v_or_h = 0, hori = 0, vert = 0,
	    horizontal_step = 0, vertical_step = 0 , nr_subarrays_left = 0;
	double Wxtramuxdrv1n = 0, Wxtramuxdrv1p = 0, Wxtramuxdrv2n = 0, Wxtramuxdrv2p = 0;
	double Cload = 0;
	double muxdrv1del = 0, muxdrv2del = 0;

   	/* First Inverter */

   	Ceq = gatecap(Wcompinvn2+Wcompinvp2,10.0 / FUDGEFACTOR) +
   	      draincap(Wcompinvp1,PCH,1) + draincap(Wcompinvn1,NCH,1);
   	Req = transreson(Wcompinvp1,PCH,1);
   	tf = Req*Ceq;
   	st1del = horowitz(inputtime,tf,VTHCOMPINV,VTHCOMPINV,FALL);
   	nextinputtime = st1del/VTHCOMPINV;
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	lkgCurrent = 0.5 * A * cmos_ileakage(Wcompinvn1,Wcompinvp1,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
   	/* Second Inverter */

   	Ceq = gatecap(Wcompinvn3+Wcompinvp3,10.0 / FUDGEFACTOR) +
   	      draincap(Wcompinvp2,PCH,1) + draincap(Wcompinvn2,NCH,1);
   	Req = transreson(Wcompinvn2,NCH,1);
   	tf = Req*Ceq;
   	st2del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHCOMPINV,RISE);
   	nextinputtime = st2del/(1.0-VTHCOMPINV);
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	lkgCurrent += 0.5 * A * cmos_ileakage(Wcompinvn2,Wcompinvp2,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

   	/* Third Inverter */

   	Ceq = gatecap(Wevalinvn+Wevalinvp,10.0 / FUDGEFACTOR) +
   	      draincap(Wcompinvp3,PCH,1) + draincap(Wcompinvn3,NCH,1);
   	Req = transreson(Wcompinvp3,PCH,1);
   	tf = Req*Ceq;
   	st3del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHEVALINV,FALL);
   	nextinputtime = st3del/(VTHEVALINV);
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	lkgCurrent += 0.5 * A * cmos_ileakage(Wcompinvn3,Wcompinvp3,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);


   	/* Final Inverter (virtual ground driver) discharging compare part */

	/*dt: Certainly don't need the dirty bit in comparison. The valid bit is assumed to be handled elsewhere, because we don't need to route any data to compare with to it.*/
	if(!force_tag) {
		//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
        //the final int value is the correct one 
   		//tagbits = ADDRESS_BITS - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks));
		tagbits = (int) (ADDRESS_BITS - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks)) + EPSILON);
		/*dt: we have to round up in case we have an odd number of bits in the tag, to account for the longer path */
		if ((tagbits % 4) == 0) {
   			tagbits=tagbits/4;
		}
		else {
			tagbits=tagbits/4 + 1;
		}
	}
	else {
		tagbits = force_tag_size;
	}
	cols = 0;
	if(Ntwl*Ntbl==1) {
	       	vert = 0;
	        hori = 4*tagbits*A*Ntspd/Ntwl;
	}
    else if(Ntwl*Ntbl==2 || Ntwl*Ntbl==4) {
            vert = 0;
            hori = 4*tagbits*A*Ntspd/Ntwl;
    }
    else if(Ntwl*Ntbl>4) {
            nr_subarrays_left = Ntwl* Ntbl;
			nr_subarrays_left /= 4;
            horizontal_step = 4*tagbits*A*Ntspd/Ntwl;
            vertical_step = C/(B*A*Ntbl*Ntspd);
            hori = horizontal_step;
            horizontal_step *= 2;
            v_or_h = 1; // next step is vertical
            
            while(nr_subarrays_left > 1) {
                if(v_or_h) {
                    vert += vertical_step;
                    v_or_h = 0;
                    vertical_step *= 2;
                    nr_subarrays_left /= 2;
                }
                else {
                    hori += horizontal_step;
                    v_or_h = 1;
                    horizontal_step *= 2;
                    nr_subarrays_left /= 2;
             }
         }
     }
	
	/*dt: I think we need to add at least one buffer (probably more) between the comparator output gate and the mux driver ('cause it's biig!)*/
	
	
	Ceq = gatecap(WmuxdrvNANDn+WmuxdrvNANDp,20.0 / FUDGEFACTOR) +
   	      hori*GlobalCwordmetal + vert*GlobalCbitmetal;
	Cload = Ceq / gatecap(1.0,0.0);
   	Wxtramuxdrv2p = Cload * SizingRatio * 2/3;
   	Wxtramuxdrv2n = Cload * SizingRatio /3;

	/*dt: changing this to a nand*/
	Ceq = gatecap(Wxtramuxdrv2n+Wxtramuxdrv2p,20.0 / FUDGEFACTOR);
	Cload = Ceq / gatecap(1.0,0.0);
   	Wxtramuxdrv1p = Cload * SizingRatio * 2/3;
   	Wxtramuxdrv1n = 2 * Cload * SizingRatio /3;

   	r1 = transreson(Wcompn,NCH,2);
   	r2 = transreson(Wevalinvn,NCH,1); /* was switch */
   	c2 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
   	      draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);
   	c1 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))
   	     +draincap(Wcompp,PCH,1) + 
		 + gatecap(Wxtramuxdrv1n+Wxtramuxdrv1p,5.0 / FUDGEFACTOR);

   	dynPower+=c2*VddPow*VddPow*2*A;
   	dynPower+=c1*VddPow*VddPow*(A-1);

	lkgCurrent += 0.5 * A * cmos_ileakage(Wevalinvn,Wevalinvp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

	// double the tag_bits as the delay is being computed for only
	// half the number. The leakage is still for the total tag bits

	// the comparator compares cols number of bits. Since these transistors are
	// stacked, a stacking factor of 0.2 is used
	lkgCurrent += 2 * cols * 0.2 * 0.5 * A * cmos_ileakage(Wcompn,Wcompp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

	// leakage due to the mux driver
	lkgCurrent += 0.5 * A * cmos_ileakage(Wmuxdrv12n,Wmuxdrv12p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
		
   	/* time to go to threshold of mux driver */

   	tstep = (r2*c2+(r1+r2)*c1)*log(1.0/VTHMUXNAND);

   	/* take into account non-zero input rise time */

   	m = Vdd/nextinputtime;

   	if ((tstep) <= (0.5*(Vdd-Vt)/m)) {
  		a = m;
	    b = 2*((Vdd*VTHEVALINV)-Vt);
        c = -2*(tstep)*(Vdd-Vt)+1/m*((Vdd*VTHEVALINV)-Vt)*((Vdd*VTHEVALINV)-Vt);
 	    Tcomparatorni = (-b+sqrt(b*b-4*a*c))/(2*a);
   	} else {
		Tcomparatorni = (tstep) + (Vdd+Vt)/(2*m) - (Vdd*VTHEVALINV)/m;
   	}
   	nextinputtime = Tcomparatorni/(1.0-VTHMUXNAND);
	//*outputtime = Tcomparatorni/(1.0-VTHMUXNAND);

	/*dt: first mux buffer */

	Ceq = gatecap(Wxtramuxdrv2n+Wxtramuxdrv2p,20.0 / FUDGEFACTOR) +
   	      2 * draincap(Wxtramuxdrv1p,PCH,1) + draincap(Wxtramuxdrv1n,NCH,1);
   	Req = transreson(Wxtramuxdrv1p,PCH,1);
   	tf = Req*Ceq;
   	muxdrv1del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHCOMPINV,FALL);
   	nextinputtime = muxdrv2del/VTHCOMPINV;
   	dynPower+=Ceq*VddPow*VddPow;

	lkgCurrent = 0.5 * A * cmos_ileakage(Wxtramuxdrv2n,Wxtramuxdrv2p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	

	/*dt: last mux buffer */
	
	Ceq = gatecap(WmuxdrvNANDn+WmuxdrvNANDp,20.0 / FUDGEFACTOR) +
   	      hori*GlobalCwordmetal + vert*GlobalCbitmetal + 
   	      draincap(Wxtramuxdrv2p,PCH,1) + draincap(Wxtramuxdrv2n,NCH,1);
   	Req = transreson(Wxtramuxdrv2p,PCH,1);
   	tf = Req*Ceq;
   	muxdrv2del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHCOMPINV,FALL);
   	*outputtime = muxdrv1del/VTHCOMPINV;
   	dynPower+=Ceq*VddPow*VddPow;

	lkgCurrent = 0.5 * A * cmos_ileakage(Wxtramuxdrv1n,Wxtramuxdrv1p,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	
	
	power->readOp.dynamic = dynPower;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = lkgCurrent * VddPow;

   	return(Tcomparatorni+st1del+st2del+st3del+muxdrv1del+muxdrv2del);
}
/*----------------------------------------------------------------------*/

/* Data output delay (data side) -- see section 6.8
   This is the time through the NAND/NOR gate and the final inverter 
   assuming sel is already present */

double Cacti4_2::dataoutput_delay (int C,int B,int A,char fullyassoc,int Ndbl,double Nspd,int Ndwl,
		  double inrisetime,double *outrisetime,powerDef *power)
{
    //double Ceq,Req,Rwire,Rline;
	double Ceq,Req,Rwire;
    double tf;
    double nordel,outdel,nextinputtime;
	double l_outdrv_v = 0,l_outdrv_h = 0;
	//int rows,cols,rows_fa_subarray,cols_fa_subarray;
	int rows_fa_subarray,cols_fa_subarray;
	//double htree, Cload;
	double htree;
	int htree_int,exp,tagbits;
	double lkgCurrent = 0.0, dynPower=0.0;
	int v_or_h = 0, nr_subarrays_left = 0, vertical_step = 0, horizontal_step = 0;
	double this_delay;
	int i;
	/*dt: if we have fast cache mode, all A ways are routed to the edge of the data array*/
	int routed_ways = fast_cache_access_flag ? A : 1;

	if(!fullyassoc) {
  	}
  	else {
  		rows_fa_subarray = (C/(B*Ndbl));
		if(!force_tag) {
			//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
			//the final int value is the correct one 
    		//tagbits = ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)B);
			tagbits = (int) (ADDRESS_BITS + EXTRA_TAG_BITS-(int)logtwo((double)B) + EPSILON);
		}
		else {
			tagbits = force_tag_size;
		}
        cols_fa_subarray = (CHUNKSIZE*B)+tagbits;
        if(Ndbl==1) {
    	    l_outdrv_v= 0;
            l_outdrv_h= cols_fa_subarray;
        }
        if(Ndbl==2 || Ndbl==4) {
            l_outdrv_v= 0;
            l_outdrv_h= 2*cols_fa_subarray;
        }
        if(Ndbl>4) {
            htree=logtwo((double)(Ndbl));
			//v4.1: Fixing double->int type conversion problems. EPSILON is added below to make sure
            //the final int value is the correct one 
            //htree_int = (int) htree;
			htree_int = (int) (htree + EPSILON);
            if (htree_int % 2 ==0) {
            	exp = (htree_int/2-1);
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl)*cols_fa_subarray;
            }
            else {
                exp = (htree_int+1)/2-1;
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl/2)*cols_fa_subarray;
            }
        }
	}
    /* Delay of NOR gate */

    Ceq = 2*draincap(Woutdrvnorn,NCH,1)+draincap(Woutdrvnorp,PCH,2)+
          gatecap(Woutdrivern,10.0 / FUDGEFACTOR);

    tf = Ceq*transreson(Woutdrvnorp,PCH,2);
    nordel = horowitz(inrisetime,tf,VTHOUTDRNOR,VTHOUTDRIVE,FALL);
    nextinputtime = nordel/(VTHOUTDRIVE);
	dynPower+=Ceq*VddPow*VddPow*.5*BITOUT;

	lkgCurrent += 0.5 * BITOUT * routed_ways * muxover * cmos_ileakage(Woutdrvseln,Woutdrvselp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	lkgCurrent += 0.5 * 0.2 * BITOUT * routed_ways * muxover * cmos_ileakage(Woutdrvnorn,Woutdrvnorp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	lkgCurrent += 0.5 * 0.2 * BITOUT * routed_ways * muxover * cmos_ileakage(Woutdrvnandn,Woutdrvnandp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);


    /* Delay of output driver tree*/

	if(nr_outdrvtreesegments) {
		Ceq = (draincap(Woutdrivern,NCH,1)+draincap(Woutdriverp,PCH,1))*A*muxover
			+ Coutdrvtreesegments[0] + gatecap(3*WoutdrvtreeN[1],0.0);	
	}
	else {
		Ceq = (draincap(Woutdrivern,NCH,1)+draincap(Woutdriverp,PCH,1))*A*muxover
			+ Coutdrvtreesegments[0] + gatecap(Wsenseextdrv1n+Wsenseextdrv1p,10.0 / FUDGEFACTOR);
	}
	Rwire = Routdrvtreesegments[0];

	dynPower+= routed_ways * Ceq*VddPow*VddPow*.5*BITOUT;//factor of routed_ways added by Shyam

	lkgCurrent += 0.5 * BITOUT * routed_ways * muxover * cmos_ileakage(Woutdrivern,Woutdriverp,Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);

    tf = Ceq*(transreson(Woutdrivern,NCH,1)+Rwire);
    outdel = horowitz(nextinputtime,tf,VTHOUTDRIVE,0.5,RISE);

	*outrisetime = outdel/(1.0 - VTHOUTDRIVE);

	/*dt: doing all the H-tree segments*/

	if(nr_outdrvtreesegments) {
		for(i=1; i<nr_outdrvtreesegments-1;i++) {
			/*dt: this too should alternate...*/
			Ceq = (Coutdrvtreesegments[i] + draincap(2*WoutdrvtreeN[i],PCH,1)+ draincap(WoutdrvtreeN[i],NCH,1) + 
				gatecap(3*WoutdrvtreeN[i],0.0));
			Req = (Routdrvtreesegments[i] + transreson(WoutdrvtreeN[i],NCH,1));
			tf = Req*Ceq;
			/*dt: This shouldn't be all falling, but interleaved. Have to fix that at some point.*/
			this_delay = horowitz(*outrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);
			outdel += this_delay;
			*outrisetime = this_delay/(1.0 - VTHINV360x240);

			dynPower+= routed_ways*BITOUT*Ceq*.5*VddPow*VddPow;
			lkgCurrent += pow(2,nr_outdrvtreesegments - i)*routed_ways*BITOUT*0.5*
				          cmos_ileakage(WoutdrvtreeN[i],2*WoutdrvtreeN[i],Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
		}
		Ceq = Coutdrvtreesegments[nr_outdrvtreesegments-1] + draincap(2*WoutdrvtreeN[nr_outdrvtreesegments-1],PCH,1)+ 
			   draincap(WoutdrvtreeN[nr_outdrvtreesegments-1],NCH,1) + gatecap(Wsenseextdrv1n+Wsenseextdrv1p,10.0 / FUDGEFACTOR);
		Req = (Routdrvtreesegments[nr_outdrvtreesegments-1] + transreson(WoutdrvtreeN[nr_outdrvtreesegments-1],NCH,1));
		tf = Req*Ceq;
		/*dt: This shouldn't be all falling, but interleaved. Have to fix that at some point.*/
		this_delay = horowitz(*outrisetime,tf,VTHINV360x240,VTHINV360x240,RISE);
		outdel += this_delay;
		*outrisetime = this_delay/(1.0 - VTHINV360x240);
		dynPower+=routed_ways*BITOUT*Ceq*.5*VddPow*VddPow;
		lkgCurrent += routed_ways*BITOUT*0.5*cmos_ileakage(WoutdrvtreeN[nr_outdrvtreesegments-1],2*WoutdrvtreeN[nr_outdrvtreesegments-1],
				          Vt_bit_nmos_low,Vthn,Vt_bit_pmos_low,Vthp);
	}

    //*outrisetime = outdel/0.5;

    power->readOp.dynamic = dynPower;
    power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
    power->writeOp.leakage = lkgCurrent * VddPow;

   	return(outdel+nordel);
}

/*----------------------------------------------------------------------*/

/* Sel inverter delay (part of the output driver)  see section 6.8 */

double Cacti4_2::selb_delay_tag_path (double inrisetime,double *outrisetime,powerDef *power)
{
  double Ceq, Tst1, tf;

  Ceq = draincap (Woutdrvseln, NCH, 1) + draincap (Woutdrvselp, PCH, 1) +
    gatecap (Woutdrvnandn + Woutdrvnandp, 10.0 / FUDGEFACTOR);
  tf = Ceq * transreson (Woutdrvseln, NCH, 1);
  Tst1 = horowitz (inrisetime, tf, VTHOUTDRINV, VTHOUTDRNAND, RISE);
  *outrisetime = Tst1 / (1.0 - VTHOUTDRNAND);
  power->readOp.dynamic += Ceq * VddPow * VddPow;

  return (Tst1);
}


/*----------------------------------------------------------------------*/

/* This routine calculates the extra time required after an access before
 * the next access can occur [ie.  it returns (cycle time-access time)].
 */

double Cacti4_2::precharge_delay (double worddata)
{
  double Ceq, tf, pretime;

  /* as discussed in the tech report, the delay is the delay of
     4 inverter delays (each with fanout of 4) plus the delay of
     the wordline */

  Ceq = draincap (Wdecinvn, NCH, 1) + draincap (Wdecinvp, PCH, 1) +
    4 * gatecap (Wdecinvn + Wdecinvp, 0.0);
  tf = Ceq * transreson (Wdecinvn, NCH, 1);
  pretime = 4 * horowitz (0.0, tf, 0.5, 0.5, RISE) + worddata;

  return (pretime);
}

void Cacti4_2::calc_wire_parameters(parameter_type *parameters) {
	if (parameters->fully_assoc)
    /* If model is a fully associative cache - use larger cell size */
    {
      FACbitmetal =
	((32 / FUDGEFACTOR +
	  2 * WIREPITCH *
	  ((parameters->
	    num_write_ports + parameters->num_readwrite_ports - 1) +
	   parameters->num_read_ports)) * Default_Cmetal);
      FACwordmetal =
	((BitWidth +
	  2 * WIREPITCH *
	  ((parameters->
	    num_write_ports + parameters->num_readwrite_ports - 1)) +
	  WIREPITCH * (parameters->num_read_ports)) * Default_Cmetal);
	  //v4.1: Fixing an error in the resistance equations below. WIREHEIGHTRATIO is replaced by
	  //WIREWIDTH. 
      /*FARbitmetal =
	(((32 +
	   2 * WIREPITCH *
	   ((parameters->
	     num_write_ports + parameters->num_readwrite_ports - 1) +
	    parameters->num_read_ports))) * 
		(Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR));
      FARwordmetal =
	(((BitWidth +
	   2 * WIREPITCH *
	   ((parameters->
	     num_write_ports + parameters->num_readwrite_ports - 1)) +
	   WIREPITCH * (parameters->num_read_ports))) * (Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR));*/

	  FARbitmetal =
	(((32 / FUDGEFACTOR +
	   2 * WIREPITCH *
	   ((parameters->
	     num_write_ports + parameters->num_readwrite_ports - 1) +
	    parameters->num_read_ports))) * 
		(Default_CopperSheetResistancePerMicroM /WIREWIDTH * FUDGEFACTOR));
      FARwordmetal =
	(((BitWidth +
	   2 * WIREPITCH *
	   ((parameters->
	     num_write_ports + parameters->num_readwrite_ports - 1)) +
	   WIREPITCH * (parameters->num_read_ports))) * (Default_CopperSheetResistancePerMicroM /WIREWIDTH * FUDGEFACTOR));
    }
	/*dt: Capacitance gets better with scaling, because C = K*epsilon*L*(W/x_ox + H/L_s), where
	      W: width of the wire, L: length of the wire, H: height of the wire, x_ox: thickness of the oxide separating two wires
		  vertically, L_s: thickness of the insulator separating two wires horizontally 
		  K and epsilon are constants
		  To a first order all the variables scale with featuresize => C~ const. * f^2/f = const. * f 
		  Note that FUDGEFACTOR is the inverse of featuresize
		  Since we're going to divide by FUDGEFACTOR in the end anyway, I'm leaving this alone and just adding this comment for future reference.
    */
  Cbitmetal =
    ((BitHeight +
      2 * WIREPITCH *
      ((parameters->num_write_ports + parameters->num_readwrite_ports - 1) +
       parameters->num_read_ports)) * Default_Cmetal);
  Cwordmetal =
    ((BitWidth +
      2 * WIREPITCH *
      ((parameters->num_write_ports + parameters->num_readwrite_ports - 1)) +
      WIREPITCH * (parameters->num_read_ports)) * Default_Cmetal);
  /* dt: Resistance gets worse, since R = rho*L/(W*H), where L, W, H are the length, width and height of the wire.
     All scale with featuresize, so R ~ const*f/f^2 = const./f
	 Note that FUDGEFACTOR is the inverse of featuresize
  */

  //v4.1: Fixing an error in the resistance equations below. WIREHEIGHTRATIO is replaced by
  // WIREWIDTH. 
  /*Rbitmetal =
    (((BitHeight +
       2 * WIREPITCH *
       ((parameters->num_write_ports + parameters->num_readwrite_ports - 1) + parameters->num_read_ports))) * 
	   (Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR));
  Rwordmetal =
    (((BitWidth +
       2 * WIREPITCH *
       ((parameters->num_write_ports + parameters->num_readwrite_ports - 1)) + WIREPITCH * (parameters->num_read_ports))) * 
	   (Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR));*/

  Rbitmetal =
    (((BitHeight +
       2 * WIREPITCH *
       ((parameters->num_write_ports + parameters->num_readwrite_ports - 1) + parameters->num_read_ports))) * 
	   (Default_CopperSheetResistancePerMicroM /WIREWIDTH * FUDGEFACTOR));
  Rwordmetal =
    (((BitWidth +
       2 * WIREPITCH *
       ((parameters->num_write_ports + parameters->num_readwrite_ports - 1)) + WIREPITCH * (parameters->num_read_ports))) * 
	   (Default_CopperSheetResistancePerMicroM /WIREWIDTH * FUDGEFACTOR));

  /*dt: tags only need one RW port for when a line is evicted. All other ports can be simple read ports */
  TagCbitmetal = (BitHeight + 2 * WIREPITCH * (parameters->num_read_ports + parameters->num_readwrite_ports - 1)) * 
					Default_Cmetal;
  /*dt: the +1 is because a RW port needs two bitlines, thus longer wordlines */
  TagCwordmetal = (BitWidth + WIREPITCH * (parameters->num_read_ports + 2*(parameters->num_readwrite_ports - 1))) * 
					Default_Cmetal; 
  /*TagRbitmetal = (BitHeight + 2 * WIREPITCH * (parameters->num_read_ports + parameters->num_readwrite_ports - 1))  * 
				 (Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR);
  TagRwordmetal = (BitWidth + WIREPITCH * (parameters->num_read_ports + 2*(parameters->num_readwrite_ports - 1))) * 
	              (Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR);*/

  TagRbitmetal = (BitHeight + 2 * WIREPITCH * (parameters->num_read_ports + parameters->num_readwrite_ports - 1))  * 
				 (Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR * FUDGEFACTOR);
  TagRwordmetal = (BitWidth + WIREPITCH * (parameters->num_read_ports + 2*(parameters->num_readwrite_ports - 1))) * 
	              (Default_CopperSheetResistancePerMicroM /WIREHEIGTHRATIO * FUDGEFACTOR * FUDGEFACTOR);

  GlobalCbitmetal = CRatiolocal_to_interm*Cbitmetal;
  GlobalCwordmetal = CRatiolocal_to_interm*Cwordmetal;

  GlobalRbitmetal = RRatiolocal_to_interm*Rbitmetal; 
  GlobalRwordmetal = RRatiolocal_to_interm*Rwordmetal; 
}

/*======================================================================*/

/*======================================================================*/


void Cacti4_2::calculate_time (result_type *result,arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters, double *NSubbanks)
{
	arearesult_type arearesult_temp;
	area_type arearesult_subbanked_temp;
	
	int i;
	//int Ndwl, Ndbl, Ntwl, Ntbl, Ntspd, inter_subbanks, rows, columns,
	//tag_driver_size1, tag_driver_size2;
	int Ndwl, Ndbl, Ntwl, Ntbl, Ntspd, rows, columns;
	double Nspd;
	double Subbank_Efficiency, Total_Efficiency, max_efficiency, efficiency,
	min_efficiency;
	double max_aspect_ratio_total, aspect_ratio_total_temp;
	//double bank_h, bank_v, subbank_h, subbank_v, inter_h, inter_v;
	double bank_h, bank_v, subbank_h, subbank_v;
	double wirelength_v, wirelength_h;
	double access_time = 0;
	powerDef total_power;
	double before_mux = 0.0, after_mux = 0;
	powerDef total_power_allbanks;
	powerDef total_address_routing_power, total_power_without_routing;
	double subbank_address_routing_delay = 0.0;
	powerDef subbank_address_routing_power;
	double decoder_data_driver = 0.0, decoder_data_3to8 =
	0.0, decoder_data_inv = 0.0;
	double decoder_data = 0.0, decoder_tag = 0.0, wordline_data =
	0.0, wordline_tag = 0.0;
	powerDef decoder_data_power, decoder_tag_power, wordline_data_power, wordline_tag_power;
	double decoder_tag_driver = 0.0, decoder_tag_3to8 = 0.0, decoder_tag_inv =
	0.0;
	double bitline_data = 0.0, bitline_tag = 0.0, sense_amp_data =
	0.0, sense_amp_tag = 0.0;
	powerDef sense_amp_data_power, sense_amp_tag_power, bitline_tag_power, bitline_data_power;
	double compare_tag = 0.0, mux_driver = 0.0, data_output = 0.0, selb = 0.0;
	powerDef compare_tag_power, selb_power, mux_driver_power, valid_driver_power;
	powerDef data_output_power;
	double time_till_compare = 0.0, time_till_select = 0.0, valid_driver = 0.0;
	double cycle_time = 0.0, precharge_del = 0.0;
	double outrisetime = 0.0, inrisetime = 0.0, addr_inrisetime = 0.0;
	double senseext_scale = 1.0;
	double total_out_driver = 0.0;
	powerDef total_out_driver_power;
	double scale_init;
	int data_nor_inputs = 1, tag_nor_inputs = 1;
	double tag_delay_part1 = 0.0, tag_delay_part2 =
	0.0, tag_delay_part3 = 0.0, tag_delay_part4 = 0.0, tag_delay_part5 =
	0.0, tag_delay_part6 = 0.0;
	double max_delay = 0;
	int counter;
	double Tpre,Tpre_tag;
	int allports = 0, allreadports = 0, allwriteports = 0;
	int tag_iteration_counter = 0;
	double colsfa, desiredrisetimefa, Clinefa, Rpdrivefa; //Added by Shyam to incorporate FA caches 
	//in v4.0
	
	
	cached_tag_entry cached_tag_calculations[MAX_CACHE_ENTRIES];
	cached_tag_entry current_cache_entry;
	
	int valid_tag_cache_entries[MAX_CACHE_ENTRIES];
	
	for(i=0;i<MAX_CACHE_ENTRIES;i++)
	{
		valid_tag_cache_entries[i] = FALSE;
	}
	
	rows = parameters->number_of_sets;
	columns = 8 * parameters->block_size * parameters->data_associativity;
	FUDGEFACTOR = parameters->fudgefactor;
	VddPow = 4.5 / (pow (FUDGEFACTOR, (2.0 / 3.0)));
	
	/*dt: added to simulate accessing the tags and then only accessing one data way*/
	sequential_access_flag = parameters->sequential_access;
	/*dt: added to simulate fast and power hungry caches which route all the ways to the edge of the
		data array and do the way select there, instead of waiting for the way select signal to travel
		all the way to the subarray.
	*/
	fast_cache_access_flag = parameters->fast_access;
	pure_sram_flag = parameters->pure_sram;
	/* how many bits should this cache output?*/
	BITOUT = parameters->nr_bits_out;
	
	if (VddPow < 0.7)
		VddPow = 0.7;
	if (VddPow > 5.0)
		VddPow = 5.0;
	VbitprePow = VddPow * 3.3 / 4.5;
	parameters->VddPow = VddPow;
	
	/* go through possible Ndbl,Ndwl and find the smallest */
	/* Because of area considerations, I don't think it makes sense
		to break either dimension up larger than MAXN */
	
	/* only try moving output drivers for set associative cache */
	if (parameters->tag_associativity != 1)
	{
		scale_init = 0.1;
	}
	else
	{
		scale_init = 1.0;
	}
	precalc_leakage_params(VddPow,Tkelvin,Tox, tech_length0);
	calc_wire_parameters(parameters);
	
	result->cycle_time = BIGNUM;
	result->access_time = BIGNUM;
	result->total_power.readOp.dynamic = result->total_power.readOp.leakage = 
		result->total_power.writeOp.dynamic = result->total_power.writeOp.leakage =BIGNUM;
	result->best_muxover = 8 * parameters->block_size / BITOUT;
	result->max_access_time = 0;
	result->max_power = 0;
	arearesult_temp.max_efficiency = 1.0 / BIGNUM;
	max_efficiency = 1.0 / BIGNUM;
	min_efficiency = BIGNUM;
	arearesult->efficiency = 1.0 / BIGNUM;
	max_aspect_ratio_total = 1.0 / BIGNUM;
	arearesult->aspect_ratio_total = BIGNUM;
	
	
	
	for (counter = 0; counter < 2; counter++)
	{
		if (!parameters->fully_assoc)
		{
			/* Set associative or direct map cache model */
			
			/* 
			Varies the number of sets mapped to each wordline. 
			This makes each wordline longer but reduces the length of the bitlines.
			It also requires extra muxes before the sense amplifiers to select the right set.
			*/
			//v4.1: We now consider fractions of Nspd while exploring the partitioning
			//space. The lower bound of Nspd is BITOUT /(parameters->block_size*8).
			for (Nspd = (double)(BITOUT)/(double)(parameters->block_size*8); Nspd <= MAXDATASPD; Nspd = Nspd * 2) 
			{
				/* 
				Varies the number of wordline segments, 
				i.e. into how many "sub"-wordlines each wordline is split. 
				Increasing the number of segments, increases the number of wordline drivers, 
				but makes each segment shorter and faster.
				*/
				for (Ndwl = 1; Ndwl <= MAXDATAN; Ndwl = Ndwl * 2) 
				{
					/* 
					Varies the number of bitline segments,
					i.e. into how many "sub"-bitlines each bitline is split.
					Increasing the number of segments increases the number of sense amplifiers needed, 
					but makes each bitline segment shorter and faster. 
					*/
					for (Ndbl = 1; Ndbl <= MAXDATAN; Ndbl = Ndbl * 2) 
					{
						/*
						Varies the number of sets per wordline, but for the tag array
						*/
						if(data_organizational_parameters_valid
								(parameters->block_size,
								parameters->tag_associativity,
								parameters->cache_size,
								Ndwl, Ndbl,Nspd,
								parameters->fully_assoc,(*NSubbanks)))
						{
							tag_iteration_counter = 0;
							bank_h = 0;
							bank_v = 0;
			
							if (8 * parameters->block_size / BITOUT == 1 && Nspd == 1)
							{
								muxover = 1;
							}
							else
							{
								if (Nspd > MAX_COL_MUX)
								{
									muxover = 8 * parameters->block_size / BITOUT;
								}
								else
								{
									if (8 * parameters->block_size * Nspd / BITOUT > MAX_COL_MUX)
									{
										muxover = (8 * parameters->block_size / BITOUT) / (MAX_COL_MUX / (Nspd));
									}
									else
									{
										muxover = 1;
									}
								}
							}
			
							reset_data_device_widths();
							compute_device_widths(parameters->cache_size, parameters->block_size, parameters->tag_associativity,
												parameters->fully_assoc,Ndwl,Ndbl,Nspd);
							if(*NSubbanks == 1.0)
							{
			
								reset_powerDef(&total_address_routing_power);
								reset_powerDef(&subbank_address_routing_power);
			
								inrisetime = addr_inrisetime = 0;
			
								/* Calculate data side of cache */
								Tpre = 0;
			
								max_delay = 0;
								reset_powerDef(&decoder_data_power);
								decoder_data =
									decoder_delay (parameters->cache_size,
										parameters->block_size,
										parameters->data_associativity,
										Ndwl, Ndbl, Nspd,
										*NSubbanks,
										&decoder_data_driver,
										&decoder_data_3to8,
										&decoder_data_inv,
										inrisetime,
										&outrisetime,
										&data_nor_inputs,
										&decoder_data_power);
			
								Tpre = decoder_data;
								max_delay = MAX (max_delay, decoder_data);
								inrisetime = outrisetime;
								
								reset_powerDef(&wordline_data_power);
								wordline_data =
									wordline_delay (
										parameters->cache_size,
										parameters->block_size,
										parameters->data_associativity,
										Ndwl,Ndbl,Nspd,
										inrisetime,
										&outrisetime,
										&wordline_data_power);
								
								inrisetime = outrisetime;
								max_delay = MAX (max_delay, wordline_data);
								/*dt: assuming precharge delay is decoder + wordline delay */
								Tpre += wordline_data;
			
								reset_powerDef(&bitline_data_power);
								bitline_data =
									bitline_delay (parameters->cache_size,
										parameters->data_associativity,
										parameters->block_size,
										Ndwl, Ndbl, Nspd,
										inrisetime,
										&outrisetime,
										&bitline_data_power, Tpre);
								
								
			
								inrisetime = outrisetime;
								max_delay = MAX (max_delay, bitline_data);
			
								
								reset_powerDef(&sense_amp_data_power);
								sense_amp_data =
								sense_amp_delay (parameters->cache_size,
										parameters->block_size,
										parameters->data_associativity,
										Ndwl,Ndbl, Nspd,
										inrisetime,
										&outrisetime,
										&sense_amp_data_power);
								
								max_delay = MAX (max_delay, sense_amp_data);
								
			
								inrisetime = outrisetime;
			
								reset_powerDef(&data_output_power);
								data_output =
									dataoutput_delay (
										parameters->cache_size,
										parameters->block_size,
										parameters->data_associativity,
										parameters->fully_assoc,
										Ndbl, Nspd, Ndwl,
										inrisetime,
										&outrisetime,
										&data_output_power);
								
								max_delay = MAX (max_delay, data_output);
								inrisetime = outrisetime;
			
								reset_powerDef(&total_out_driver_power);
			
								subbank_v = 0;
								subbank_h = 0;
			
								inrisetime = outrisetime;
								max_delay = MAX (max_delay, total_out_driver);
			
								/* if the associativity is 1, the data output can come right
									after the sense amp.   Otherwise, it has to wait until 
									the data access has been done. 
								*/
								if (parameters->data_associativity == 1)
								{
									before_mux =
									subbank_address_routing_delay +
									decoder_data + wordline_data +
									bitline_data + sense_amp_data +
									total_out_driver + data_output;
									after_mux = 0;
								}
								else
								{
									before_mux =
									subbank_address_routing_delay +
									decoder_data + wordline_data +
									bitline_data + sense_amp_data;
									after_mux =
									data_output + total_out_driver;
								}
							}
			
							for (Ntspd = 1; Ntspd <= MAXTAGSPD; Ntspd = Ntspd * 2)
							{
								/*
								Varies the number of wordline segments, but for the tag array
								*/
								/*dt: Ntwl is no longer limited to one. We assume a wire-or for the valid and select lines. */
								for (Ntwl = 1; Ntwl <= MAXTAGN; Ntwl = Ntwl * 2)
								{
									/*
									Varies the number of bitline segments, but for the array
									*/
									for (Ntbl = 1; Ntbl <= MAXTAGN; Ntbl = Ntbl * 2)
									{
										if (tag_organizational_parameters_valid
											(parameters->block_size,
											parameters->tag_associativity,
											parameters->cache_size,
											Ntwl, Ntbl, Ntspd,
											parameters->fully_assoc,(*NSubbanks)))
										{
											tag_iteration_counter++;
						
											reset_tag_device_widths();
										
											compute_tag_device_widths(parameters->cache_size, parameters->block_size, parameters->tag_associativity,
																						Ntspd,Ntwl,Ntbl,(*NSubbanks));
						
											area_subbanked (ADDRESS_BITS, BITOUT,
													parameters->num_readwrite_ports,
													parameters->num_read_ports,
													parameters->num_write_ports,
													Ndbl, Ndwl, Nspd, 
													Ntbl, Ntwl,Ntspd,
													*NSubbanks,
													parameters,
													&arearesult_subbanked_temp,
													&arearesult_temp);
						
											Subbank_Efficiency =
												(area_all_dataramcells +
													area_all_tagramcells) * 100 /
												(arearesult_temp.totalarea /
													100000000.0);
										
											//v4.1: No longer using calculate_area function as area has already been
											//computed for the given tech node
											/*Total_Efficiency =
											(*NSubbanks) *
											(area_all_dataramcells +
												area_all_tagramcells) * 100 /
											(calculate_area
												(arearesult_subbanked_temp,
												parameters->fudgefactor) /
												100000000.0);*/ 
						
											Total_Efficiency =
												(*NSubbanks) *
												(area_all_dataramcells +
													area_all_tagramcells) * 100 /
												(arearesult_subbanked_temp.height * arearesult_subbanked_temp.width/
													100000000.0);
			
											//efficiency = Subbank_Efficiency;
											efficiency = Total_Efficiency;
					
											arearesult_temp.efficiency = efficiency;
											aspect_ratio_total_temp =
												(arearesult_subbanked_temp.height /
												arearesult_subbanked_temp.width);
											aspect_ratio_total_temp =
												(aspect_ratio_total_temp > 1.0) ? 
												(aspect_ratio_total_temp) : 1.0 / (aspect_ratio_total_temp);
					
											arearesult_temp.aspect_ratio_total = aspect_ratio_total_temp;
			
											if (*NSubbanks > 1)
											{
												subbank_dim (parameters->cache_size,
													parameters->block_size,
													parameters->data_associativity,
													parameters->fully_assoc,
													Ndbl, Ndwl, Nspd, Ntbl,
													Ntwl, Ntspd, *NSubbanks,
													&bank_h, &bank_v);
					
												reset_powerDef(&total_address_routing_power);
					
												subbanks_routing_power (
															parameters->fully_assoc,
															parameters->data_associativity,
															*NSubbanks,
															&bank_h,
															&bank_v,
															&total_address_routing_power);
					
												reset_powerDef(&subbank_address_routing_power);
												/*dt: this has to be reset on every loop iteration, or else we implicitly reuse the risetime from the end
												of the last loop iteration!*/
												inrisetime = addr_inrisetime = 0;
												if (*NSubbanks > 2)
												{
													subbank_address_routing_delay =
													address_routing_delay
													(parameters->cache_size,
														parameters->block_size,
														parameters->data_associativity,
														parameters->fully_assoc, Ndwl,
														Ndbl, Nspd, Ntwl, Ntbl, Ntspd,
														NSubbanks, &outrisetime,
														&subbank_address_routing_power);
										
													/*dt: moved this here, because we only have extra signal slope if there's
														something before the decoder */
													inrisetime = outrisetime;
													addr_inrisetime = outrisetime;
												}
										
												/* Calculate data side of cache */
												Tpre = 0;
						
												max_delay = 0;
												reset_powerDef(&decoder_data_power);
												decoder_data =
													decoder_delay (parameters->cache_size,
																parameters->block_size,
																parameters->data_associativity,
																Ndwl, Ndbl, Nspd,
																*NSubbanks,
																&decoder_data_driver,
																&decoder_data_3to8,
																&decoder_data_inv,
																inrisetime,
																&outrisetime,
																&data_nor_inputs,
																&decoder_data_power);
						
												Tpre = decoder_data;
												max_delay = MAX (max_delay, decoder_data);
												inrisetime = outrisetime;
										
												reset_powerDef(&wordline_data_power);
												wordline_data =
													wordline_delay (
															parameters->cache_size,
															parameters->block_size,
															parameters->data_associativity,
															Ndwl,Ndbl,Nspd,
															inrisetime,
															&outrisetime,
															&wordline_data_power);
											
												inrisetime = outrisetime;
												max_delay = MAX (max_delay, wordline_data);
												/*dt: assuming precharge delay is decoder + wordline delay */
												Tpre += wordline_data;
							
												reset_powerDef(&bitline_data_power);
												bitline_data =
													bitline_delay (parameters->cache_size,
																parameters->data_associativity,
																parameters->block_size,
																Ndwl, Ndbl, Nspd,
																inrisetime,
																&outrisetime,
																&bitline_data_power, Tpre);
											
						
												inrisetime = outrisetime;
												max_delay = MAX (max_delay, bitline_data);
						
											
												reset_powerDef(&sense_amp_data_power);
												sense_amp_data =
													sense_amp_delay (parameters->cache_size,
														parameters->block_size,
														parameters->data_associativity,
														Ndwl,Ndbl, Nspd,
														inrisetime,
														&outrisetime,
														&sense_amp_data_power);
											
												max_delay = MAX (max_delay, sense_amp_data);
											
						
												inrisetime = outrisetime;
						
												reset_powerDef(&data_output_power);
												data_output =
													dataoutput_delay (
																parameters->cache_size,
																parameters->block_size,
																parameters->data_associativity,
																parameters->fully_assoc,
																Ndbl, Nspd, Ndwl,
																inrisetime,
																&outrisetime,
																&data_output_power);
											
												max_delay = MAX (max_delay, data_output);
												inrisetime = outrisetime;
						
												reset_powerDef(&total_out_driver_power);
						
												subbank_v = 0;
												subbank_h = 0;
						
						
												subbank_routing_length (
														parameters->cache_size,
														parameters->block_size,
														parameters->data_associativity,
														parameters->fully_assoc,
														Ndbl, Nspd,
														Ndwl, Ntbl,
														Ntwl, Ntspd,
														*NSubbanks,
														&subbank_v,
														&subbank_h);
						
												if (*NSubbanks > 2)
												{
													total_out_driver =
														senseext_driver_delay (
															parameters->data_associativity,
															parameters->fully_assoc,
															inrisetime, &outrisetime,
															subbank_v, subbank_h,
															&total_out_driver_power);
												}
											
												inrisetime = outrisetime;
												max_delay = MAX (max_delay, total_out_driver);
						
												/* if the associativity is 1, the data output can come right
													after the sense amp.   Otherwise, it has to wait until 
													the data access has been done. */
							
												if (parameters->data_associativity == 1)
												{
													before_mux =
														subbank_address_routing_delay +
														decoder_data + wordline_data +
														bitline_data + sense_amp_data +
														total_out_driver + data_output;
														after_mux = 0;
												}
												else
												{
													before_mux =
														subbank_address_routing_delay +
														decoder_data + wordline_data +
														bitline_data + sense_amp_data;
														after_mux =
														data_output + total_out_driver;
												}
											}
				
											/*
											* Now worry about the tag side.
											*/
				
				
											reset_powerDef(&decoder_tag_power);
											if(tag_iteration_counter < MAX_CACHE_ENTRIES && valid_tag_cache_entries[tag_iteration_counter] == TRUE)
											{
												current_cache_entry = cached_tag_calculations[tag_iteration_counter];
												decoder_tag = current_cache_entry.decoder.delay;
												copy_powerDef(&decoder_tag_power,current_cache_entry.decoder.power);
											}
											else
											{
												decoder_tag =
													decoder_tag_delay (
														parameters->cache_size,
														parameters->block_size,
														parameters->tag_associativity,
														Ntwl, Ntbl, Ntspd,
														*NSubbanks,
														&decoder_tag_driver,
														&decoder_tag_3to8,
														&decoder_tag_inv,
														addr_inrisetime,
														&outrisetime,
														&tag_nor_inputs,
														&decoder_tag_power);
						
												cached_tag_calculations[tag_iteration_counter].decoder.delay = decoder_tag;
												copy_powerDef(
													&(cached_tag_calculations[tag_iteration_counter].decoder.power)
													,decoder_tag_power);
											}
											
											max_delay =
												MAX (max_delay, decoder_tag);
													Tpre_tag = decoder_tag;
													inrisetime = outrisetime;
						
											reset_powerDef(&wordline_tag_power);
											if(tag_iteration_counter < MAX_CACHE_ENTRIES && valid_tag_cache_entries[tag_iteration_counter] == TRUE)
											{
												current_cache_entry = cached_tag_calculations[tag_iteration_counter];
												wordline_tag = current_cache_entry.wordline.delay;
												copy_powerDef(&wordline_tag_power,current_cache_entry.wordline.power);
											}
											else
											{
												wordline_tag =
													wordline_tag_delay (
															parameters->cache_size,
															parameters->block_size,
															parameters->tag_associativity,
															Ntspd, Ntwl, Ntbl,
															*NSubbanks,
															inrisetime,
															&outrisetime,
															&wordline_tag_power);
						
												cached_tag_calculations[tag_iteration_counter].wordline.delay = wordline_tag;
												copy_powerDef(
													&(cached_tag_calculations[tag_iteration_counter].wordline.power)
													,wordline_tag_power);
											}
											
											max_delay =
												MAX (max_delay, wordline_tag);
													inrisetime = outrisetime;
													Tpre_tag += wordline_tag;
						
											reset_powerDef(&bitline_tag_power);
											if(tag_iteration_counter < MAX_CACHE_ENTRIES && valid_tag_cache_entries[tag_iteration_counter] == TRUE )
											{
												current_cache_entry = cached_tag_calculations[tag_iteration_counter];
												bitline_tag = current_cache_entry.bitline.delay;
												copy_powerDef(&bitline_tag_power,current_cache_entry.bitline.power);
											}
											else
											{
												bitline_tag =
													bitline_tag_delay (
														parameters->cache_size,
														parameters->tag_associativity,
														parameters->block_size,
														Ntwl,Ntbl, Ntspd,
														*NSubbanks,
														inrisetime,
														&outrisetime,
														&bitline_tag_power, Tpre_tag);
						
												cached_tag_calculations[tag_iteration_counter].bitline.delay = bitline_tag;
												copy_powerDef(
													&(cached_tag_calculations[tag_iteration_counter].bitline.power)
													,bitline_tag_power);
											}
											
											max_delay =
												MAX (max_delay, bitline_tag);
													inrisetime = outrisetime;
											
											reset_powerDef(&sense_amp_tag_power);
											if(valid_tag_cache_entries[tag_iteration_counter] == TRUE)
											{
												current_cache_entry = cached_tag_calculations[tag_iteration_counter];
												sense_amp_tag = current_cache_entry.senseamp.delay;
												copy_powerDef(&sense_amp_tag_power,current_cache_entry.senseamp.power);
						
												outrisetime = current_cache_entry.senseamp_outputrisetime;
											}
											else
											{
												sense_amp_tag =
													sense_amp_tag_delay (
														parameters->cache_size,
														parameters->block_size,
														parameters->tag_associativity,
														Ntwl, Ntbl, Ntspd, *NSubbanks,
														inrisetime, &outrisetime,
														&sense_amp_tag_power);
						
												cached_tag_calculations[tag_iteration_counter].senseamp.delay = sense_amp_tag;
												copy_powerDef(
													&(cached_tag_calculations[tag_iteration_counter].senseamp.power)
													,sense_amp_tag_power);
							
												cached_tag_calculations[tag_iteration_counter].senseamp_outputrisetime = outrisetime;
						
												if(*NSubbanks == 1 && tag_iteration_counter < MAX_CACHE_ENTRIES)
												{
													valid_tag_cache_entries[tag_iteration_counter] = TRUE;
												}
											}
						
											max_delay = MAX (max_delay, sense_amp_tag);
											inrisetime = outrisetime;
						
											wirelength_v = wirelength_h = 0;
											if(parameters->tag_associativity != 1)
											{
												precalc_muxdrv_widths(parameters->cache_size,
													parameters->block_size,
													parameters->data_associativity,
													Ndwl, Ndbl, Nspd,
													&wirelength_v, &wirelength_h);
											}
						
											/* split comparator - look at half the address bits only */
											reset_powerDef(&compare_tag_power);
											compare_tag =
												half_compare_delay (
															parameters->cache_size,
															parameters->block_size,
															parameters->tag_associativity,
															Ntwl,Ntbl, Ntspd,
															*NSubbanks,
															inrisetime,
															&outrisetime,
															&compare_tag_power);
											
											inrisetime = outrisetime;
											max_delay = MAX (max_delay, compare_tag);
						
											reset_powerDef(&valid_driver_power);
											reset_powerDef(&mux_driver_power);
											reset_powerDef(&selb_power);
											if (parameters->tag_associativity == 1)
											{
												mux_driver = 0;
												valid_driver =
												valid_driver_delay (
														parameters->cache_size,
														parameters->block_size,
														parameters->tag_associativity,
														parameters->fully_assoc,
														Ndbl, Ndwl,
														Nspd, Ntbl,
														Ntwl, Ntspd,
														NSubbanks,
														inrisetime,
														&valid_driver_power);
											
												max_delay =
												MAX (max_delay, valid_driver);
												time_till_compare =
												subbank_address_routing_delay +
												decoder_tag + wordline_tag +
												bitline_tag + sense_amp_tag;
						
												time_till_select =
												time_till_compare + compare_tag +
												valid_driver;
						
						
												/*
												* From the above info, calculate the total access time
												*/
												/*If we're doing a pure SRAM table, then we don't need to take into account tags */
												if(pure_sram_flag)
												{
													access_time = before_mux + after_mux;
												}
												else
												{
													if(sequential_access_flag)
													{
														/*dt: testing sequential access mode*/
														/*approximating the time it takes the match and valid signal to travel to the edge of the tag array with decode_tag */
														access_time = before_mux + time_till_compare + compare_tag + decoder_tag;
													}
													else
													{
														access_time =
															MAX (before_mux + after_mux,
															time_till_select);
													}
												}
											}
											else
											{
												if(fast_cache_access_flag||sequential_access_flag)
												{
													mux_driver = 0;
												}//||sequential_access_flag added by Shyam to fix the bug
												//of mux driver delay being larger than access time in serial access mode
												//now mux driver delay code won't be touched in serial access mode
												else
												{
													/* dualin mux driver - added for split comparator
														- inverter replaced by nand gate */
													mux_driver =
															mux_driver_delay_dualin
																(parameters->cache_size,
																parameters->block_size,
																parameters->tag_associativity,
																Ntbl, Ntspd,
																inrisetime, &outrisetime,
																wirelength_v, wirelength_h,
																&mux_driver_power);
												}
												max_delay = MAX (max_delay, mux_driver);
						
												selb = selb_delay_tag_path (inrisetime,
															&outrisetime,
															&selb_power);
												max_delay = MAX (max_delay, selb);
						
												valid_driver = 0;
						
												time_till_compare =
													subbank_address_routing_delay +
													decoder_tag + wordline_tag +
													bitline_tag + sense_amp_tag;
						
												if(fast_cache_access_flag)
												{
													/*dt: don't wait for the mux signal to travel to the subarray, have the n ways
													routed to the edge of the data array*/
													time_till_select =
														time_till_compare + compare_tag +
														selb;
												}
												else
												{
													time_till_select =
														time_till_compare + compare_tag +
														mux_driver +
														selb;
												}
											
												if(sequential_access_flag)
												{
													/*dt: testing sequential access*/
													/*approximating the time it takes the match and valid signal to travel to the edge of the tag array with decode_tag */
													access_time = before_mux + time_till_compare + compare_tag + decoder_tag;
												}
												else
												{
													access_time =
														MAX (before_mux,
														time_till_select) +
														after_mux;
												}
											}
						
											/*
											* Calcuate the cycle time
											*/
						
											precharge_del = precharge_delay(wordline_data);
						
											/*dt: replacing this with a simple calculation:
													The things which cannot be easily pipelined are:
													wordline and bitlin
													senseamp
													comparator
													so those are the things which are going to limit cycle time
											*/
											cycle_time = MAX(
															MAX(wordline_data + bitline_data + sense_amp_data,
																wordline_tag  + bitline_tag  + sense_amp_tag),
															compare_tag);
						
											/*
											* The parameters are for a 0.8um process.  A quick way to
											* scale the results to another process is to divide all
											* the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
											*/
								
											//v4.1: All delay and energy components are no longer scaled by FUDGEFACTOR as they have already
											//been computed for the input tech node.
											allports = parameters->num_readwrite_ports + parameters->num_read_ports + parameters->num_write_ports;
											allreadports = parameters->num_readwrite_ports + parameters->num_read_ports;
											allwriteports = parameters->num_readwrite_ports + parameters->num_write_ports;
						
											/*dt:We'll add this at the end*/
											mult_powerDef(&total_address_routing_power,allports);
									
											reset_powerDef(&total_power);
									
											mac_powerDef(&total_power,&subbank_address_routing_power,allports);
											//v4.2: While calculating total read (and write) energy, the decoder and wordline
											//energies per read (or write) port were being multiplied by total number of ports
											//which is not right
											//mac_powerDef(&total_power,&decoder_data_power,allports);
											total_power.readOp.dynamic += decoder_data_power.readOp.dynamic * allreadports;
											total_power.writeOp.dynamic += decoder_data_power.writeOp.dynamic * allwriteports;
											//mac_powerDef(&total_power,&wordline_data_power,allports);
											total_power.readOp.dynamic += wordline_data_power.readOp.dynamic * allreadports;
											total_power.writeOp.dynamic += wordline_data_power.writeOp.dynamic * allwriteports;
											/*dt: We can have different numbers of read/write ports, so the power numbers have to keep that in mind. Ports which can do both reads and
											writes are counted as writes for max power, because writes use full swing and thus use more power. (Assuming full bitline swing is greater 
											than senseamp power) */
											total_power.readOp.dynamic += bitline_data_power.readOp.dynamic * allreadports;
											total_power.writeOp.dynamic += bitline_data_power.writeOp.dynamic * allwriteports;
											/*dt: for multiported SRAM cells we assume NAND stacks on all the pure read ports. We assume neglegible
											leakage for these ports. The following code adjusts the leakage numbers accordingly.*/
											total_power.readOp.leakage += bitline_data_power.readOp.leakage * allreadports;//changed to allreadports by Shyam. Earlier this was allwriteports.
											total_power.writeOp.leakage += bitline_data_power.writeOp.leakage * allwriteports; 
						
											mac_powerDef(&total_power,&sense_amp_data_power,allreadports);
											mac_powerDef(&total_power,&total_out_driver_power,allreadports);
											/*dt: Tags are only written to when a cache line is evicted/replaced. The only bit which is written is the dirty bit. The dirty bits are kept
											in the data array. */
											//if on pure_sram_flag added by Shyam since tag power should not be added to total power in pure SRAM mode.
											if(!pure_sram_flag)
											{
												mac_powerDef(&total_power,&decoder_tag_power,allreadports);
												mac_powerDef(&total_power,&wordline_tag_power,allreadports);
												mac_powerDef(&total_power,&bitline_tag_power,allreadports);
												mac_powerDef(&total_power,&sense_amp_tag_power,allreadports);
												mac_powerDef(&total_power,&compare_tag_power,allreadports);
												mac_powerDef(&total_power,&valid_driver_power,allreadports);
												mac_powerDef(&total_power,&mux_driver_power,allreadports);
												mac_powerDef(&total_power,&selb_power,allreadports);
											}
											mac_powerDef(&total_power,&data_output_power,allreadports);
						
						
											reset_powerDef(&total_power_without_routing);
											mac_powerDef(&total_power_without_routing,&total_power,1); // copy over and ..
						
											//total_power.readOp.dynamic /= FUDGEFACTOR;
											//total_power.writeOp.dynamic /= FUDGEFACTOR;
											/*dt: Leakage isn't scaled with FUDGEFACTOR, because that's already done by the leakage model much more realistically */
						
								
											mac_powerDef(&total_power_without_routing,&subbank_address_routing_power,-allports); // ... then subtract ..
											mac_powerDef(&total_power_without_routing,&total_out_driver_power,-allreadports);
											mac_powerDef(&total_power_without_routing,&valid_driver_power,-allreadports);
								
											/*total_power_without_routing.readOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
											total_power_without_routing.readOp.leakage *= (*NSubbanks);
											total_power_without_routing.writeOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
											total_power_without_routing.writeOp.leakage *= (*NSubbanks);*/
									
											total_power_without_routing.readOp.dynamic *= (*NSubbanks);
											total_power_without_routing.readOp.leakage *= (*NSubbanks);
											total_power_without_routing.writeOp.dynamic *= (*NSubbanks);
											total_power_without_routing.writeOp.leakage *= (*NSubbanks);
											/*dt: see above for leakage*/
						
											reset_powerDef(&total_power_allbanks);
											//total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic / FUDGEFACTOR;
											total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic;
											total_power_allbanks.readOp.leakage = total_power_without_routing.readOp.leakage + total_address_routing_power.readOp.leakage;
											//total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic / FUDGEFACTOR;
											total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic;
											total_power_allbanks.writeOp.leakage = total_power_without_routing.writeOp.leakage + total_address_routing_power.writeOp.leakage;
						
											//      if (counter==1)
											//        fprintf(stderr, "Pow - %f, Acc - %f, Pow - %f, Acc - %f, Combo - %f\n", total_power*1e9, access_time*1e9, total_power/result->max_power, access_time/result->max_access_time, total_power/result->max_power*access_time/result->max_access_time);
						
											if (counter == 1)
											{
												if ( objective_function(1,1,1,
																	result->access_time / result->max_access_time,
																	1.0 / arearesult->efficiency,
																	((result->total_power.readOp.dynamic/result->cycle_time)+result->total_power.readOp.leakage+
																		(result->total_power.writeOp.dynamic/result->cycle_time)+result->total_power.writeOp.leakage) / 2.0
																	/ result->max_power)
													>
													objective_function(1,1,1,
																		access_time /(result->max_access_time),
																		1.0 / efficiency,
																		((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
																		(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage) / 2.0 
																		/ result->max_power)
												)//read and write dynamic energy components changed to power components by Shyam. Earlier 
												//read and write dynamic energies were being added to leakage power components and getting swamped
												{
													result->senseext_scale = senseext_scale;
													copy_powerDef(&result->total_power,total_power);
													copy_powerDef(&result->total_power_without_routing,total_power_without_routing);
													//copy_and_div_powerDef(&result->total_routing_power,total_address_routing_power,FUDGEFACTOR);
													copy_powerDef(&result->total_routing_power,total_address_routing_power);
													copy_powerDef(&result->total_power_allbanks,total_power_allbanks);
							
													result->subbank_address_routing_delay = subbank_address_routing_delay / FUDGEFACTOR;
													//copy_and_div_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power,FUDGEFACTOR);
													copy_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power);	
							
													//result->cycle_time = cycle_time / FUDGEFACTOR;
													//result->access_time = access_time / FUDGEFACTOR;
													result->cycle_time = cycle_time;
													result->access_time = access_time;
													result->best_muxover = muxover;
													result->best_Ndwl = Ndwl;
													result->best_Ndbl = Ndbl;
													result->best_Nspd = Nspd;
													result->best_Ntwl = Ntwl;
													result->best_Ntbl = Ntbl;
													result->best_Ntspd = Ntspd;
													/*result->decoder_delay_data = decoder_data / FUDGEFACTOR;
													copy_and_div_powerDef(&result->decoder_power_data,decoder_data_power,FUDGEFACTOR);
													result->decoder_delay_tag = decoder_tag / FUDGEFACTOR;
													copy_and_div_powerDef(&result->decoder_power_tag,decoder_tag_power,FUDGEFACTOR);
													result->dec_tag_driver = decoder_tag_driver / FUDGEFACTOR;
													result->dec_tag_3to8 = decoder_tag_3to8 / FUDGEFACTOR;
													result->dec_tag_inv = decoder_tag_inv / FUDGEFACTOR;
													result->dec_data_driver = decoder_data_driver / FUDGEFACTOR;
													result->dec_data_3to8 = decoder_data_3to8 / FUDGEFACTOR;
													result->dec_data_inv = decoder_data_inv / FUDGEFACTOR;
													result->wordline_delay_data = wordline_data / FUDGEFACTOR;
													copy_and_div_powerDef(&result->wordline_power_data,wordline_data_power,FUDGEFACTOR);
													result->wordline_delay_tag = wordline_tag / FUDGEFACTOR;
													copy_and_div_powerDef(&result->wordline_power_tag,wordline_tag_power,FUDGEFACTOR);
													result->bitline_delay_data = bitline_data / FUDGEFACTOR;
													copy_and_div_powerDef(&result->bitline_power_data,bitline_data_power,FUDGEFACTOR);
													result->bitline_delay_tag = bitline_tag / FUDGEFACTOR;
													copy_and_div_powerDef(&result->bitline_power_tag,bitline_tag_power,FUDGEFACTOR);
													result->sense_amp_delay_data = sense_amp_data / FUDGEFACTOR;
													copy_and_div_powerDef(&result->sense_amp_power_data,sense_amp_data_power,FUDGEFACTOR);
													result->sense_amp_delay_tag = sense_amp_tag / FUDGEFACTOR;
													copy_and_div_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power,FUDGEFACTOR);
													result->total_out_driver_delay_data = total_out_driver / FUDGEFACTOR;
													copy_and_div_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
													result->compare_part_delay = compare_tag / FUDGEFACTOR;
													copy_and_div_powerDef(&result->compare_part_power,compare_tag_power,FUDGEFACTOR);
													result->drive_mux_delay = mux_driver / FUDGEFACTOR;
													copy_and_div_powerDef(&result->drive_mux_power,mux_driver_power,FUDGEFACTOR);
													result->selb_delay = selb / FUDGEFACTOR;
													copy_and_div_powerDef(&result->selb_power,selb_power,FUDGEFACTOR);
													result->drive_valid_delay = valid_driver / FUDGEFACTOR;
													copy_and_div_powerDef(&result->drive_valid_power,valid_driver_power,FUDGEFACTOR);
													result->data_output_delay = data_output / FUDGEFACTOR;
													copy_and_div_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);
													result->precharge_delay = precharge_del / FUDGEFACTOR;*/
						
													result->decoder_delay_data = decoder_data;
													copy_powerDef(&result->decoder_power_data,decoder_data_power);
													result->decoder_delay_tag = decoder_tag;
													copy_powerDef(&result->decoder_power_tag,decoder_tag_power);
													result->dec_tag_driver = decoder_tag_driver;
													result->dec_tag_3to8 = decoder_tag_3to8;
													result->dec_tag_inv = decoder_tag_inv;
													result->dec_data_driver = decoder_data_driver;
													result->dec_data_3to8 = decoder_data_3to8;
													result->dec_data_inv = decoder_data_inv;
													result->wordline_delay_data = wordline_data;
													copy_powerDef(&result->wordline_power_data,wordline_data_power);
													result->wordline_delay_tag = wordline_tag;
													copy_powerDef(&result->wordline_power_tag,wordline_tag_power);
													result->bitline_delay_data = bitline_data;
													copy_powerDef(&result->bitline_power_data,bitline_data_power);
													result->bitline_delay_tag = bitline_tag;
													copy_powerDef(&result->bitline_power_tag,bitline_tag_power);
													result->sense_amp_delay_data = sense_amp_data;
													copy_powerDef(&result->sense_amp_power_data,sense_amp_data_power);
													result->sense_amp_delay_tag = sense_amp_tag;
													copy_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power);
													result->total_out_driver_delay_data = total_out_driver;
													copy_powerDef(&result->total_out_driver_power_data,total_out_driver_power);
													result->compare_part_delay = compare_tag;
													copy_powerDef(&result->compare_part_power,compare_tag_power);
													result->drive_mux_delay = mux_driver;
													copy_powerDef(&result->drive_mux_power,mux_driver_power);
													result->selb_delay = selb;
													copy_powerDef(&result->selb_power,selb_power);
													result->drive_valid_delay = valid_driver;
													copy_powerDef(&result->drive_valid_power,valid_driver_power);
													result->data_output_delay = data_output;
													copy_powerDef(&result->data_output_power,data_output_power);
													result->precharge_delay = precharge_del;
						
						
						
													result->data_nor_inputs = data_nor_inputs;
													result->tag_nor_inputs = tag_nor_inputs;
													area_subbanked (ADDRESS_BITS,
															BITOUT,
															parameters->
															num_readwrite_ports,
															parameters->
															num_read_ports,
															parameters->
															num_write_ports,
															Ndbl, Ndwl,
															Nspd, Ntbl,
															Ntwl, Ntspd,
															*NSubbanks,
															parameters,
															arearesult_subbanked,
															arearesult);
						
												
													//v4.1: No longer using calculate_area function as area has already been
													//computed for the given tech node
																/*arearesult->efficiency =
															(*NSubbanks) *
															(area_all_dataramcells +
																area_all_tagramcells) * 100 /
															(calculate_area
																(*arearesult_subbanked,
																parameters->fudgefactor) / 100000000.0);*/
						
						
													arearesult->efficiency =
														(*NSubbanks) *
														(area_all_dataramcells +
															area_all_tagramcells) * 100 /
														(arearesult_subbanked->height * arearesult_subbanked->width / 100000000.0);
													arearesult->aspect_ratio_total =
															(arearesult_subbanked->height / arearesult_subbanked->width);
													arearesult->aspect_ratio_total =
														(arearesult->aspect_ratio_total > 1.0) ? 
														(arearesult->aspect_ratio_total) : 1.0 / (arearesult->aspect_ratio_total);
													arearesult->max_efficiency = max_efficiency;
														arearesult->max_aspect_ratio_total = max_aspect_ratio_total;
						
						
												}
											}
											else
											{
												if (result->max_access_time < access_time)
												{
													result->max_access_time = access_time ;
												}
												
												if (result->max_power < 
													((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
																		(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0)
												{
													result->max_power = ((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
																		(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0;
													//read and write dynamic energy components changed to power components by Shyam. Earlier 
													//read and write dynamic energies were being added to leakage power components and getting swamped
												}
												
												if (arearesult_temp.max_efficiency < efficiency)
												{
													arearesult_temp.max_efficiency = efficiency;
													max_efficiency = efficiency;
												}
												
												if (min_efficiency > efficiency)
												{
													min_efficiency = efficiency;
												}
												
												if (max_aspect_ratio_total < aspect_ratio_total_temp)
												{
													max_aspect_ratio_total = aspect_ratio_total_temp;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			/* Fully associative model - only vary Ndbl|Ntbl */
		
			for (Ndbl = 1; Ndbl <= MAXDATAN; Ndbl = Ndbl * 2)
			{
				Ntbl = Ndbl;
				//v4.1: Nspd is now of double data type
				//Ndwl = Nspd = Ntwl = Ntspd = 1;
				Ndwl = Ntwl = Ntspd = 1;
				Nspd = 1.0;
		
		
				if (data_organizational_parameters_valid
					(parameters->block_size, 1, parameters->cache_size, Ndwl,
					Ndbl, Nspd, parameters->fully_assoc,(*NSubbanks)))
				{
				
					if (8 * parameters->block_size / BITOUT == 1
						&& Nspd == 1)
					{
						muxover = 1;
					}
					else
					{
						if (Nspd > MAX_COL_MUX)
						{
							muxover = 8 * parameters->block_size / BITOUT;
						}
						else
						{
							if (8 * parameters->block_size * Nspd /
								BITOUT > MAX_COL_MUX)
							{
								muxover =
									(8 * parameters->block_size / BITOUT) /
									(MAX_COL_MUX / (Nspd));
							}
							else
							{
								muxover = 1;
							}
						}
					}
				
				
					area_subbanked (ADDRESS_BITS, BITOUT,
							parameters->num_readwrite_ports,
							parameters->num_read_ports,
							parameters->num_write_ports, Ndbl, Ndwl,
							Nspd, Ntbl, Ntwl, Ntspd, *NSubbanks,
							parameters, &arearesult_subbanked_temp,
							&arearesult_temp);
				
					Subbank_Efficiency =
						(area_all_dataramcells +
							area_all_tagramcells) * 100 /
						(arearesult_temp.totalarea / 100000000.0);
				
					//v4.1: No longer using calculate_area function as area has already been
					//computed for the given tech node
					/*Total_Efficiency =
					(*NSubbanks) * (area_all_dataramcells +
							area_all_tagramcells) * 100 /
					(calculate_area (arearesult_subbanked_temp,
								parameters->fudgefactor) / 100000000.0);*/
				
					Total_Efficiency =
						(*NSubbanks) * (area_all_dataramcells +
						area_all_tagramcells) * 100 /
						(arearesult_subbanked_temp.height * arearesult_subbanked_temp.width / 100000000.0);
					// efficiency = Subbank_Efficiency;
					efficiency = Total_Efficiency;
			
					arearesult_temp.efficiency = efficiency;
					aspect_ratio_total_temp =
						(arearesult_subbanked_temp.height /
							arearesult_subbanked_temp.width);
					aspect_ratio_total_temp =
						(aspect_ratio_total_temp >
						1.0) ? (aspect_ratio_total_temp) : 1.0 /
						(aspect_ratio_total_temp);
			
					arearesult_temp.aspect_ratio_total =
						aspect_ratio_total_temp;
			
					bank_h = 0;
					bank_v = 0;
		
					subbank_dim (parameters->cache_size, parameters->block_size,
							parameters->data_associativity,
							parameters->fully_assoc, Ndbl, Ndwl, Nspd,
							Ntbl, Ntwl, Ntspd, *NSubbanks, &bank_h,
							&bank_v);
		
					reset_powerDef(&subbank_address_routing_power);
					/*dt: this has to be reset on every loop iteration, or else we implicitly reuse the risetime from the end
					of the last loop iteration!*/
					inrisetime = addr_inrisetime = 0;
		
					subbanks_routing_power (parameters->fully_assoc,
								parameters->data_associativity,
								*NSubbanks, &bank_h, &bank_v,
								&total_address_routing_power);
		
					if (*NSubbanks > 2)
					{
						subbank_address_routing_delay =
						address_routing_delay (parameters->cache_size,
									parameters->block_size,
									parameters->data_associativity,
									parameters->fully_assoc, Ndwl,
									Ndbl, Nspd, Ntwl, Ntbl, Ntspd,
									NSubbanks, &outrisetime,
									&subbank_address_routing_power);
					}
		
				
		
					/* Calculate data side of cache */
					inrisetime = outrisetime;
					addr_inrisetime = outrisetime;
			
					max_delay = 0;
					/* tag path contained here */
					reset_powerDef(&decoder_data_power);
					decoder_data = fa_tag_delay (parameters->cache_size,
									parameters->block_size,
									Ntwl, Ntbl, Ntspd,
									&tag_delay_part1,
									&tag_delay_part2,
									&tag_delay_part3,
									&tag_delay_part4,
									&tag_delay_part5,
									&tag_delay_part6, &outrisetime,
									&tag_nor_inputs,
									&decoder_data_power);
		
					inrisetime = outrisetime;
					max_delay = MAX (max_delay, decoder_data);
					Tpre = decoder_data;
		
		
					reset_powerDef(&wordline_data_power);
		
					//Added by Shyam to make FA caches work
					Wdecinvn = 20.0 / FUDGEFACTOR;//From v3.2 #define value
					Wdecinvp = 40.0 / FUDGEFACTOR;//From v3.2 #define value
					Woutdrvseln = 24.0 / FUDGEFACTOR;
					Woutdrvselp = 40.0 / FUDGEFACTOR;
					Woutdrvnandn = 10.0 / FUDGEFACTOR;
					Woutdrvnandp = 30.0 / FUDGEFACTOR;
					Woutdrvnorn = 5.0 / FUDGEFACTOR;
					Woutdrvnorp = 20.0 / FUDGEFACTOR;
					Woutdrivern = 48.0 / FUDGEFACTOR;
					Woutdriverp = 80.0 / FUDGEFACTOR;
				
					colsfa = CHUNKSIZE*parameters->block_size*1*Nspd/Ndwl;
					desiredrisetimefa = krise*log((double)(colsfa))/2.0;
					Clinefa = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ Cwordmetal)*colsfa;
					Rpdrivefa = desiredrisetimefa/(Clinefa*log(VSINV)*-1.0);
					WwlDrvp = restowidth(Rpdrivefa,PCH);
					if (WwlDrvp > Wworddrivemax)
					{
						WwlDrvp = Wworddrivemax;
					}
					//End of Shyam's FA change
		
					wordline_data = wordline_delay (
									parameters->cache_size,
									parameters->block_size,
									1, Ndwl, Ndbl,Nspd,
									inrisetime, &outrisetime,
									&wordline_data_power);
				
					inrisetime = outrisetime;
					max_delay = MAX (max_delay, wordline_data);
					/*dt: assuming that the precharge delay is equal to decode + wordline delay */
					Tpre += wordline_data;
		
					reset_powerDef(&bitline_data_power);
					bitline_data = bitline_delay (parameters->cache_size, 1,
								parameters->block_size, Ndwl,
								Ndbl, Nspd, inrisetime,
								&outrisetime,
								&bitline_data_power, Tpre);
				
				
					inrisetime = outrisetime;
					max_delay = MAX (max_delay, bitline_data);
		
					{
						reset_powerDef(&sense_amp_data_power);
						sense_amp_data =
							sense_amp_delay (parameters->cache_size,
									parameters->block_size,
									parameters->data_associativity,
									Ndwl, Ndbl, Nspd,
									inrisetime, &outrisetime,
									&sense_amp_data_power);
						
						max_delay = MAX (max_delay, sense_amp_data);
					}
					inrisetime = outrisetime;
			
					reset_powerDef(&data_output_power);
					data_output =
					dataoutput_delay (parameters->cache_size,
								parameters->block_size, 1,
								parameters->fully_assoc, 
								Ndbl, Nspd, Ndwl,
								inrisetime, &outrisetime,
								&data_output_power);
			
					inrisetime = outrisetime;
					max_delay = MAX (max_delay, data_output);
			
					reset_powerDef(&total_out_driver_power);
			
					subbank_v = 0;
					subbank_h = 0;
		
					subbank_routing_length (parameters->cache_size,
								parameters->block_size,
								parameters->data_associativity,
								parameters->fully_assoc, Ndbl, Nspd,
								Ndwl, Ntbl, Ntwl, Ntspd, *NSubbanks,
								&subbank_v, &subbank_h);
			
					if (*NSubbanks > 2)
					{
						total_out_driver =
							senseext_driver_delay (parameters->data_associativity,
								parameters->fully_assoc,
								inrisetime, &outrisetime,
								subbank_v, subbank_h,
								&total_out_driver_power);
					}
		
		/*
					total_out_driver = total_out_driver_delay(parameters->cache_size,
						parameters->block_size,parameters->associativity,parameters->fully_assoc,
						Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,
						*NSubbanks,inrisetime,&outrisetime, &total_out_driver_power);
		*/
				
					inrisetime = outrisetime;
					max_delay = MAX (max_delay, total_out_driver);
		
					access_time =
						subbank_address_routing_delay + decoder_data +
						wordline_data + bitline_data + sense_amp_data +
						data_output + total_out_driver;
		
					/*
					* Calcuate the cycle time
					*/
		
					precharge_del = precharge_delay(wordline_data);
		
					cycle_time = MAX(
									MAX(wordline_data + bitline_data + sense_amp_data,
										wordline_tag  + bitline_tag + sense_amp_tag),
									compare_tag);
		
					/*
					* The parameters are for a 0.8um process.  A quick way to
					* scale the results to another process is to divide all
					* the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
					*/
					/*dt: see previous comment on sense amp leakage*/
					/*
					sense_amp_data_power +=
					(data_output + total_out_driver) * 500e-6 * 5;
					*/
					allports = parameters->num_readwrite_ports + parameters->num_read_ports + parameters->num_write_ports;
					allreadports = parameters->num_readwrite_ports + parameters->num_read_ports;
					allwriteports = parameters->num_readwrite_ports + parameters->num_write_ports;
		
					mult_powerDef(&total_address_routing_power,allports);
		
					reset_powerDef(&total_power);
			
					mac_powerDef(&total_power,&subbank_address_routing_power,allports);
					
					//v4.2: While calculating total read (and write) energy, the decoder and wordline
					//energies per read (or write) port were being multiplied by total number of ports
					//which is not right
					//mac_powerDef(&total_power,&decoder_data_power,allports);
					total_power.readOp.dynamic += decoder_data_power.readOp.dynamic * allreadports;
					total_power.writeOp.dynamic += decoder_data_power.writeOp.dynamic * allwriteports;
					//mac_powerDef(&total_power,&wordline_data_power,allports);
					total_power.readOp.dynamic += wordline_data_power.readOp.dynamic * allreadports;
					total_power.writeOp.dynamic += wordline_data_power.writeOp.dynamic * allwriteports;
			
					/*dt: We can have different numbers of read/write ports, so the power numbers have to keep that in mind. Ports which can do both reads and
					writes are counted as writes for max power, because writes use full swing and thus use more power. (Assuming full bitline swing is greater 
					than senseamp power) */
					total_power.readOp.dynamic += bitline_data_power.readOp.dynamic * allreadports;
					total_power.writeOp.dynamic += bitline_data_power.writeOp.dynamic * allwriteports;
					/*dt: for multiported SRAM cells we assume NAND stacks on all the pure read ports. We assume neglegible
					leakage for these ports. The following code adjusts the leakage numbers accordingly.*/
					total_power.readOp.leakage += bitline_data_power.readOp.leakage * allwriteports;
					total_power.writeOp.leakage += bitline_data_power.writeOp.leakage * allwriteports; 
			
					mac_powerDef(&total_power,&sense_amp_data_power,allreadports);
					mac_powerDef(&total_power,&total_out_driver_power,allreadports);
					/*dt: in a fully associative cache we don't have tag decoders, wordlines or bitlines, etc. . Only CAM */
					reset_powerDef(&decoder_tag_power);
					reset_powerDef(&wordline_tag_power);
					reset_powerDef(&bitline_tag_power);
					reset_powerDef(&sense_amp_tag_power);
					reset_powerDef(&compare_tag_power);
					reset_powerDef(&valid_driver_power);
					reset_powerDef(&mux_driver_power);
					reset_powerDef(&selb_power);
		
					mac_powerDef(&total_power,&data_output_power,allreadports);
			
					reset_powerDef(&total_power_without_routing);
					mac_powerDef(&total_power_without_routing,&total_power,1); // copy over and ..
		
					//total_power.readOp.dynamic /= FUDGEFACTOR;
					//total_power.writeOp.dynamic /= FUDGEFACTOR;
					/*dt: Leakage isn't scaled with FUDGEFACTOR, because that's already done by the leakage model much more realistically */
			
					mac_powerDef(&total_power_without_routing,&subbank_address_routing_power,-allports); // ... then subtract ..
					mac_powerDef(&total_power_without_routing,&total_out_driver_power,-allreadports);
					mac_powerDef(&total_power_without_routing,&valid_driver_power,-allreadports);
			
					/*dt: See above for leakage */
					//total_power_without_routing.readOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
					total_power_without_routing.readOp.dynamic *= (*NSubbanks);
					total_power_without_routing.readOp.leakage *= (*NSubbanks);
					//total_power_without_routing.writeOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
					total_power_without_routing.writeOp.dynamic *= (*NSubbanks);
					total_power_without_routing.writeOp.leakage *= (*NSubbanks);
			
					//total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic / FUDGEFACTOR;
					total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic ;
					total_power_allbanks.readOp.leakage = total_power_without_routing.readOp.leakage + total_address_routing_power.readOp.leakage;
					//total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic / FUDGEFACTOR;
					total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic;
					total_power_allbanks.writeOp.leakage = total_power_without_routing.writeOp.leakage + total_address_routing_power.writeOp.leakage;
		
					if (counter == 1)
					{
						// if ((result->total_power/result->max_power)/2+(result->access_time/result->max_access_time) > ((total_power/result->max_power)/2+access_time/(result->max_access_time*FUDGEFACTOR))) 
		
						if ( objective_function(1,1,1,
												result->access_time / result->max_access_time,
												1.0 / arearesult->efficiency,
												((result->total_power.readOp.dynamic/result->cycle_time)+result->total_power.readOp.leakage+
													(result->total_power.writeOp.dynamic/result->cycle_time)+result->total_power.writeOp.leakage) / 2.0
												/ result->max_power)
								>
								objective_function(1,1,1,
													access_time /(result->max_access_time),
													1.0 / efficiency,
													((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
													(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage) / 2.0
													/ result->max_power)
						)//read and write dynamic energy components changed to power components by Shyam. Earlier 
						//read and write dynamic energies were being added to leakage power components and getting swamped
					
						{
							// if ((result->total_power/result->max_power)/2+(result->access_time/result->max_access_time) + (min_efficiency/arearesult->efficiency)/4 + (arearesult->aspect_ratio_total/max_aspect_ratio_total)/3 > ((total_power/result->max_power)/2+access_time/(result->max_access_time*FUDGEFACTOR)+ (min_efficiency/efficiency)/4 + (arearesult_temp.aspect_ratio_total/max_aspect_ratio_total)/3)) 
		
							//          if (result->cycle_time+1e-11*(result->best_Ndwl+result->best_Ndbl+result->best_Nspd+result->best_Ntwl+result->best_Ntbl+result->best_Ntspd) > cycle_time/FUDGEFACTOR+1e-11*(Ndwl+Ndbl+Nspd+Ntwl+Ntbl+Ntspd)) 	
		
							result->senseext_scale = senseext_scale;
							copy_powerDef(&result->total_power,total_power);
							copy_powerDef(&result->total_power_without_routing,total_power_without_routing);
							//copy_and_div_powerDef(&result->total_routing_power,total_address_routing_power,FUDGEFACTOR);
							copy_powerDef(&result->total_routing_power,total_address_routing_power);
							copy_powerDef(&result->total_power_allbanks,total_power_allbanks);
		
							/*result->subbank_address_routing_delay = subbank_address_routing_delay / FUDGEFACTOR;
							copy_and_div_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power,FUDGEFACTOR);
							result->cycle_time = cycle_time / FUDGEFACTOR;
							result->access_time = access_time / FUDGEFACTOR;
							result->best_Ndwl = Ndwl;
							result->best_Ndbl = Ndbl;
							result->best_Nspd = Nspd;
							result->best_Ntwl = Ntwl;
							result->best_Ntbl = Ntbl;
							result->best_Ntspd = Ntspd;
							result->decoder_delay_data = decoder_data / FUDGEFACTOR;
							copy_and_div_powerDef(&result->decoder_power_data,decoder_data_power,FUDGEFACTOR);
							result->decoder_delay_tag = decoder_tag / FUDGEFACTOR;
							copy_and_div_powerDef(&result->decoder_power_tag,decoder_tag_power,FUDGEFACTOR);
							result->dec_tag_driver = decoder_tag_driver / FUDGEFACTOR;
							result->dec_tag_3to8 = decoder_tag_3to8 / FUDGEFACTOR;
							result->dec_tag_inv = decoder_tag_inv / FUDGEFACTOR;
							result->dec_data_driver = decoder_data_driver / FUDGEFACTOR;
							result->dec_data_3to8 = decoder_data_3to8 / FUDGEFACTOR;
							result->dec_data_inv = decoder_data_inv / FUDGEFACTOR;
							result->wordline_delay_data = wordline_data / FUDGEFACTOR;
							copy_and_div_powerDef(&result->wordline_power_data,wordline_data_power,FUDGEFACTOR);
							result->wordline_delay_tag = wordline_tag / FUDGEFACTOR;
							copy_and_div_powerDef(&result->wordline_power_tag,wordline_tag_power,FUDGEFACTOR);
							result->bitline_delay_data = bitline_data / FUDGEFACTOR;
							copy_and_div_powerDef(&result->bitline_power_data,bitline_data_power,FUDGEFACTOR);
							result->bitline_delay_tag = bitline_tag / FUDGEFACTOR;
							copy_and_div_powerDef(&result->bitline_power_tag,bitline_tag_power,FUDGEFACTOR);
							result->sense_amp_delay_data = sense_amp_data / FUDGEFACTOR;
							copy_and_div_powerDef(&result->sense_amp_power_data,sense_amp_data_power,FUDGEFACTOR);
							result->sense_amp_delay_tag = sense_amp_tag / FUDGEFACTOR;
							copy_and_div_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power,FUDGEFACTOR);
							result->total_out_driver_delay_data = total_out_driver / FUDGEFACTOR;
							copy_and_div_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
							result->compare_part_delay = compare_tag / FUDGEFACTOR;
							copy_and_div_powerDef(&result->compare_part_power,compare_tag_power,FUDGEFACTOR);
							result->drive_mux_delay = mux_driver / FUDGEFACTOR;
							copy_and_div_powerDef(&result->drive_mux_power,mux_driver_power,FUDGEFACTOR);
							result->selb_delay = selb / FUDGEFACTOR;
							copy_and_div_powerDef(&result->selb_power,selb_power,FUDGEFACTOR);
							result->drive_valid_delay = valid_driver / FUDGEFACTOR;
							copy_and_div_powerDef(&result->drive_valid_power,valid_driver_power,FUDGEFACTOR);
							result->data_output_delay = data_output / FUDGEFACTOR;
							copy_and_div_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);
							result->precharge_delay = precharge_del / FUDGEFACTOR;*/
		
							result->subbank_address_routing_delay = subbank_address_routing_delay;
							copy_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power);
							result->cycle_time = cycle_time;
							result->access_time = access_time;
							result->best_Ndwl = Ndwl;
							result->best_Ndbl = Ndbl;
							result->best_Nspd = Nspd;
							result->best_Ntwl = Ntwl;
							result->best_Ntbl = Ntbl;
							result->best_Ntspd = Ntspd;
							result->decoder_delay_data = decoder_data;
							copy_powerDef(&result->decoder_power_data,decoder_data_power);
							result->decoder_delay_tag = decoder_tag;
							copy_powerDef(&result->decoder_power_tag,decoder_tag_power);
							result->dec_tag_driver = decoder_tag_driver;
							result->dec_tag_3to8 = decoder_tag_3to8;
							result->dec_tag_inv = decoder_tag_inv;
							result->dec_data_driver = decoder_data_driver;
							result->dec_data_3to8 = decoder_data_3to8;
							result->dec_data_inv = decoder_data_inv;
							result->wordline_delay_data = wordline_data;
							copy_powerDef(&result->wordline_power_data,wordline_data_power);
							result->wordline_delay_tag = wordline_tag;
							copy_powerDef(&result->wordline_power_tag,wordline_tag_power);
							result->bitline_delay_data = bitline_data;
							copy_powerDef(&result->bitline_power_data,bitline_data_power);
							result->bitline_delay_tag = bitline_tag;
							copy_powerDef(&result->bitline_power_tag,bitline_tag_power);
							result->sense_amp_delay_data = sense_amp_data;
							copy_powerDef(&result->sense_amp_power_data,sense_amp_data_power);
							result->sense_amp_delay_tag = sense_amp_tag;
							copy_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power);
							result->total_out_driver_delay_data = total_out_driver;
							copy_powerDef(&result->total_out_driver_power_data,total_out_driver_power);
							result->compare_part_delay = compare_tag;
							copy_powerDef(&result->compare_part_power,compare_tag_power);
							result->drive_mux_delay = mux_driver;
							copy_powerDef(&result->drive_mux_power,mux_driver_power);
							result->selb_delay = selb;
							copy_powerDef(&result->selb_power,selb_power);
							result->drive_valid_delay = valid_driver;
							copy_powerDef(&result->drive_valid_power,valid_driver_power);
							result->data_output_delay = data_output;
							copy_powerDef(&result->data_output_power,data_output_power);
							result->precharge_delay = precharge_del;
							result->data_nor_inputs = data_nor_inputs;
							result->tag_nor_inputs = tag_nor_inputs;
		
							area_subbanked (ADDRESS_BITS, BITOUT,
									parameters->num_readwrite_ports,
									parameters->num_read_ports,
									parameters->num_write_ports, Ndbl,
									Ndwl, Nspd, Ntbl, Ntwl, Ntspd,
									*NSubbanks, parameters,
									arearesult_subbanked, arearesult);
		
							//v4.1: No longer using calculate_area function as area has already been
							//computed for the given tech node
							/*arearesult->efficiency =
							(*NSubbanks) * (area_all_dataramcells +
									area_all_tagramcells) * 100 /
							(calculate_area (*arearesult_subbanked,
										parameters->fudgefactor) /
								100000000.0);*/
		
							arearesult->efficiency =
								(*NSubbanks) * (area_all_dataramcells +
								area_all_tagramcells) * 100 /
							(arearesult_subbanked->height * arearesult_subbanked->width /100000000.0);
		
							arearesult->aspect_ratio_total =
								(arearesult_subbanked->height /
									arearesult_subbanked->width);
							arearesult->aspect_ratio_total =
								(arearesult->aspect_ratio_total >
									1.0) ? (arearesult->aspect_ratio_total) : 1.0 /
								(arearesult->aspect_ratio_total);
							arearesult->max_efficiency = max_efficiency;
							arearesult->max_aspect_ratio_total =
								max_aspect_ratio_total;
		
						}
					}
					else
					{
		
						if (result->max_access_time < access_time)
							result->max_access_time = access_time;
						if (result->max_power < 
							((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
							(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0)
						{
							result->max_power = ((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
													(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0;
						} //read and write dynamic energy components changed to power components by Shyam. Earlier 
						//read and write dynamic energies were being added to leakage power components and getting swamped
						if (arearesult_temp.max_efficiency < efficiency)
						{
							arearesult_temp.max_efficiency = efficiency;
							max_efficiency = efficiency;
						}
						if (min_efficiency > efficiency)
						{
							min_efficiency = efficiency;
						}
						if (max_aspect_ratio_total < aspect_ratio_total_temp)
						{
							max_aspect_ratio_total = aspect_ratio_total_temp;
						}
		
					}
				}
			}
		}
	}
}














void Cacti4_2::calculate_time(result_type *result,arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters, double *NSubbanks, int Ndwl, int Ndbl, int Ntwl, int Ntbl, int Ntspd, double Nspd, double voltage)
{
	arearesult_type arearesult_temp;
	area_type arearesult_subbanked_temp;
	
	int i;
	//int Ndwl, Ndbl, Ntwl, Ntbl, Ntspd, inter_subbanks, rows, columns,
	//tag_driver_size1, tag_driver_size2;
	int rows, columns;
	double Subbank_Efficiency, Total_Efficiency,/* max_efficiency,*/ efficiency /*, min_efficiency*/;
	double /*max_aspect_ratio_total,*/ aspect_ratio_total_temp;
	//double bank_h, bank_v, subbank_h, subbank_v, inter_h, inter_v;
	double bank_h, bank_v, subbank_h, subbank_v;
	double wirelength_v, wirelength_h;
	double access_time = 0;
	powerDef total_power;
	double before_mux = 0.0, after_mux = 0;
	powerDef total_power_allbanks;
	powerDef total_address_routing_power, total_power_without_routing;
	double subbank_address_routing_delay = 0.0;
	powerDef subbank_address_routing_power;
	double decoder_data_driver = 0.0, decoder_data_3to8 =
	0.0, decoder_data_inv = 0.0;
	double decoder_data = 0.0, decoder_tag = 0.0, wordline_data =
	0.0, wordline_tag = 0.0;
	powerDef decoder_data_power, decoder_tag_power, wordline_data_power, wordline_tag_power;
	double decoder_tag_driver = 0.0, decoder_tag_3to8 = 0.0, decoder_tag_inv =
	0.0;
	double bitline_data = 0.0, bitline_tag = 0.0, sense_amp_data =
	0.0, sense_amp_tag = 0.0;
	powerDef sense_amp_data_power, sense_amp_tag_power, bitline_tag_power, bitline_data_power;
	double compare_tag = 0.0, mux_driver = 0.0, data_output = 0.0, selb = 0.0;
	powerDef compare_tag_power, selb_power, mux_driver_power, valid_driver_power;
	powerDef data_output_power;
	double time_till_compare = 0.0, time_till_select = 0.0, valid_driver = 0.0;
	double cycle_time = 0.0, precharge_del = 0.0;
	double outrisetime = 0.0, inrisetime = 0.0, addr_inrisetime = 0.0;
	double senseext_scale = 1.0;
	double total_out_driver = 0.0;
	powerDef total_out_driver_power;
	double scale_init;
	int data_nor_inputs = 1, tag_nor_inputs = 1;
	double tag_delay_part1 = 0.0, tag_delay_part2 =
	0.0, tag_delay_part3 = 0.0, tag_delay_part4 = 0.0, tag_delay_part5 =
	0.0, tag_delay_part6 = 0.0;
	double max_delay = 0;
	int counter;
	double Tpre,Tpre_tag;
	int allports = 0, allreadports = 0, allwriteports = 0;
	int tag_iteration_counter = 0;
	double colsfa, desiredrisetimefa, Clinefa, Rpdrivefa; //Added by Shyam to incorporate FA caches 
	//in v4.0
	
	
	cached_tag_entry cached_tag_calculations;
	
	rows = parameters->number_of_sets;
	columns = 8 * parameters->block_size * parameters->data_associativity;
	FUDGEFACTOR = parameters->fudgefactor;
	VddPow = voltage; //4.5 / (pow (FUDGEFACTOR, (2.0 / 3.0)));
	
	/*dt: added to simulate accessing the tags and then only accessing one data way*/
	sequential_access_flag = parameters->sequential_access;
	/*dt: added to simulate fast and power hungry caches which route all the ways to the edge of the
		data array and do the way select there, instead of waiting for the way select signal to travel
		all the way to the subarray.
	*/
	fast_cache_access_flag = parameters->fast_access;
	pure_sram_flag = parameters->pure_sram;
	/* how many bits should this cache output?*/
	BITOUT = parameters->nr_bits_out;
	
	if (VddPow < 0.7)
		VddPow = 0.7;
	if (VddPow > 5.0)
		VddPow = 5.0;
	VbitprePow = VddPow * 3.3 / 4.5;
	parameters->VddPow = VddPow;
	
	/* go through possible Ndbl,Ndwl and find the smallest */
	/* Because of area considerations, I don't think it makes sense
		to break either dimension up larger than MAXN */
	
	/* only try moving output drivers for set associative cache */
	if (parameters->tag_associativity != 1)
	{
		scale_init = 0.1;
	}
	else
	{
		scale_init = 1.0;
	}
	precalc_leakage_params(VddPow,Tkelvin,Tox, tech_length0);
	calc_wire_parameters(parameters);
	
	result->cycle_time = BIGNUM;
	result->access_time = BIGNUM;
	result->total_power.readOp.dynamic = result->total_power.readOp.leakage = 
		result->total_power.writeOp.dynamic = result->total_power.writeOp.leakage =BIGNUM;
	result->best_muxover = 8 * parameters->block_size / BITOUT;
	result->max_access_time = 0;
	result->max_power = 0;
	arearesult_temp.max_efficiency = 1.0 / BIGNUM;
	//max_efficiency = 1.0 / BIGNUM;
	//min_efficiency = BIGNUM;
	arearesult->efficiency = 1.0 / BIGNUM;
	//max_aspect_ratio_total = 1.0 / BIGNUM;
	arearesult->aspect_ratio_total = BIGNUM;
	
	
	
	if (!parameters->fully_assoc)
	{
		/* Set associative or direct map cache model */
		
		/* Gilles: Nspd should be initialized here */
		/* Gilles: Ndwl should be initialized here */
		/* Gilles: Ndbl should be initialized here */
		if(data_organizational_parameters_valid
				(parameters->block_size,
				parameters->tag_associativity,
				parameters->cache_size,
				Ndwl, Ndbl,Nspd,
				parameters->fully_assoc,(*NSubbanks)))
		{
			tag_iteration_counter = 0;
			bank_h = 0;
			bank_v = 0;

			if (8 * parameters->block_size / BITOUT == 1 && Nspd == 1)
			{
				muxover = 1;
			}
			else
			{
				if (Nspd > MAX_COL_MUX)
				{
					muxover = 8 * parameters->block_size / BITOUT;
				}
				else
				{
					if (8 * parameters->block_size * Nspd / BITOUT > MAX_COL_MUX)
					{
						muxover = (8 * parameters->block_size / BITOUT) / (MAX_COL_MUX / (Nspd));
					}
					else
					{
						muxover = 1;
					}
				}
			}

			reset_data_device_widths();
			compute_device_widths(parameters->cache_size, parameters->block_size, parameters->tag_associativity,
								parameters->fully_assoc,Ndwl,Ndbl,Nspd);
			if(*NSubbanks == 1.0)
			{

				reset_powerDef(&total_address_routing_power);
				reset_powerDef(&subbank_address_routing_power);

				inrisetime = addr_inrisetime = 0;

				/* Calculate data side of cache */
				Tpre = 0;

				max_delay = 0;
				reset_powerDef(&decoder_data_power);
				decoder_data =
					decoder_delay (parameters->cache_size,
						parameters->block_size,
						parameters->data_associativity,
						Ndwl, Ndbl, Nspd,
						*NSubbanks,
						&decoder_data_driver,
						&decoder_data_3to8,
						&decoder_data_inv,
						inrisetime,
						&outrisetime,
						&data_nor_inputs,
						&decoder_data_power);

				Tpre = decoder_data;
				max_delay = MAX (max_delay, decoder_data);
				inrisetime = outrisetime;
				
				reset_powerDef(&wordline_data_power);
				wordline_data =
					wordline_delay (
						parameters->cache_size,
						parameters->block_size,
						parameters->data_associativity,
						Ndwl,Ndbl,Nspd,
						inrisetime,
						&outrisetime,
						&wordline_data_power);
				
				inrisetime = outrisetime;
				max_delay = MAX (max_delay, wordline_data);
				/*dt: assuming precharge delay is decoder + wordline delay */
				Tpre += wordline_data;

				reset_powerDef(&bitline_data_power);
				bitline_data =
					bitline_delay (parameters->cache_size,
						parameters->data_associativity,
						parameters->block_size,
						Ndwl, Ndbl, Nspd,
						inrisetime,
						&outrisetime,
						&bitline_data_power, Tpre);
				
				

				inrisetime = outrisetime;
				max_delay = MAX (max_delay, bitline_data);

				
				reset_powerDef(&sense_amp_data_power);
				sense_amp_data =
				sense_amp_delay (parameters->cache_size,
						parameters->block_size,
						parameters->data_associativity,
						Ndwl,Ndbl, Nspd,
						inrisetime,
						&outrisetime,
						&sense_amp_data_power);
				
				max_delay = MAX (max_delay, sense_amp_data);
				

				inrisetime = outrisetime;

				reset_powerDef(&data_output_power);
				data_output =
					dataoutput_delay (
						parameters->cache_size,
						parameters->block_size,
						parameters->data_associativity,
						parameters->fully_assoc,
						Ndbl, Nspd, Ndwl,
						inrisetime,
						&outrisetime,
						&data_output_power);
				
				max_delay = MAX (max_delay, data_output);
				inrisetime = outrisetime;

				reset_powerDef(&total_out_driver_power);

				subbank_v = 0;
				subbank_h = 0;

				inrisetime = outrisetime;
				max_delay = MAX (max_delay, total_out_driver);

				/* if the associativity is 1, the data output can come right
					after the sense amp.   Otherwise, it has to wait until 
					the data access has been done. 
				*/
				if (parameters->data_associativity == 1)
				{
					before_mux =
					subbank_address_routing_delay +
					decoder_data + wordline_data +
					bitline_data + sense_amp_data +
					total_out_driver + data_output;
					after_mux = 0;
				}
				else
				{
					before_mux =
					subbank_address_routing_delay +
					decoder_data + wordline_data +
					bitline_data + sense_amp_data;
					after_mux =
					data_output + total_out_driver;
				}
			}

			/* Gilles Ntspd should initialized here */
			/* Gilles Ntwl should initialized here */
			/* Gilles Ntbl should initialized here */
			
			if (tag_organizational_parameters_valid
				(parameters->block_size,
				parameters->tag_associativity,
				parameters->cache_size,
				Ntwl, Ntbl, Ntspd,
				parameters->fully_assoc,(*NSubbanks)))
			{
				tag_iteration_counter++;

				reset_tag_device_widths();
			
				compute_tag_device_widths(parameters->cache_size, parameters->block_size, parameters->tag_associativity,
															Ntspd,Ntwl,Ntbl,(*NSubbanks));

				area_subbanked (ADDRESS_BITS, BITOUT,
						parameters->num_readwrite_ports,
						parameters->num_read_ports,
						parameters->num_write_ports,
						Ndbl, Ndwl, Nspd, 
						Ntbl, Ntwl,Ntspd,
						*NSubbanks,
						parameters,
						&arearesult_subbanked_temp,
						&arearesult_temp);

				Subbank_Efficiency =
					(area_all_dataramcells +
						area_all_tagramcells) * 100 /
					(arearesult_temp.totalarea /
						100000000.0);
			
				//v4.1: No longer using calculate_area function as area has already been
				//computed for the given tech node
				/*Total_Efficiency =
				(*NSubbanks) *
				(area_all_dataramcells +
					area_all_tagramcells) * 100 /
				(calculate_area
					(arearesult_subbanked_temp,
					parameters->fudgefactor) /
					100000000.0);*/ 

				Total_Efficiency =
					(*NSubbanks) *
					(area_all_dataramcells +
						area_all_tagramcells) * 100 /
					(arearesult_subbanked_temp.height * arearesult_subbanked_temp.width/
						100000000.0);

				//efficiency = Subbank_Efficiency;
				efficiency = Total_Efficiency;

				arearesult_temp.efficiency = efficiency;
				aspect_ratio_total_temp =
					(arearesult_subbanked_temp.height /
					arearesult_subbanked_temp.width);
				aspect_ratio_total_temp =
					(aspect_ratio_total_temp > 1.0) ? 
					(aspect_ratio_total_temp) : 1.0 / (aspect_ratio_total_temp);

				arearesult_temp.aspect_ratio_total = aspect_ratio_total_temp;

				if (*NSubbanks > 1)
				{
					subbank_dim (parameters->cache_size,
						parameters->block_size,
						parameters->data_associativity,
						parameters->fully_assoc,
						Ndbl, Ndwl, Nspd, Ntbl,
						Ntwl, Ntspd, *NSubbanks,
						&bank_h, &bank_v);

					reset_powerDef(&total_address_routing_power);

					subbanks_routing_power (
								parameters->fully_assoc,
								parameters->data_associativity,
								*NSubbanks,
								&bank_h,
								&bank_v,
								&total_address_routing_power);

					reset_powerDef(&subbank_address_routing_power);
					/*dt: this has to be reset on every loop iteration, or else we implicitly reuse the risetime from the end
					of the last loop iteration!*/
					inrisetime = addr_inrisetime = 0;
					if (*NSubbanks > 2)
					{
						subbank_address_routing_delay =
						address_routing_delay
						(parameters->cache_size,
							parameters->block_size,
							parameters->data_associativity,
							parameters->fully_assoc, Ndwl,
							Ndbl, Nspd, Ntwl, Ntbl, Ntspd,
							NSubbanks, &outrisetime,
							&subbank_address_routing_power);
			
						/*dt: moved this here, because we only have extra signal slope if there's
							something before the decoder */
						inrisetime = outrisetime;
						addr_inrisetime = outrisetime;
					}
			
					/* Calculate data side of cache */
					Tpre = 0;

					max_delay = 0;
					reset_powerDef(&decoder_data_power);
					decoder_data =
						decoder_delay (parameters->cache_size,
									parameters->block_size,
									parameters->data_associativity,
									Ndwl, Ndbl, Nspd,
									*NSubbanks,
									&decoder_data_driver,
									&decoder_data_3to8,
									&decoder_data_inv,
									inrisetime,
									&outrisetime,
									&data_nor_inputs,
									&decoder_data_power);

					Tpre = decoder_data;
					max_delay = MAX (max_delay, decoder_data);
					inrisetime = outrisetime;
			
					reset_powerDef(&wordline_data_power);
					wordline_data =
						wordline_delay (
								parameters->cache_size,
								parameters->block_size,
								parameters->data_associativity,
								Ndwl,Ndbl,Nspd,
								inrisetime,
								&outrisetime,
								&wordline_data_power);
				
					inrisetime = outrisetime;
					max_delay = MAX (max_delay, wordline_data);
					/*dt: assuming precharge delay is decoder + wordline delay */
					Tpre += wordline_data;

					reset_powerDef(&bitline_data_power);
					bitline_data =
						bitline_delay (parameters->cache_size,
									parameters->data_associativity,
									parameters->block_size,
									Ndwl, Ndbl, Nspd,
									inrisetime,
									&outrisetime,
									&bitline_data_power, Tpre);
				

					inrisetime = outrisetime;
					max_delay = MAX (max_delay, bitline_data);

				
					reset_powerDef(&sense_amp_data_power);
					sense_amp_data =
						sense_amp_delay (parameters->cache_size,
							parameters->block_size,
							parameters->data_associativity,
							Ndwl,Ndbl, Nspd,
							inrisetime,
							&outrisetime,
							&sense_amp_data_power);
				
					max_delay = MAX (max_delay, sense_amp_data);
				

					inrisetime = outrisetime;

					reset_powerDef(&data_output_power);
					data_output =
						dataoutput_delay (
									parameters->cache_size,
									parameters->block_size,
									parameters->data_associativity,
									parameters->fully_assoc,
									Ndbl, Nspd, Ndwl,
									inrisetime,
									&outrisetime,
									&data_output_power);
				
					max_delay = MAX (max_delay, data_output);
					inrisetime = outrisetime;

					reset_powerDef(&total_out_driver_power);

					subbank_v = 0;
					subbank_h = 0;


					subbank_routing_length (
							parameters->cache_size,
							parameters->block_size,
							parameters->data_associativity,
							parameters->fully_assoc,
							Ndbl, Nspd,
							Ndwl, Ntbl,
							Ntwl, Ntspd,
							*NSubbanks,
							&subbank_v,
							&subbank_h);

					if (*NSubbanks > 2)
					{
						total_out_driver =
							senseext_driver_delay (
								parameters->data_associativity,
								parameters->fully_assoc,
								inrisetime, &outrisetime,
								subbank_v, subbank_h,
								&total_out_driver_power);
					}
				
					inrisetime = outrisetime;
					max_delay = MAX (max_delay, total_out_driver);

					/* if the associativity is 1, the data output can come right
						after the sense amp.   Otherwise, it has to wait until 
						the data access has been done. */

					if (parameters->data_associativity == 1)
					{
						before_mux =
							subbank_address_routing_delay +
							decoder_data + wordline_data +
							bitline_data + sense_amp_data +
							total_out_driver + data_output;
							after_mux = 0;
					}
					else
					{
						before_mux =
							subbank_address_routing_delay +
							decoder_data + wordline_data +
							bitline_data + sense_amp_data;
							after_mux =
							data_output + total_out_driver;
					}
				}

				/*
				* Now worry about the tag side.
				*/


				reset_powerDef(&decoder_tag_power);
				decoder_tag =
					decoder_tag_delay (
						parameters->cache_size,
						parameters->block_size,
						parameters->tag_associativity,
						Ntwl, Ntbl, Ntspd,
						*NSubbanks,
						&decoder_tag_driver,
						&decoder_tag_3to8,
						&decoder_tag_inv,
						addr_inrisetime,
						&outrisetime,
						&tag_nor_inputs,
						&decoder_tag_power);

				cached_tag_calculations.decoder.delay = decoder_tag;
				copy_powerDef(
					&(cached_tag_calculations.decoder.power)
					,decoder_tag_power);
				
				max_delay =
					MAX (max_delay, decoder_tag);
						Tpre_tag = decoder_tag;
						inrisetime = outrisetime;

				reset_powerDef(&wordline_tag_power);

				wordline_tag =
					wordline_tag_delay (
							parameters->cache_size,
							parameters->block_size,
							parameters->tag_associativity,
							Ntspd, Ntwl, Ntbl,
							*NSubbanks,
							inrisetime,
							&outrisetime,
							&wordline_tag_power);

				cached_tag_calculations.wordline.delay = wordline_tag;
				copy_powerDef(
					&(cached_tag_calculations.wordline.power)
					,wordline_tag_power);
				
				max_delay =
					MAX (max_delay, wordline_tag);
						inrisetime = outrisetime;
						Tpre_tag += wordline_tag;

				reset_powerDef(&bitline_tag_power);
				bitline_tag =
					bitline_tag_delay (
						parameters->cache_size,
						parameters->tag_associativity,
						parameters->block_size,
						Ntwl,Ntbl, Ntspd,
						*NSubbanks,
						inrisetime,
						&outrisetime,
						&bitline_tag_power, Tpre_tag);

				cached_tag_calculations.bitline.delay = bitline_tag;
				copy_powerDef(
					&(cached_tag_calculations.bitline.power)
					,bitline_tag_power);
				
				max_delay =
					MAX (max_delay, bitline_tag);
						inrisetime = outrisetime;
				
				reset_powerDef(&sense_amp_tag_power);

				sense_amp_tag =
					sense_amp_tag_delay (
						parameters->cache_size,
						parameters->block_size,
						parameters->tag_associativity,
						Ntwl, Ntbl, Ntspd, *NSubbanks,
						inrisetime, &outrisetime,
						&sense_amp_tag_power);

				cached_tag_calculations.senseamp.delay = sense_amp_tag;
				copy_powerDef(
					&(cached_tag_calculations.senseamp.power)
					,sense_amp_tag_power);

				cached_tag_calculations.senseamp_outputrisetime = outrisetime;

				max_delay = MAX (max_delay, sense_amp_tag);
				inrisetime = outrisetime;

				wirelength_v = wirelength_h = 0;
				if(parameters->tag_associativity != 1)
				{
					precalc_muxdrv_widths(parameters->cache_size,
						parameters->block_size,
						parameters->data_associativity,
						Ndwl, Ndbl, Nspd,
						&wirelength_v, &wirelength_h);
				}

				/* split comparator - look at half the address bits only */
				reset_powerDef(&compare_tag_power);
				compare_tag =
					half_compare_delay (
								parameters->cache_size,
								parameters->block_size,
								parameters->tag_associativity,
								Ntwl,Ntbl, Ntspd,
								*NSubbanks,
								inrisetime,
								&outrisetime,
								&compare_tag_power);
				
				inrisetime = outrisetime;
				max_delay = MAX (max_delay, compare_tag);

				reset_powerDef(&valid_driver_power);
				reset_powerDef(&mux_driver_power);
				reset_powerDef(&selb_power);
				if (parameters->tag_associativity == 1)
				{
					mux_driver = 0;
					valid_driver =
					valid_driver_delay (
							parameters->cache_size,
							parameters->block_size,
							parameters->tag_associativity,
							parameters->fully_assoc,
							Ndbl, Ndwl,
							Nspd, Ntbl,
							Ntwl, Ntspd,
							NSubbanks,
							inrisetime,
							&valid_driver_power);
				
					max_delay =
					MAX (max_delay, valid_driver);
					time_till_compare =
					subbank_address_routing_delay +
					decoder_tag + wordline_tag +
					bitline_tag + sense_amp_tag;

					time_till_select =
					time_till_compare + compare_tag +
					valid_driver;


					/*
					* From the above info, calculate the total access time
					*/
					/*If we're doing a pure SRAM table, then we don't need to take into account tags */
					if(pure_sram_flag)
					{
						access_time = before_mux + after_mux;
					}
					else
					{
						if(sequential_access_flag)
						{
							/*dt: testing sequential access mode*/
							/*approximating the time it takes the match and valid signal to travel to the edge of the tag array with decode_tag */
							access_time = before_mux + time_till_compare + compare_tag + decoder_tag;
						}
						else
						{
							access_time =
								MAX (before_mux + after_mux,
								time_till_select);
						}
					}
				}
				else
				{
					if(fast_cache_access_flag||sequential_access_flag)
					{
						mux_driver = 0;
					}//||sequential_access_flag added by Shyam to fix the bug
					//of mux driver delay being larger than access time in serial access mode
					//now mux driver delay code won't be touched in serial access mode
					else
					{
						/* dualin mux driver - added for split comparator
							- inverter replaced by nand gate */
						mux_driver =
								mux_driver_delay_dualin
									(parameters->cache_size,
									parameters->block_size,
									parameters->tag_associativity,
									Ntbl, Ntspd,
									inrisetime, &outrisetime,
									wirelength_v, wirelength_h,
									&mux_driver_power);
					}
					max_delay = MAX (max_delay, mux_driver);

					selb = selb_delay_tag_path (inrisetime,
								&outrisetime,
								&selb_power);
					max_delay = MAX (max_delay, selb);

					valid_driver = 0;

					time_till_compare =
						subbank_address_routing_delay +
						decoder_tag + wordline_tag +
						bitline_tag + sense_amp_tag;

					if(fast_cache_access_flag)
					{
						/*dt: don't wait for the mux signal to travel to the subarray, have the n ways
						routed to the edge of the data array*/
						time_till_select =
							time_till_compare + compare_tag +
							selb;
					}
					else
					{
						time_till_select =
							time_till_compare + compare_tag +
							mux_driver +
							selb;
					}
				
					if(sequential_access_flag)
					{
						/*dt: testing sequential access*/
						/*approximating the time it takes the match and valid signal to travel to the edge of the tag array with decode_tag */
						access_time = before_mux + time_till_compare + compare_tag + decoder_tag;
					}
					else
					{
						access_time =
							MAX (before_mux,
							time_till_select) +
							after_mux;
					}
				}

				/*
				* Calcuate the cycle time
				*/

				precharge_del = precharge_delay(wordline_data);

				/*dt: replacing this with a simple calculation:
						The things which cannot be easily pipelined are:
						wordline and bitlin
						senseamp
						comparator
						so those are the things which are going to limit cycle time
				*/
				cycle_time = MAX(
								MAX(wordline_data + bitline_data + sense_amp_data,
									wordline_tag  + bitline_tag  + sense_amp_tag),
								compare_tag);

				/*
				* The parameters are for a 0.8um process.  A quick way to
				* scale the results to another process is to divide all
				* the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
				*/
	
				//v4.1: All delay and energy components are no longer scaled by FUDGEFACTOR as they have already
				//been computed for the input tech node.
				allports = parameters->num_readwrite_ports + parameters->num_read_ports + parameters->num_write_ports;
				allreadports = parameters->num_readwrite_ports + parameters->num_read_ports;
				allwriteports = parameters->num_readwrite_ports + parameters->num_write_ports;

				/*dt:We'll add this at the end*/
				mult_powerDef(&total_address_routing_power,allports);
		
				reset_powerDef(&total_power);
		
				mac_powerDef(&total_power,&subbank_address_routing_power,allports);
				//v4.2: While calculating total read (and write) energy, the decoder and wordline
				//energies per read (or write) port were being multiplied by total number of ports
				//which is not right
				//mac_powerDef(&total_power,&decoder_data_power,allports);
				total_power.readOp.dynamic += decoder_data_power.readOp.dynamic * allreadports;
				total_power.writeOp.dynamic += decoder_data_power.writeOp.dynamic * allwriteports;
				//mac_powerDef(&total_power,&wordline_data_power,allports);
				total_power.readOp.dynamic += wordline_data_power.readOp.dynamic * allreadports;
				total_power.writeOp.dynamic += wordline_data_power.writeOp.dynamic * allwriteports;
				/*dt: We can have different numbers of read/write ports, so the power numbers have to keep that in mind. Ports which can do both reads and
				writes are counted as writes for max power, because writes use full swing and thus use more power. (Assuming full bitline swing is greater 
				than senseamp power) */
				total_power.readOp.dynamic += bitline_data_power.readOp.dynamic * allreadports;
				total_power.writeOp.dynamic += bitline_data_power.writeOp.dynamic * allwriteports;
				/*dt: for multiported SRAM cells we assume NAND stacks on all the pure read ports. We assume neglegible
				leakage for these ports. The following code adjusts the leakage numbers accordingly.*/
				total_power.readOp.leakage += bitline_data_power.readOp.leakage * allreadports;//changed to allreadports by Shyam. Earlier this was allwriteports.
				total_power.writeOp.leakage += bitline_data_power.writeOp.leakage * allwriteports; 

				mac_powerDef(&total_power,&sense_amp_data_power,allreadports);
				mac_powerDef(&total_power,&total_out_driver_power,allreadports);
				/*dt: Tags are only written to when a cache line is evicted/replaced. The only bit which is written is the dirty bit. The dirty bits are kept
				in the data array. */
				//if on pure_sram_flag added by Shyam since tag power should not be added to total power in pure SRAM mode.
				if(!pure_sram_flag)
				{
					mac_powerDef(&total_power,&decoder_tag_power,allreadports);
					mac_powerDef(&total_power,&wordline_tag_power,allreadports);
					mac_powerDef(&total_power,&bitline_tag_power,allreadports);
					mac_powerDef(&total_power,&sense_amp_tag_power,allreadports);
					mac_powerDef(&total_power,&compare_tag_power,allreadports);
					mac_powerDef(&total_power,&valid_driver_power,allreadports);
					mac_powerDef(&total_power,&mux_driver_power,allreadports);
					mac_powerDef(&total_power,&selb_power,allreadports);
				}
				mac_powerDef(&total_power,&data_output_power,allreadports);


				reset_powerDef(&total_power_without_routing);
				mac_powerDef(&total_power_without_routing,&total_power,1); // copy over and ..

				//total_power.readOp.dynamic /= FUDGEFACTOR;
				//total_power.writeOp.dynamic /= FUDGEFACTOR;
				/*dt: Leakage isn't scaled with FUDGEFACTOR, because that's already done by the leakage model much more realistically */

	
				mac_powerDef(&total_power_without_routing,&subbank_address_routing_power,-allports); // ... then subtract ..
				mac_powerDef(&total_power_without_routing,&total_out_driver_power,-allreadports);
				mac_powerDef(&total_power_without_routing,&valid_driver_power,-allreadports);
	
				/*total_power_without_routing.readOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
				total_power_without_routing.readOp.leakage *= (*NSubbanks);
				total_power_without_routing.writeOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
				total_power_without_routing.writeOp.leakage *= (*NSubbanks);*/
		
				total_power_without_routing.readOp.dynamic *= (*NSubbanks);
				total_power_without_routing.readOp.leakage *= (*NSubbanks);
				total_power_without_routing.writeOp.dynamic *= (*NSubbanks);
				total_power_without_routing.writeOp.leakage *= (*NSubbanks);
				/*dt: see above for leakage*/

				reset_powerDef(&total_power_allbanks);
				//total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic / FUDGEFACTOR;
				total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic;
				total_power_allbanks.readOp.leakage = total_power_without_routing.readOp.leakage + total_address_routing_power.readOp.leakage;
				//total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic / FUDGEFACTOR;
				total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic;
				total_power_allbanks.writeOp.leakage = total_power_without_routing.writeOp.leakage + total_address_routing_power.writeOp.leakage;

				//      if (counter==1)
				//        fprintf(stderr, "Pow - %f, Acc - %f, Pow - %f, Acc - %f, Combo - %f\n", total_power*1e9, access_time*1e9, total_power/result->max_power, access_time/result->max_access_time, total_power/result->max_power*access_time/result->max_access_time);

				/* Gilles: Filling of the result follows */
				result->senseext_scale = senseext_scale;
				copy_powerDef(&result->total_power,total_power);
				copy_powerDef(&result->total_power_without_routing,total_power_without_routing);
				//copy_and_div_powerDef(&result->total_routing_power,total_address_routing_power,FUDGEFACTOR);
				copy_powerDef(&result->total_routing_power,total_address_routing_power);
				copy_powerDef(&result->total_power_allbanks,total_power_allbanks);

				result->subbank_address_routing_delay = subbank_address_routing_delay / FUDGEFACTOR;
				//copy_and_div_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power,FUDGEFACTOR);
				copy_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power);	

				//result->cycle_time = cycle_time / FUDGEFACTOR;
				//result->access_time = access_time / FUDGEFACTOR;
				result->cycle_time = cycle_time;
				result->access_time = access_time;
				result->best_muxover = muxover;
				result->best_Ndwl = Ndwl;
				result->best_Ndbl = Ndbl;
				result->best_Nspd = Nspd;
				result->best_Ntwl = Ntwl;
				result->best_Ntbl = Ntbl;
				result->best_Ntspd = Ntspd;
				/*result->decoder_delay_data = decoder_data / FUDGEFACTOR;
				copy_and_div_powerDef(&result->decoder_power_data,decoder_data_power,FUDGEFACTOR);
				result->decoder_delay_tag = decoder_tag / FUDGEFACTOR;
				copy_and_div_powerDef(&result->decoder_power_tag,decoder_tag_power,FUDGEFACTOR);
				result->dec_tag_driver = decoder_tag_driver / FUDGEFACTOR;
				result->dec_tag_3to8 = decoder_tag_3to8 / FUDGEFACTOR;
				result->dec_tag_inv = decoder_tag_inv / FUDGEFACTOR;
				result->dec_data_driver = decoder_data_driver / FUDGEFACTOR;
				result->dec_data_3to8 = decoder_data_3to8 / FUDGEFACTOR;
				result->dec_data_inv = decoder_data_inv / FUDGEFACTOR;
				result->wordline_delay_data = wordline_data / FUDGEFACTOR;
				copy_and_div_powerDef(&result->wordline_power_data,wordline_data_power,FUDGEFACTOR);
				result->wordline_delay_tag = wordline_tag / FUDGEFACTOR;
				copy_and_div_powerDef(&result->wordline_power_tag,wordline_tag_power,FUDGEFACTOR);
				result->bitline_delay_data = bitline_data / FUDGEFACTOR;
				copy_and_div_powerDef(&result->bitline_power_data,bitline_data_power,FUDGEFACTOR);
				result->bitline_delay_tag = bitline_tag / FUDGEFACTOR;
				copy_and_div_powerDef(&result->bitline_power_tag,bitline_tag_power,FUDGEFACTOR);
				result->sense_amp_delay_data = sense_amp_data / FUDGEFACTOR;
				copy_and_div_powerDef(&result->sense_amp_power_data,sense_amp_data_power,FUDGEFACTOR);
				result->sense_amp_delay_tag = sense_amp_tag / FUDGEFACTOR;
				copy_and_div_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power,FUDGEFACTOR);
				result->total_out_driver_delay_data = total_out_driver / FUDGEFACTOR;
				copy_and_div_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
				result->compare_part_delay = compare_tag / FUDGEFACTOR;
				copy_and_div_powerDef(&result->compare_part_power,compare_tag_power,FUDGEFACTOR);
				result->drive_mux_delay = mux_driver / FUDGEFACTOR;
				copy_and_div_powerDef(&result->drive_mux_power,mux_driver_power,FUDGEFACTOR);
				result->selb_delay = selb / FUDGEFACTOR;
				copy_and_div_powerDef(&result->selb_power,selb_power,FUDGEFACTOR);
				result->drive_valid_delay = valid_driver / FUDGEFACTOR;
				copy_and_div_powerDef(&result->drive_valid_power,valid_driver_power,FUDGEFACTOR);
				result->data_output_delay = data_output / FUDGEFACTOR;
				copy_and_div_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);
				result->precharge_delay = precharge_del / FUDGEFACTOR;*/

				result->decoder_delay_data = decoder_data;
				copy_powerDef(&result->decoder_power_data,decoder_data_power);
				result->decoder_delay_tag = decoder_tag;
				copy_powerDef(&result->decoder_power_tag,decoder_tag_power);
				result->dec_tag_driver = decoder_tag_driver;
				result->dec_tag_3to8 = decoder_tag_3to8;
				result->dec_tag_inv = decoder_tag_inv;
				result->dec_data_driver = decoder_data_driver;
				result->dec_data_3to8 = decoder_data_3to8;
				result->dec_data_inv = decoder_data_inv;
				result->wordline_delay_data = wordline_data;
				copy_powerDef(&result->wordline_power_data,wordline_data_power);
				result->wordline_delay_tag = wordline_tag;
				copy_powerDef(&result->wordline_power_tag,wordline_tag_power);
				result->bitline_delay_data = bitline_data;
				copy_powerDef(&result->bitline_power_data,bitline_data_power);
				result->bitline_delay_tag = bitline_tag;
				copy_powerDef(&result->bitline_power_tag,bitline_tag_power);
				result->sense_amp_delay_data = sense_amp_data;
				copy_powerDef(&result->sense_amp_power_data,sense_amp_data_power);
				result->sense_amp_delay_tag = sense_amp_tag;
				copy_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power);
				result->total_out_driver_delay_data = total_out_driver;
				copy_powerDef(&result->total_out_driver_power_data,total_out_driver_power);
				result->compare_part_delay = compare_tag;
				copy_powerDef(&result->compare_part_power,compare_tag_power);
				result->drive_mux_delay = mux_driver;
				copy_powerDef(&result->drive_mux_power,mux_driver_power);
				result->selb_delay = selb;
				copy_powerDef(&result->selb_power,selb_power);
				result->drive_valid_delay = valid_driver;
				copy_powerDef(&result->drive_valid_power,valid_driver_power);
				result->data_output_delay = data_output;
				copy_powerDef(&result->data_output_power,data_output_power);
				result->precharge_delay = precharge_del;



				result->data_nor_inputs = data_nor_inputs;
				result->tag_nor_inputs = tag_nor_inputs;
				area_subbanked (ADDRESS_BITS,
						BITOUT,
						parameters->
						num_readwrite_ports,
						parameters->
						num_read_ports,
						parameters->
						num_write_ports,
						Ndbl, Ndwl,
						Nspd, Ntbl,
						Ntwl, Ntspd,
						*NSubbanks,
						parameters,
						arearesult_subbanked,
						arearesult);

			
				//v4.1: No longer using calculate_area function as area has already been
				//computed for the given tech node
							/*arearesult->efficiency =
						(*NSubbanks) *
						(area_all_dataramcells +
							area_all_tagramcells) * 100 /
						(calculate_area
							(*arearesult_subbanked,
							parameters->fudgefactor) / 100000000.0);*/


				/* Gilles: I no longer can compute arearesult->efficiency */
/*					arearesult->efficiency =
					(*NSubbanks) *
					(area_all_dataramcells +
						area_all_tagramcells) * 100 /
					(arearesult_subbanked->height * arearesult_subbanked->width / 100000000.0);*/
				arearesult->aspect_ratio_total =
						(arearesult_subbanked->height / arearesult_subbanked->width);
				arearesult->aspect_ratio_total =
					(arearesult->aspect_ratio_total > 1.0) ? 
					(arearesult->aspect_ratio_total) : 1.0 / (arearesult->aspect_ratio_total);
				/* Gilles: I no longer can compute max_efficiency */
/*					arearesult->max_efficiency = max_efficiency;
					arearesult->max_aspect_ratio_total = max_aspect_ratio_total;*/
				
				/* Gilles: I no longer can compute max_access_time */
/*					if (result->max_access_time < access_time)
				{
					result->max_access_time = access_time ;
				}*/
				
				/* Gilles: I no longer can compute max_power */
/*					if (result->max_power < 
					((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
										(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0)
				{
					result->max_power = ((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
										(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0;
					//read and write dynamic energy components changed to power components by Shyam. Earlier 
					//read and write dynamic energies were being added to leakage power components and getting swamped
				}*/
				
				/* Gilles: I no longer can compute max_efficiency */
// 					if (arearesult_temp.max_efficiency < efficiency)
// 					{
// 						arearesult_temp.max_efficiency = efficiency;
// 						max_efficiency = efficiency;
// 					}
				
				/* Gilles: I no longer can compute min_efficiency */
/*					if (min_efficiency > efficiency)
				{
					min_efficiency = efficiency;
				}*/
				
				/* Gilles: I no longer can compute max_aspect_ratio_total */
/*					if (max_aspect_ratio_total < aspect_ratio_total_temp)
				{
					max_aspect_ratio_total = aspect_ratio_total_temp;
				}*/
			}
		}
	}
	else
	{
		/* Fully associative model - only vary Ndbl|Ntbl */
	
		/* Gilles: Ndbl should be initialized here */
		
		Ntbl = Ndbl;
		//v4.1: Nspd is now of double data type
		//Ndwl = Nspd = Ntwl = Ntspd = 1;
		Ndwl = Ntwl = Ntspd = 1;
		Nspd = 1.0;


		if (data_organizational_parameters_valid
			(parameters->block_size, 1, parameters->cache_size, Ndwl,
			Ndbl, Nspd, parameters->fully_assoc,(*NSubbanks)))
		{
		
			if (8 * parameters->block_size / BITOUT == 1
				&& Nspd == 1)
			{
				muxover = 1;
			}
			else
			{
				if (Nspd > MAX_COL_MUX)
				{
					muxover = 8 * parameters->block_size / BITOUT;
				}
				else
				{
					if (8 * parameters->block_size * Nspd /
						BITOUT > MAX_COL_MUX)
					{
						muxover =
							(8 * parameters->block_size / BITOUT) /
							(MAX_COL_MUX / (Nspd));
					}
					else
					{
						muxover = 1;
					}
				}
			}
		
		
			area_subbanked (ADDRESS_BITS, BITOUT,
					parameters->num_readwrite_ports,
					parameters->num_read_ports,
					parameters->num_write_ports, Ndbl, Ndwl,
					Nspd, Ntbl, Ntwl, Ntspd, *NSubbanks,
					parameters, &arearesult_subbanked_temp,
					&arearesult_temp);
		
			Subbank_Efficiency =
				(area_all_dataramcells +
					area_all_tagramcells) * 100 /
				(arearesult_temp.totalarea / 100000000.0);
		
			//v4.1: No longer using calculate_area function as area has already been
			//computed for the given tech node
			/*Total_Efficiency =
			(*NSubbanks) * (area_all_dataramcells +
					area_all_tagramcells) * 100 /
			(calculate_area (arearesult_subbanked_temp,
						parameters->fudgefactor) / 100000000.0);*/
		
			Total_Efficiency =
				(*NSubbanks) * (area_all_dataramcells +
				area_all_tagramcells) * 100 /
				(arearesult_subbanked_temp.height * arearesult_subbanked_temp.width / 100000000.0);
			// efficiency = Subbank_Efficiency;
			efficiency = Total_Efficiency;
	
			arearesult_temp.efficiency = efficiency;
			aspect_ratio_total_temp =
				(arearesult_subbanked_temp.height /
					arearesult_subbanked_temp.width);
			aspect_ratio_total_temp =
				(aspect_ratio_total_temp >
				1.0) ? (aspect_ratio_total_temp) : 1.0 /
				(aspect_ratio_total_temp);
	
			arearesult_temp.aspect_ratio_total =
				aspect_ratio_total_temp;
	
			bank_h = 0;
			bank_v = 0;

			subbank_dim (parameters->cache_size, parameters->block_size,
					parameters->data_associativity,
					parameters->fully_assoc, Ndbl, Ndwl, Nspd,
					Ntbl, Ntwl, Ntspd, *NSubbanks, &bank_h,
					&bank_v);

			reset_powerDef(&subbank_address_routing_power);
			/*dt: this has to be reset on every loop iteration, or else we implicitly reuse the risetime from the end
			of the last loop iteration!*/
			inrisetime = addr_inrisetime = 0;

			subbanks_routing_power (parameters->fully_assoc,
						parameters->data_associativity,
						*NSubbanks, &bank_h, &bank_v,
						&total_address_routing_power);

			if (*NSubbanks > 2)
			{
				subbank_address_routing_delay =
				address_routing_delay (parameters->cache_size,
							parameters->block_size,
							parameters->data_associativity,
							parameters->fully_assoc, Ndwl,
							Ndbl, Nspd, Ntwl, Ntbl, Ntspd,
							NSubbanks, &outrisetime,
							&subbank_address_routing_power);
			}

		

			/* Calculate data side of cache */
			inrisetime = outrisetime;
			addr_inrisetime = outrisetime;
	
			max_delay = 0;
			/* tag path contained here */
			reset_powerDef(&decoder_data_power);
			decoder_data = fa_tag_delay (parameters->cache_size,
							parameters->block_size,
							Ntwl, Ntbl, Ntspd,
							&tag_delay_part1,
							&tag_delay_part2,
							&tag_delay_part3,
							&tag_delay_part4,
							&tag_delay_part5,
							&tag_delay_part6, &outrisetime,
							&tag_nor_inputs,
							&decoder_data_power);

			inrisetime = outrisetime;
			max_delay = MAX (max_delay, decoder_data);
			Tpre = decoder_data;


			reset_powerDef(&wordline_data_power);

			//Added by Shyam to make FA caches work
			Wdecinvn = 20.0 / FUDGEFACTOR;//From v3.2 #define value
			Wdecinvp = 40.0 / FUDGEFACTOR;//From v3.2 #define value
			Woutdrvseln = 24.0 / FUDGEFACTOR;
			Woutdrvselp = 40.0 / FUDGEFACTOR;
			Woutdrvnandn = 10.0 / FUDGEFACTOR;
			Woutdrvnandp = 30.0 / FUDGEFACTOR;
			Woutdrvnorn = 5.0 / FUDGEFACTOR;
			Woutdrvnorp = 20.0 / FUDGEFACTOR;
			Woutdrivern = 48.0 / FUDGEFACTOR;
			Woutdriverp = 80.0 / FUDGEFACTOR;
		
			colsfa = CHUNKSIZE*parameters->block_size*1*Nspd/Ndwl;
			desiredrisetimefa = krise*log((double)(colsfa))/2.0;
			Clinefa = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ Cwordmetal)*colsfa;
			Rpdrivefa = desiredrisetimefa/(Clinefa*log(VSINV)*-1.0);
			WwlDrvp = restowidth(Rpdrivefa,PCH);
			if (WwlDrvp > Wworddrivemax)
			{
				WwlDrvp = Wworddrivemax;
			}
			//End of Shyam's FA change

			wordline_data = wordline_delay (
							parameters->cache_size,
							parameters->block_size,
							1, Ndwl, Ndbl,Nspd,
							inrisetime, &outrisetime,
							&wordline_data_power);
		
			inrisetime = outrisetime;
			max_delay = MAX (max_delay, wordline_data);
			/*dt: assuming that the precharge delay is equal to decode + wordline delay */
			Tpre += wordline_data;

			reset_powerDef(&bitline_data_power);
			bitline_data = bitline_delay (parameters->cache_size, 1,
						parameters->block_size, Ndwl,
						Ndbl, Nspd, inrisetime,
						&outrisetime,
						&bitline_data_power, Tpre);
		
		
			inrisetime = outrisetime;
			max_delay = MAX (max_delay, bitline_data);

			{
				reset_powerDef(&sense_amp_data_power);
				sense_amp_data =
					sense_amp_delay (parameters->cache_size,
							parameters->block_size,
							parameters->data_associativity,
							Ndwl, Ndbl, Nspd,
							inrisetime, &outrisetime,
							&sense_amp_data_power);
				
				max_delay = MAX (max_delay, sense_amp_data);
			}
			inrisetime = outrisetime;
	
			reset_powerDef(&data_output_power);
			data_output =
			dataoutput_delay (parameters->cache_size,
						parameters->block_size, 1,
						parameters->fully_assoc, 
						Ndbl, Nspd, Ndwl,
						inrisetime, &outrisetime,
						&data_output_power);
	
			inrisetime = outrisetime;
			max_delay = MAX (max_delay, data_output);
	
			reset_powerDef(&total_out_driver_power);
	
			subbank_v = 0;
			subbank_h = 0;

			subbank_routing_length (parameters->cache_size,
						parameters->block_size,
						parameters->data_associativity,
						parameters->fully_assoc, Ndbl, Nspd,
						Ndwl, Ntbl, Ntwl, Ntspd, *NSubbanks,
						&subbank_v, &subbank_h);
	
			if (*NSubbanks > 2)
			{
				total_out_driver =
					senseext_driver_delay (parameters->data_associativity,
						parameters->fully_assoc,
						inrisetime, &outrisetime,
						subbank_v, subbank_h,
						&total_out_driver_power);
			}

/*
			total_out_driver = total_out_driver_delay(parameters->cache_size,
				parameters->block_size,parameters->associativity,parameters->fully_assoc,
				Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,
				*NSubbanks,inrisetime,&outrisetime, &total_out_driver_power);
*/
		
			inrisetime = outrisetime;
			max_delay = MAX (max_delay, total_out_driver);

			access_time =
				subbank_address_routing_delay + decoder_data +
				wordline_data + bitline_data + sense_amp_data +
				data_output + total_out_driver;

			/*
			* Calcuate the cycle time
			*/

			precharge_del = precharge_delay(wordline_data);

			cycle_time = MAX(
							MAX(wordline_data + bitline_data + sense_amp_data,
								wordline_tag  + bitline_tag + sense_amp_tag),
							compare_tag);

			/*
			* The parameters are for a 0.8um process.  A quick way to
			* scale the results to another process is to divide all
			* the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
			*/
			/*dt: see previous comment on sense amp leakage*/
			/*
			sense_amp_data_power +=
			(data_output + total_out_driver) * 500e-6 * 5;
			*/
			allports = parameters->num_readwrite_ports + parameters->num_read_ports + parameters->num_write_ports;
			allreadports = parameters->num_readwrite_ports + parameters->num_read_ports;
			allwriteports = parameters->num_readwrite_ports + parameters->num_write_ports;

			mult_powerDef(&total_address_routing_power,allports);

			reset_powerDef(&total_power);
	
			mac_powerDef(&total_power,&subbank_address_routing_power,allports);
			
			//v4.2: While calculating total read (and write) energy, the decoder and wordline
			//energies per read (or write) port were being multiplied by total number of ports
			//which is not right
			//mac_powerDef(&total_power,&decoder_data_power,allports);
			total_power.readOp.dynamic += decoder_data_power.readOp.dynamic * allreadports;
			total_power.writeOp.dynamic += decoder_data_power.writeOp.dynamic * allwriteports;
			//mac_powerDef(&total_power,&wordline_data_power,allports);
			total_power.readOp.dynamic += wordline_data_power.readOp.dynamic * allreadports;
			total_power.writeOp.dynamic += wordline_data_power.writeOp.dynamic * allwriteports;
	
			/*dt: We can have different numbers of read/write ports, so the power numbers have to keep that in mind. Ports which can do both reads and
			writes are counted as writes for max power, because writes use full swing and thus use more power. (Assuming full bitline swing is greater 
			than senseamp power) */
			total_power.readOp.dynamic += bitline_data_power.readOp.dynamic * allreadports;
			total_power.writeOp.dynamic += bitline_data_power.writeOp.dynamic * allwriteports;
			/*dt: for multiported SRAM cells we assume NAND stacks on all the pure read ports. We assume neglegible
			leakage for these ports. The following code adjusts the leakage numbers accordingly.*/
			total_power.readOp.leakage += bitline_data_power.readOp.leakage * allwriteports;
			total_power.writeOp.leakage += bitline_data_power.writeOp.leakage * allwriteports; 
	
			mac_powerDef(&total_power,&sense_amp_data_power,allreadports);
			mac_powerDef(&total_power,&total_out_driver_power,allreadports);
			/*dt: in a fully associative cache we don't have tag decoders, wordlines or bitlines, etc. . Only CAM */
			reset_powerDef(&decoder_tag_power);
			reset_powerDef(&wordline_tag_power);
			reset_powerDef(&bitline_tag_power);
			reset_powerDef(&sense_amp_tag_power);
			reset_powerDef(&compare_tag_power);
			reset_powerDef(&valid_driver_power);
			reset_powerDef(&mux_driver_power);
			reset_powerDef(&selb_power);

			mac_powerDef(&total_power,&data_output_power,allreadports);
	
			reset_powerDef(&total_power_without_routing);
			mac_powerDef(&total_power_without_routing,&total_power,1); // copy over and ..

			//total_power.readOp.dynamic /= FUDGEFACTOR;
			//total_power.writeOp.dynamic /= FUDGEFACTOR;
			/*dt: Leakage isn't scaled with FUDGEFACTOR, because that's already done by the leakage model much more realistically */
	
			mac_powerDef(&total_power_without_routing,&subbank_address_routing_power,-allports); // ... then subtract ..
			mac_powerDef(&total_power_without_routing,&total_out_driver_power,-allreadports);
			mac_powerDef(&total_power_without_routing,&valid_driver_power,-allreadports);
	
			/*dt: See above for leakage */
			//total_power_without_routing.readOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
			total_power_without_routing.readOp.dynamic *= (*NSubbanks);
			total_power_without_routing.readOp.leakage *= (*NSubbanks);
			//total_power_without_routing.writeOp.dynamic *= (*NSubbanks)/ FUDGEFACTOR;
			total_power_without_routing.writeOp.dynamic *= (*NSubbanks);
			total_power_without_routing.writeOp.leakage *= (*NSubbanks);
	
			//total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic / FUDGEFACTOR;
			total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_address_routing_power.readOp.dynamic ;
			total_power_allbanks.readOp.leakage = total_power_without_routing.readOp.leakage + total_address_routing_power.readOp.leakage;
			//total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic / FUDGEFACTOR;
			total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_address_routing_power.writeOp.dynamic;
			total_power_allbanks.writeOp.leakage = total_power_without_routing.writeOp.leakage + total_address_routing_power.writeOp.leakage;

			/* Gilles: Filling the result structure follows */
			result->senseext_scale = senseext_scale;
			copy_powerDef(&result->total_power,total_power);
			copy_powerDef(&result->total_power_without_routing,total_power_without_routing);
			//copy_and_div_powerDef(&result->total_routing_power,total_address_routing_power,FUDGEFACTOR);
			copy_powerDef(&result->total_routing_power,total_address_routing_power);
			copy_powerDef(&result->total_power_allbanks,total_power_allbanks);

			/*result->subbank_address_routing_delay = subbank_address_routing_delay / FUDGEFACTOR;
			copy_and_div_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power,FUDGEFACTOR);
			result->cycle_time = cycle_time / FUDGEFACTOR;
			result->access_time = access_time / FUDGEFACTOR;
			result->best_Ndwl = Ndwl;
			result->best_Ndbl = Ndbl;
			result->best_Nspd = Nspd;
			result->best_Ntwl = Ntwl;
			result->best_Ntbl = Ntbl;
			result->best_Ntspd = Ntspd;
			result->decoder_delay_data = decoder_data / FUDGEFACTOR;
			copy_and_div_powerDef(&result->decoder_power_data,decoder_data_power,FUDGEFACTOR);
			result->decoder_delay_tag = decoder_tag / FUDGEFACTOR;
			copy_and_div_powerDef(&result->decoder_power_tag,decoder_tag_power,FUDGEFACTOR);
			result->dec_tag_driver = decoder_tag_driver / FUDGEFACTOR;
			result->dec_tag_3to8 = decoder_tag_3to8 / FUDGEFACTOR;
			result->dec_tag_inv = decoder_tag_inv / FUDGEFACTOR;
			result->dec_data_driver = decoder_data_driver / FUDGEFACTOR;
			result->dec_data_3to8 = decoder_data_3to8 / FUDGEFACTOR;
			result->dec_data_inv = decoder_data_inv / FUDGEFACTOR;
			result->wordline_delay_data = wordline_data / FUDGEFACTOR;
			copy_and_div_powerDef(&result->wordline_power_data,wordline_data_power,FUDGEFACTOR);
			result->wordline_delay_tag = wordline_tag / FUDGEFACTOR;
			copy_and_div_powerDef(&result->wordline_power_tag,wordline_tag_power,FUDGEFACTOR);
			result->bitline_delay_data = bitline_data / FUDGEFACTOR;
			copy_and_div_powerDef(&result->bitline_power_data,bitline_data_power,FUDGEFACTOR);
			result->bitline_delay_tag = bitline_tag / FUDGEFACTOR;
			copy_and_div_powerDef(&result->bitline_power_tag,bitline_tag_power,FUDGEFACTOR);
			result->sense_amp_delay_data = sense_amp_data / FUDGEFACTOR;
			copy_and_div_powerDef(&result->sense_amp_power_data,sense_amp_data_power,FUDGEFACTOR);
			result->sense_amp_delay_tag = sense_amp_tag / FUDGEFACTOR;
			copy_and_div_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power,FUDGEFACTOR);
			result->total_out_driver_delay_data = total_out_driver / FUDGEFACTOR;
			copy_and_div_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
			result->compare_part_delay = compare_tag / FUDGEFACTOR;
			copy_and_div_powerDef(&result->compare_part_power,compare_tag_power,FUDGEFACTOR);
			result->drive_mux_delay = mux_driver / FUDGEFACTOR;
			copy_and_div_powerDef(&result->drive_mux_power,mux_driver_power,FUDGEFACTOR);
			result->selb_delay = selb / FUDGEFACTOR;
			copy_and_div_powerDef(&result->selb_power,selb_power,FUDGEFACTOR);
			result->drive_valid_delay = valid_driver / FUDGEFACTOR;
			copy_and_div_powerDef(&result->drive_valid_power,valid_driver_power,FUDGEFACTOR);
			result->data_output_delay = data_output / FUDGEFACTOR;
			copy_and_div_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);
			result->precharge_delay = precharge_del / FUDGEFACTOR;*/

			result->subbank_address_routing_delay = subbank_address_routing_delay;
			copy_powerDef(&result->subbank_address_routing_power,subbank_address_routing_power);
			result->cycle_time = cycle_time;
			result->access_time = access_time;
			result->best_Ndwl = Ndwl;
			result->best_Ndbl = Ndbl;
			result->best_Nspd = Nspd;
			result->best_Ntwl = Ntwl;
			result->best_Ntbl = Ntbl;
			result->best_Ntspd = Ntspd;
			result->decoder_delay_data = decoder_data;
			copy_powerDef(&result->decoder_power_data,decoder_data_power);
			result->decoder_delay_tag = decoder_tag;
			copy_powerDef(&result->decoder_power_tag,decoder_tag_power);
			result->dec_tag_driver = decoder_tag_driver;
			result->dec_tag_3to8 = decoder_tag_3to8;
			result->dec_tag_inv = decoder_tag_inv;
			result->dec_data_driver = decoder_data_driver;
			result->dec_data_3to8 = decoder_data_3to8;
			result->dec_data_inv = decoder_data_inv;
			result->wordline_delay_data = wordline_data;
			copy_powerDef(&result->wordline_power_data,wordline_data_power);
			result->wordline_delay_tag = wordline_tag;
			copy_powerDef(&result->wordline_power_tag,wordline_tag_power);
			result->bitline_delay_data = bitline_data;
			copy_powerDef(&result->bitline_power_data,bitline_data_power);
			result->bitline_delay_tag = bitline_tag;
			copy_powerDef(&result->bitline_power_tag,bitline_tag_power);
			result->sense_amp_delay_data = sense_amp_data;
			copy_powerDef(&result->sense_amp_power_data,sense_amp_data_power);
			result->sense_amp_delay_tag = sense_amp_tag;
			copy_powerDef(&result->sense_amp_power_tag,sense_amp_tag_power);
			result->total_out_driver_delay_data = total_out_driver;
			copy_powerDef(&result->total_out_driver_power_data,total_out_driver_power);
			result->compare_part_delay = compare_tag;
			copy_powerDef(&result->compare_part_power,compare_tag_power);
			result->drive_mux_delay = mux_driver;
			copy_powerDef(&result->drive_mux_power,mux_driver_power);
			result->selb_delay = selb;
			copy_powerDef(&result->selb_power,selb_power);
			result->drive_valid_delay = valid_driver;
			copy_powerDef(&result->drive_valid_power,valid_driver_power);
			result->data_output_delay = data_output;
			copy_powerDef(&result->data_output_power,data_output_power);
			result->precharge_delay = precharge_del;
			result->data_nor_inputs = data_nor_inputs;
			result->tag_nor_inputs = tag_nor_inputs;

			area_subbanked (ADDRESS_BITS, BITOUT,
					parameters->num_readwrite_ports,
					parameters->num_read_ports,
					parameters->num_write_ports, Ndbl,
					Ndwl, Nspd, Ntbl, Ntwl, Ntspd,
					*NSubbanks, parameters,
					arearesult_subbanked, arearesult);

			//v4.1: No longer using calculate_area function as area has already been
			//computed for the given tech node
			/*arearesult->efficiency =
			(*NSubbanks) * (area_all_dataramcells +
					area_all_tagramcells) * 100 /
			(calculate_area (*arearesult_subbanked,
						parameters->fudgefactor) /
				100000000.0);*/

// 			arearesult->efficiency =
// 				(*NSubbanks) * (area_all_dataramcells +
// 				area_all_tagramcells) * 100 /
// 				(arearesult_subbanked->height * arearesult_subbanked->width /100000000.0);

			arearesult->aspect_ratio_total =
				(arearesult_subbanked->height /
					arearesult_subbanked->width);
			arearesult->aspect_ratio_total =
				(arearesult->aspect_ratio_total >
					1.0) ? (arearesult->aspect_ratio_total) : 1.0 /
				(arearesult->aspect_ratio_total);
// 			arearesult->max_efficiency = max_efficiency;
// 			arearesult->max_aspect_ratio_total =
// 				max_aspect_ratio_total;

/*			if (result->max_access_time < access_time)
				result->max_access_time = access_time;*/
/*			if (result->max_power < 
				((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
				(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0)
			{
				result->max_power = ((total_power.readOp.dynamic/(cycle_time))+total_power.readOp.leakage+
										(total_power.writeOp.dynamic/(cycle_time))+total_power.writeOp.leakage)/ 2.0;
			} //read and write dynamic energy components changed to power components by Shyam. Earlier */
			//read and write dynamic energies were being added to leakage power components and getting swamped
/*			if (arearesult_temp.max_efficiency < efficiency)
			{
				arearesult_temp.max_efficiency = efficiency;
				max_efficiency = efficiency;
			}*/
/*			if (min_efficiency > efficiency)
			{
				min_efficiency = efficiency;
			}*/
/*			if (max_aspect_ratio_total < aspect_ratio_total_temp)
			{
				max_aspect_ratio_total = aspect_ratio_total_temp;
			}*/
		}
	}
}











/* from io.c */
int Cacti4_2::input_data(int argc,char *argv[]) const
{
   int C,B,A,ERP,EWP,RWP,NSER, NSubbanks, fully_assoc;
   double tech;
   double logbanks, assoc;
   double logbanksfloor, assocfloor;
   int bits_output = 64;

   /*if ((argc!=6) && (argc!=9)) {
      printf("Cmd-line parameters: C B A TECH NSubbanks\n");
      printf("                 OR: C B A TECH RWP ERP EWP NSubbanks\n");
      exit(0);
   }*/

   if ((argc!=6) && (argc!=9)&& (argc!=15)) {
      printf("Cmd-line parameters: C B A TECH NSubbanks\n");
      printf("                 OR: C B A TECH RWP ERP EWP NSubbanks\n");
      exit(1);
   }

   B = atoi(argv[2]);
   if ((B < 1)) {
       printf("Block size must >=1\n");
       exit(1);
   }

    if (argc==9)
     {
		if ((B*8 < bits_output)) {
			printf("Block size must be at least %d\n", bits_output/8);
			exit(1);
		}
 
		tech = atof(argv[4]);
		if ((tech <= 0)) {
			printf("Feature size must be > 0\n");
			exit(1);
		 }
		if ((tech > 0.8)) {
			printf("Feature size must be <= 0.80 (um)\n");
			 exit(1);
		 }

       RWP = atoi(argv[5]);
       ERP = atoi(argv[6]);
       EWP = atoi(argv[7]);
       NSER = 0;

       if ((RWP < 0) || (EWP < 0) || (ERP < 0)) {
		 printf("Ports must >=0\n");
		 exit(1);
       }
       if (RWP > 2) {
		 printf("Maximum of 2 read/write ports\n");
		 exit(1);
       }
       if ((RWP+ERP+EWP) < 1) {
       	 printf("Must have at least one port\n");
       	 exit(1);
       }

       NSubbanks = atoi(argv[8]);

       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         exit(1);
       }

       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);
      
       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         exit(1);
       }

     }
   
   else if(argc==6)
     {

		if ((B*8 < bits_output)) {
			printf("Block size must be at least %d\n", bits_output/8);
			exit(1);
		}
 
		tech = atof(argv[4]);
		if ((tech <= 0)) {
			printf("Feature size must be > 0\n");
			exit(1);
		 }

		if ((tech > 0.8)) {
			printf("Feature size must be <= 0.80 (um)\n");
			exit(1);
		 }

       RWP=1;
       ERP=0;
       EWP=0;
       NSER=0;

       NSubbanks = atoi(argv[5]);
       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         exit(1); 
       }
       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);

       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         exit(1);
       }

     }
	 else 
     {
       tech = atof(argv[9]);
	   NSubbanks = atoi(argv[8]);
	   if ((tech <= 0)) {
			printf("Feature size must be > 0\n");
			exit(1);
		 }

	   if ((tech > 0.8)) {
			printf("Feature size must be <= 0.80 (um)\n");
			exit(1);
		}
     }

   C = atoi(argv[1])/((int) (NSubbanks));
   if (atoi(argv[1]) < 64) {
       printf("Cache size must be greater than 32!\n");
       exit(1);
   }
 
   if ((strcmp(argv[3],"FA") == 0) || (argv[3][0] == '0'))
     {
       A=C/B;
       fully_assoc = 1;
     }
   else
     {
       if (strcmp(argv[3],"DM") == 0)
         {
           A=1;
           fully_assoc = 0;
         }
       else
         {
           fully_assoc = 0;
           A = atoi(argv[3]);
		   if ((A < 0)||(A > 16)) {
             printf("Associativity must be  1,2,4,8,16 or 0(fully associative)\n");
             exit(1);
           }
           assoc = logtwo((double)(A));
           assocfloor = floor(assoc);

           if(assoc > assocfloor){
             printf("Associativity should be a power of 2\n");
             exit(1);
           }

           }
     }

   if (!fully_assoc && C/(B*A) < 1) {
     printf("Number of sets is less than 1:\n  Need to either increase cache size, or decrease associativity or block size\n  (or use fully associative cache)\n");
       exit(1);
   }
 
   return(OK);
}

void Cacti4_2::output_time_components(result_type *result,parameter_type *parameters) const
{
int A;
       printf(" address routing delay (ns): %g\n",result->subbank_address_routing_delay/1e-9);
       //printf(" address routing power (nJ): %g\n",result->subbank_address_routing_power*1e9);
 
  A=parameters->tag_associativity;
   if (!parameters->fully_assoc)
     {
       printf(" decode_data (ns): %g\n",result->decoder_delay_data/1e-9);
       printf("     dyn. energy (nJ): %g\n",result->decoder_power_data.readOp.dynamic*1e9);
	   printf("     leak. power (mW): %g\n",result->decoder_power_data.readOp.leakage*1e3);
     }
   else
     {
       printf(" tag_comparison (ns): %g\n",result->decoder_delay_data/1e-9);
       printf("     dyn. energy (nJ): %g\n",result->decoder_power_data.readOp.dynamic*1e9);
	   printf("     leak. power (mW): %g\n",result->decoder_power_data.readOp.leakage*1e3);
     }
   printf(" wordline and bitline data (ns): %g\n",(result->wordline_delay_data+result->bitline_delay_data)/1e-9);
   printf("  dyn. wordline energy (nJ): %g\n",(result->wordline_power_data.readOp.dynamic)*1e9);
   printf("  leak. wordline power (mW): %g\n",(result->wordline_power_data.readOp.leakage)*1e3);
   printf("  dyn. read data bitline energy (nJ): %g\n",(result->bitline_power_data.readOp.dynamic)*1e9);
   printf("  dyn. write data bitline energy (nJ): %g\n",(result->bitline_power_data.writeOp.dynamic)*1e9);
   printf("  leak. data bitline power (mW): %g\n",(result->bitline_power_data.writeOp.leakage)*1e3);

   printf(" sense_amp_data (ns): %g\n",result->sense_amp_delay_data/1e-9);
   printf("     dyn. energy (nJ): %g\n",result->sense_amp_power_data.readOp.dynamic*1e9);
   printf("     leak. power (mW): %g\n",result->sense_amp_power_data.readOp.leakage*1e3);
   if (!parameters->fully_assoc)
     {
       printf(" decode_tag (ns): %g\n",result->decoder_delay_tag/1e-9);
       printf("     dyn. energy (nJ): %g\n",result->decoder_power_tag.readOp.dynamic*1e9);
	   printf("     leak. power (mW): %g\n",result->decoder_power_tag.readOp.leakage*1e3);

        printf(" wordline and bitline tag (ns): %g\n",(result->wordline_delay_tag+result->bitline_delay_tag)/1e-9);
		printf("  dyn. wordline energy (nJ): %g\n",(result->wordline_power_tag.readOp.dynamic)*1e9);
		printf("  leak. wordline power (mW): %g\n",(result->wordline_power_tag.readOp.leakage)*1e3);
		printf("  dyn. read data bitline energy (nJ): %g\n",(result->bitline_power_tag.readOp.dynamic)*1e9);
		printf("  dyn. write data bitline energy (nJ): %g\n",(result->bitline_power_tag.writeOp.dynamic)*1e9);
		printf("  leak. data bitline power (mW): %g\n",(result->bitline_power_tag.writeOp.leakage)*1e3);

       printf(" sense_amp_tag (ns): %g\n",result->sense_amp_delay_tag/1e-9);
       printf("  dyn. read energy (nJ): %g\n",result->sense_amp_power_tag.readOp.dynamic*1e9);
	   printf("  leak. read power (mW): %g\n",result->sense_amp_power_tag.readOp.leakage*1e3);
       printf(" compare (ns): %g\n",result->compare_part_delay/1e-9);
       printf("  dyn. read energy (nJ): %g\n",result->compare_part_power.readOp.dynamic*1e9);
	   printf("  leak. read power (mW): %g\n",result->compare_part_power.readOp.leakage*1e3);
       if (A == 1)
         {
           printf(" valid signal driver (ns): %g\n",result->drive_valid_delay/1e-9);
           printf("     dyn. read energy (nJ): %g\n",result->drive_valid_power.readOp.dynamic*1e9);
		   printf("    leak. read power (mW): %g\n",result->drive_valid_power.readOp.leakage*1e3);
         }
       else {
         printf(" mux driver (ns): %g\n",result->drive_mux_delay/1e-9);
         printf("  dyn. read energy (nJ): %g\n",result->drive_mux_power.readOp.dynamic*1e9);
		 printf("  leak. read power (mW): %g\n",result->drive_mux_power.readOp.leakage*1e3);
         printf(" sel inverter (ns): %g\n",result->selb_delay/1e-9);
         printf("  dyn. read energy (nJ): %g\n",result->selb_power.readOp.dynamic*1e9);
		 printf("  leak. read power (mW): %g\n",result->selb_power.readOp.leakage*1e3);
       }
     }
   printf(" data output driver (ns): %g\n",result->data_output_delay/1e-9);
   printf("	   dyn. read energy (nJ): %g\n",result->data_output_power.readOp.dynamic*1e9);
   printf("	   leak. read power (mW): %g\n",result->data_output_power.readOp.leakage*1e3);
   printf(" total_out_driver (ns): %g\n", result->total_out_driver_delay_data/1e-9);
   printf("	 dyn. read energy (nJ): %g\n", result->total_out_driver_power_data.readOp.dynamic*1e9);
   printf("	 leak. read power (mW): %g\n", result->total_out_driver_power_data.readOp.leakage*1e3);

   printf(" total data path (without output driver) (ns): %g\n",result->subbank_address_routing_delay/1e-9+result->decoder_delay_data/1e-9+result->wordline_delay_data/1e-9+result->bitline_delay_data/1e-9+result->sense_amp_delay_data/1e-9);
   if (!parameters->fully_assoc)
     {
       if (A==1)
         printf(" total tag path is dm (ns): %g\n", result->subbank_address_routing_delay/1e-9+result->decoder_delay_tag/1e-9+result->wordline_delay_tag/1e-9+result->bitline_delay_tag/1e-9+result->sense_amp_delay_tag/1e-9+result->compare_part_delay/1e-9);
       else
         printf(" total tag path is set assoc (ns): %g\n", result->subbank_address_routing_delay/1e-9+result->decoder_delay_tag/1e-9+result->wordline_delay_tag/1e-9+result->bitline_delay_tag/1e-9+result->sense_amp_delay_tag/1e-9+result->compare_part_delay/1e-9+result->drive_mux_delay/1e-9+result->selb_delay/1e-9);
     }
}

void Cacti4_2::output_area_components(arearesult_type *arearesult, parameter_type *parameters) const
{
    printf("\nArea Components:\n\n");

	//v4.1: No longer using calculate_area function as area has already been
	//computed for the given tech node. 
/*
    printf("Aspect Ratio Data height/width: %f\n", aspect_ratio_data);
    printf("Aspect Ratio Tag height/width: %f\n", aspect_ratio_tag);
    printf("Aspect Ratio Subbank height/width: %f\n", aspect_ratio_subbank);
    printf("Aspect Ratio Total height/width: %f\n\n", aspect_ratio_total);
*/
    printf("Aspect Ratio Total height/width: %f\n\n", arearesult->aspect_ratio_total);

	printf("Data array (mm^2): %f\n",arearesult->dataarray_area.scaled_area);
	printf("Data predecode (mm^2): %f\n",arearesult->datapredecode_area.scaled_area);
	printf("Data colmux predecode (mm^2): %f\n",arearesult->datacolmuxpredecode_area.scaled_area);
	printf("Data colmux post decode (mm^2): %f\n",arearesult->datacolmuxpostdecode_area.scaled_area);
	printf("Data write signal (mm^2): %f\n",arearesult->datawritesig_area.scaled_area);

	printf("\nTag array (mm^2): %f\n",arearesult->tagarray_area.scaled_area);
	printf("Tag predecode (mm^2): %f\n",arearesult->tagpredecode_area.scaled_area);
	printf("Tag colmux predecode (mm^2): %f\n",arearesult->tagcolmuxpredecode_area.scaled_area);
	printf("Tag colmux post decode (mm^2): %f\n",arearesult->tagcolmuxpostdecode_area.scaled_area);
	printf("Tag output driver decode (mm^2): %f\n",arearesult->tagoutdrvdecode_area.scaled_area);
	printf("Tag output driver enable signals (mm^2): %f\n",arearesult->tagoutdrvsig_area.scaled_area);

	printf("\nPercentage of data ramcells alone of total area: %f %%\n", arearesult->perc_data);
    printf("Percentage of tag ramcells alone of total area: %f %%\n",arearesult->perc_tag);
    printf("Percentage of total control/routing alone of total area: %f %%\n",arearesult->perc_cont);
	printf("\nSubbank Efficiency : %f\n", arearesult->sub_eff);
	printf("Total Efficiency : %f\n",arearesult->total_eff);
	printf("\nTotal area One bank (mm^2): %f\n",arearesult->totalarea);
	printf("Total area subbanked (mm^2): %f\n",arearesult->subbankarea);

}


void Cacti4_2::output_data(result_type *result,arearesult_type *arearesult, parameter_type *parameters) const
{
   double datapath,tagpath;

   FILE *stream;

   stream=fopen("cache_params.aux", "w");

   datapath = result->subbank_address_routing_delay+result->decoder_delay_data+result->wordline_delay_data+result->bitline_delay_data+result->sense_amp_delay_data+result->total_out_driver_delay_data+result->data_output_delay;
   if (parameters->tag_associativity == 1) {
         tagpath = result->subbank_address_routing_delay+result->decoder_delay_tag+result->wordline_delay_tag+result->bitline_delay_tag+result->sense_amp_delay_tag+result->compare_part_delay+result->drive_valid_delay;
   } else {
         tagpath = result->subbank_address_routing_delay+result->decoder_delay_tag+result->wordline_delay_tag+result->bitline_delay_tag+result->sense_amp_delay_tag+result->compare_part_delay+
			 //result->drive_mux_delay+
			 result->selb_delay+result->data_output_delay+result->total_out_driver_delay_data;
   }

   if (stream){
	   fprintf(stream, "#define PARAMNDWL %d\n#define PARAMNDBL %d\n#define PARAMNSPD %f\n#define PARAMNTWL %d\n#define PARAMNTBL %d\n#define PARAMNTSPD %d\n#define PARAMSENSESCALE %f\n#define PARAMGS %d\n#define PARAMDNOR %d\n#define PARAMTNOR %d\n#define PARAMRPORTS %d\n#define PARAMWPORTS %d\n#define PARAMRWPORTS %d\n#define PARAMMUXOVER %d\n", result->best_Ndwl, result->best_Ndbl, result->best_Nspd, result->best_Ntwl, result->best_Ntbl, result->best_Ntspd, result->senseext_scale, (result->senseext_scale==1.0), result->data_nor_inputs, result->tag_nor_inputs, parameters->num_read_ports, parameters->num_write_ports, parameters->num_readwrite_ports, result->best_muxover);
   }
 
    fclose(stream);
#  if OUTPUTTYPE == LONG
      printf("\n---------- CACTI version 4.1 ----------\n");
      printf("\nCache Parameters:\n");
	  printf("  Number of banks: %d\n",(int)result->subbanks);
      printf("  Total Cache Size: %d\n",(int) (parameters->cache_size));
      printf("  Size in bytes of a bank: %d\n",parameters->cache_size / (int)result->subbanks);
      printf("  Number of sets per bank: %d\n",parameters->number_of_sets);
      if (parameters->fully_assoc)
        printf("  Associativity: fully associative\n");
      else
        {
          if (parameters->tag_associativity==1)
            printf("  Associativity: direct mapped\n");
          else
            printf("  Associativity: %d\n",parameters->tag_associativity);
        }
      printf("  Block Size (bytes): %d\n",parameters->block_size);
      printf("  Read/Write Ports: %d\n",parameters->num_readwrite_ports);
      printf("  Read Ports: %d\n",parameters->num_read_ports);
      printf("  Write Ports: %d\n",parameters->num_write_ports);
      printf("  Technology Size: %2.2fum\n", parameters->tech_size);
      printf("  Vdd: %2.1fV\n", parameters->VddPow);

      printf("\nAccess Time (ns): %g\n",result->access_time*1e9);
      printf("Cycle Time (ns):  %g\n",result->cycle_time*1e9);
      //if (parameters->fully_assoc)
        //{
			printf("Total dynamic Read Power at max. freq. (W): %g\n",result->total_power_allbanks.readOp.dynamic/result->cycle_time);
			printf("Total leakage Read/Write Power all Banks (mW): %g\n",result->total_power_allbanks.readOp.leakage*1e3);
			printf("Total dynamic Read Energy all Banks (nJ): %g\n",result->total_power_allbanks.readOp.dynamic*1e9);
			printf("Total dynamic Write Energy all Banks (nJ): %g\n",result->total_power_allbanks.writeOp.dynamic*1e9);
			printf("Total dynamic Read Energy Without Routing (nJ): %g\n",result->total_power_without_routing.readOp.dynamic*1e9);
			printf("Total dynamic Write Energy Without Routing (nJ): %g\n",result->total_power_without_routing.writeOp.dynamic*1e9);
			printf("Total dynamic Routing Energy (nJ): %g\n",result->total_routing_power.readOp.dynamic*1e9);
			printf("Total leakage Read/Write Power Without Routing (mW): %g\n",result->total_power_without_routing.readOp.leakage*1e3);
			//printf("Total leakage Write Power all Banks (mW): %g\n",result->total_power_allbanks.writeOp.leakage*1e3);
			//printf("Total leakage Write Power Without Routing (mW): %g\n",result->total_power_without_routing.writeOp.leakage*1e3);
			printf("Total leakage Read/Write Routing Power (mW): %g\n",result->total_routing_power.readOp.leakage*1e3);

          //printf("Maximum Bank Power (nJ):  %g\n",(result->subbank_address_routing_power+result->decoder_power_data+result->wordline_power_data+result->bitline_power_data+result->sense_amp_power_data+result->data_output_power+result->total_out_driver_power_data)*1e9);
          //      printf("Power (W) - 500MHz:  %g\n",(result->decoder_power_data+result->wordline_power_data+result->bitline_power_data+result->sense_amp_power_data+result->data_output_power)*500*1e6);
        //}
      /*else
        {
	  printf("Total dynamic Read Power at max. freq. (W): %g\n",result->total_power_allbanks.readOp.dynamic/result->cycle_time);
	  printf("Total dynamic Read Energy all Banks (nJ): %g\n",result->total_power_allbanks.readOp.dynamic*1e9);
	  printf("Total leakage Read Power all Banks (mW): %g\n",result->total_power_allbanks.readOp.leakage*1e3);
	  printf("Total dynamic Write Energy all Banks (nJ): %g\n",result->total_power_allbanks.writeOp.dynamic*1e9);
	  printf("Total leakage Write Power all Banks (mW): %g\n",result->total_power_allbanks.writeOp.leakage*1e3);
	  printf("Total dynamic Read Energy Without Routing (nJ): %g\n",result->total_power_without_routing.readOp.dynamic*1e9);
	  printf("Total leakage Read Power Without Routing (mW): %g\n",result->total_power_without_routing.readOp.leakage*1e3);
	  printf("Total dynamic Write Energy Without Routing (nJ): %g\n",result->total_power_without_routing.writeOp.dynamic*1e9);
	  printf("Total leakage Write Power Without Routing (mW): %g\n",result->total_power_without_routing.writeOp.leakage*1e3);
	  printf("Total dynamic Routing Energy (nJ): %g\n",result->total_routing_power.readOp.dynamic*1e9);
	  printf("Total leakage Routing Power (mW): %g\n",result->total_routing_power.readOp.leakage*1e3);*/
          //printf("Maximum Bank Power (nJ):  %g\n",(result->subbank_address_routing_power+result->decoder_power_data+result->wordline_power_data+result->bitline_power_data+result->sense_amp_power_data+result->total_out_driver_power_data+result->decoder_power_tag+result->wordline_power_tag+result->bitline_power_tag+result->sense_amp_power_tag+result->compare_part_power+result->drive_valid_power+result->drive_mux_power+result->selb_power+result->data_output_power)*1e9);
          //      printf("Power (W) - 500MHz:  %g\n",(result->decoder_power_data+result->wordline_power_data+result->bitline_power_data+result->sense_amp_power_data+result->total_out_driver_power_data+result->decoder_power_tag+result->wordline_power_tag+result->bitline_power_tag+result->sense_amp_power_tag+result->compare_part_power+result->drive_valid_power+result->drive_mux_power+result->selb_power+result->data_output_power)*500*1e6);
        //}
      printf("\nBest Ndwl (L1): %d\n",result->best_Ndwl);
      printf("Best Ndbl (L1): %d\n",result->best_Ndbl);
      printf("Best Nspd (L1): %f\n",result->best_Nspd);
      printf("Best Ntwl (L1): %d\n",result->best_Ntwl);
      printf("Best Ntbl (L1): %d\n",result->best_Ntbl);
      printf("Best Ntspd (L1): %d\n",result->best_Ntspd);
      //printf("Nor inputs (data): %d\n",result->data_nor_inputs);
      //printf("Nor inputs (tag): %d\n",result->tag_nor_inputs);

	  output_area_components(arearesult,parameters);
      printf("\nTime Components:\n");
     
      printf(" data side (with Output driver) (ns): %g\n",datapath/1e-9);
      if (!parameters->fully_assoc)
        printf(" tag side (with Output driver) (ns): %g\n",(tagpath)/1e-9);
      output_time_components(result,parameters);

#  else
      printf("%d %d %d  %d %d %d %d %d %d  %e %e %e %e  %e %e %e %e  %e %e %e %e  %e %e %e %e  %e %e\n",
               parameters->cache_size,
               parameters->block_size,
               parameters->associativity,
               result->best_Ndwl,
               result->best_Ndbl,
               result->best_Nspd,
               result->best_Ntwl,
               result->best_Ntbl,
               result->best_Ntspd,
               result->access_time,
               result->cycle_time,
               datapath,
               tagpath,
               result->decoder_delay_data,
               result->wordline_delay_data,
               result->bitline_delay_data,
               result->sense_amp_delay_data,
               result->decoder_delay_tag,
               result->wordline_delay_tag,
               result->bitline_delay_tag,
               result->sense_amp_delay_tag,
               result->compare_part_delay,
               result->drive_mux_delay,
               result->selb_delay,
               result->drive_valid_delay,
               result->data_output_delay,
               result->precharge_delay);



# endif

}


Cacti4_2::total_result_type Cacti4_2::cacti_interface(
		int cache_size,
		int line_size,
		int associativity,
		int rw_ports,
		int excl_read_ports,
		int excl_write_ports,
		int single_ended_read_ports,
		int banks,
		double tech_node,
		int output_width,
		int specific_tag,
		int tag_width,
		int access_mode,
		int pure_sram,
		double *VddPow)
{
   int C,B,A,ERP,EWP,RWP,NSER;
   double tech;
   double logbanks, assoc;
   double logbanksfloor, assocfloor;
   int seq_access = 0;
   int fast_access = 0;
   int bits_output = output_width;
   int nr_args = 9;
   double NSubbanks = (double)banks;

   double ratioofbankstoports;

   //extern int force_tag, force_tag_size;

   total_result_type endresult;

   result_type result;
   arearesult_type arearesult;
   area_type arearesult_subbanked;
   parameter_type parameters; 

   /* input parameters:
         C B A ERP EWP */

   /*dt: make sure we're using some simple leakage reduction */
   dualVt = FALSE;

    force_tag = 0;

   if(specific_tag) {
	   force_tag = 1;
	   force_tag_size = tag_width;
   }
   switch (access_mode){
	   case 0:
		   seq_access = fast_access = FALSE;
		   break;
	   case 1:
		   seq_access = TRUE;
		   fast_access = FALSE;
		   break;
	   case 2:
		   seq_access = FALSE;
		   fast_access = TRUE;
		   break;
   }

   B = line_size;
   if ((B < 1)) {
       printf("Block size must >=1\n");
       return endresult;
	   //exit(1);
   }

   if ((B*8 < bits_output)) {
       printf("Block size must be at least %d\n", bits_output/8);
       return endresult;
	   //exit(1);
   }
   

   tech = tech_node;
   if ((tech <= 0)) {
       printf("Feature size must be > 0\n");
       return endresult;
	   //exit(1);
   }
   if ((tech > 0.8)) {
       printf("Feature size must be <= 0.80 (um)\n");
       return endresult;
	   //exit(1);
   }

   if (nr_args ==9)
     {
       RWP = rw_ports;
       ERP = excl_read_ports;
       EWP = excl_write_ports;
       NSER = single_ended_read_ports;


       if ((RWP < 0) || (EWP < 0) || (ERP < 0)) {
			printf("Ports must >=0\n");
			return endresult;
			//exit(1);
       }
       if (RWP > 2) {
			printf("Maximum of 2 read/write ports\n");
			return endresult;
			//exit(1);
       }
       if ((RWP+ERP+EWP) < 1) {
       	 printf("Must have at least one port\n");
       	 return endresult;
		 //exit(1);
       }

       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);
      
       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
   else if (nr_args==10)
     {
       RWP = rw_ports;
       ERP = excl_read_ports;
       EWP = excl_write_ports;
       NSER = single_ended_read_ports;

	   seq_access = 1;

       if ((RWP < 0) || (EWP < 0) || (ERP < 0)) {
			printf("Ports must >=0\n");
			return endresult;
			//exit(1);
       }
       if (RWP > 2) {
			printf("Maximum of 2 read/write ports\n");
			return endresult;
			//exit(1);
       }
       if ((RWP+ERP+EWP) < 1) {
       	 printf("Must have at least one port\n");
       	 return endresult;
		 //exit(1);
       }
       

       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);
      
       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
   else if(nr_args==6)
     {
       RWP=1;
       ERP=0;
       EWP=0;
       NSER=0;

       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult; 
		 //exit(1);
       }
       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);

       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
	 else if(nr_args==8)
     {
       RWP=1;
       ERP=0;
       EWP=0;
       NSER=0;

	   bits_output = output_width;
	   seq_access = 1;

       NSubbanks = banks;
       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }
       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);

       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
 
   C = cache_size/((int) (NSubbanks));
   if ((C < 64)) {
       printf("Cache size must >=64\n");
       return endresult;
	   //exit(1);
   }
 
   if (associativity == 0)
     {
       A=C/B;
       parameters.fully_assoc = 1;
     }
   else
     {
       if (associativity == 1)
         {
           A=1;
           parameters.fully_assoc = 0;
         }
       else
         {
           parameters.fully_assoc = 0;
           A = associativity;
           if ((A < 1)) {
             printf("Associativity must >= 1\n");
             return endresult;
			 //exit(1);
           }
           assoc = logtwo((double)(A));
		   assocfloor = floor(assoc);
		   
           if(assoc > assocfloor){
             printf("Associativity should be a power of 2\n");
             return endresult;
			 //exit(1);
           }

           if ((A > 32)) {
             printf("Associativity must <= 32\n or try FA (fully associative)\n");
             return endresult;
			 //exit(1);
           }
         }
     }

   if (C/(B*A)<=1 && !parameters.fully_assoc) {
     printf("Number of sets is too small:\n  Need to either increase cache size, or decrease associativity or block size\n  (or use fully associative cache)\n");
     return endresult;
	//exit(1); 
   }
 
   parameters.cache_size = C;
   parameters.block_size = B;

   parameters.nr_bits_out = bits_output;
   /*dt: testing sequential access mode*/
   if(seq_access) {
	parameters.tag_associativity = A;
	parameters.data_associativity = 1;
	parameters.sequential_access = 1;
   }
   else {
	parameters.tag_associativity = parameters.data_associativity = A;
	parameters.sequential_access = 0;
   }
   if(fast_access) {
	   parameters.fast_access = 1;
   }
   else {
	   parameters.fast_access = 0;
   }
   parameters.num_readwrite_ports = RWP;
   parameters.num_read_ports = ERP;
   parameters.num_write_ports = EWP;
   parameters.num_single_ended_read_ports =NSER;
   parameters.number_of_sets = C/(B*A);
   parameters.fudgefactor = .8/tech;   
   parameters.tech_size=(double) tech;
   parameters.pure_sram = pure_sram;

   //If multiple banks and multiple ports are specified, then if number of banks/total number 
   //of ports > 1 then assume that the multiple ports are implemented via the multiple banks.
   //Also assume that each bank has only 1 RWP port. There are some problems with this logic that 
   //will be fixed in v5.0
   ratioofbankstoports = NSubbanks/(RWP + ERP + EWP);
   if(ratioofbankstoports >= 1.0){
	   //We assume that each bank has 1 RWP port.
	   parameters.num_readwrite_ports = 1;
       parameters.num_read_ports = 0;
       parameters.num_write_ports = 0;
       parameters.num_single_ended_read_ports = 0;
	}

   if (parameters.number_of_sets < 1) {
      printf("Less than one set...\n");
      return endresult;
	  //exit(1);
   }   
 
   init_tech_params_default_process();//v4.1: First initialize all tech variables
   //to 0.8 micron values. init_tech_params function below then reinitializes tech variables to
   //given process values
   init_tech_params(parameters.tech_size);
   calculate_time(&result,&arearesult,&arearesult_subbanked,&parameters,&NSubbanks);

   //v4.1: No longer using calculate_area function as area has already been
   //computed for the given tech node

   /*arearesult.dataarray_area.scaled_area = calculate_area(arearesult.dataarray_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datapredecode_area.scaled_area = calculate_area(arearesult.datapredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpredecode_area.scaled_area = calculate_area(arearesult.datacolmuxpredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpostdecode_area.scaled_area = calculate_area(arearesult.datacolmuxpostdecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datawritesig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)*calculate_area(arearesult.datawritesig_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;

   arearesult.tagarray_area.scaled_area = calculate_area(arearesult.tagarray_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagpredecode_area.scaled_area = calculate_area(arearesult.tagpredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpredecode_area.scaled_area = calculate_area(arearesult.tagcolmuxpredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpostdecode_area.scaled_area = calculate_area(arearesult.tagcolmuxpostdecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvdecode_area.scaled_area = calculate_area(arearesult.tagoutdrvdecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvsig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)*
											   calculate_area(arearesult.tagoutdrvsig_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;

   arearesult.perc_data = 100*area_all_dataramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_tag  = 100*area_all_tagramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_cont = 100*(arearesult.totalarea*CONVERT_TO_MMSQUARE-area_all_dataramcells-area_all_tagramcells)/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.sub_eff   = (area_all_dataramcells+area_all_tagramcells)*100/(arearesult.totalarea/100000000.0);
   arearesult.total_eff = (NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/
							(calculate_area(arearesult_subbanked,parameters.fudgefactor)*CONVERT_TO_MMSQUARE);
   arearesult.totalarea *= CONVERT_TO_MMSQUARE;
   arearesult.subbankarea = calculate_area(arearesult_subbanked,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;*/
   
   arearesult.dataarray_area.scaled_area = arearesult.dataarray_area.height * arearesult.dataarray_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datapredecode_area.scaled_area = arearesult.datapredecode_area.height * arearesult.datapredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpredecode_area.scaled_area = arearesult.datacolmuxpredecode_area.height * arearesult.datacolmuxpredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpostdecode_area.scaled_area = arearesult.datacolmuxpostdecode_area.height * arearesult.datacolmuxpostdecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datawritesig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)* arearesult.datawritesig_area.height * arearesult.datawritesig_area.width * CONVERT_TO_MMSQUARE;

   arearesult.tagarray_area.scaled_area = arearesult.tagarray_area.height * arearesult.tagarray_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagpredecode_area.scaled_area = arearesult.tagpredecode_area.height * arearesult.tagpredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpredecode_area.scaled_area = arearesult.tagcolmuxpredecode_area.height * arearesult.tagcolmuxpredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpostdecode_area.scaled_area = arearesult.tagcolmuxpostdecode_area.height* arearesult.tagcolmuxpostdecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvdecode_area.scaled_area = arearesult.tagoutdrvdecode_area.height * arearesult.tagoutdrvdecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvsig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)*
											 arearesult.tagoutdrvsig_area.height * arearesult.tagoutdrvsig_area.width * CONVERT_TO_MMSQUARE;

   arearesult.perc_data = 100*area_all_dataramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_tag  = 100*area_all_tagramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_cont = 100*(arearesult.totalarea*CONVERT_TO_MMSQUARE-area_all_dataramcells-area_all_tagramcells)/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.sub_eff   = (area_all_dataramcells+area_all_tagramcells)*100/(arearesult.totalarea/100000000.0);
   arearesult.total_eff = (NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/
							(arearesult_subbanked.height * arearesult_subbanked.width * CONVERT_TO_MMSQUARE);
   arearesult.totalarea *= CONVERT_TO_MMSQUARE;
   arearesult.subbankarea = arearesult_subbanked.height * arearesult_subbanked.width * CONVERT_TO_MMSQUARE; 

  
   if(result.bitline_delay_data < 0.0) {
	   result.bitline_delay_data = 10^-12;
   }
   if(result.bitline_delay_tag < 0.0) {
	   result.bitline_delay_tag = 10^-13;
   }
   endresult.result = result;
   endresult.result.subbanks = banks;
   endresult.area = arearesult;
   endresult.params = parameters;
   
   if(VddPow) *VddPow = parameters.VddPow;

   return endresult;
}

Cacti4_2::total_result_type Cacti4_2::cacti_interface(
		int cache_size,
		int line_size,
		int associativity,
		int rw_ports,
		int excl_read_ports,
		int excl_write_ports,
		int single_ended_read_ports,
		int banks,
		double tech_node,
		int output_width,
		int specific_tag,
		int tag_width,
		int access_mode,
		int pure_sram,
		int Ndwl,
		int Ndbl,
		int Ntwl,
		int Ntbl,
		int Ntspd,
		double Nspd,
		double voltage)
{
   int C,B,A,ERP,EWP,RWP,NSER;
   double tech;
   double logbanks, assoc;
   double logbanksfloor, assocfloor;
   int seq_access = 0;
   int fast_access = 0;
   int bits_output = output_width;
   int nr_args = 9;
   double NSubbanks = (double)banks;

   double ratioofbankstoports;

   //extern int force_tag, force_tag_size;

   total_result_type endresult;

   result_type result;
   arearesult_type arearesult;
   area_type arearesult_subbanked;
   parameter_type parameters; 

   /* input parameters:
         C B A ERP EWP */

   /*dt: make sure we're using some simple leakage reduction */
   dualVt = FALSE;

    force_tag = 0;

   if(specific_tag) {
	   force_tag = 1;
	   force_tag_size = tag_width;
   }
   switch (access_mode){
	   case 0:
		   seq_access = fast_access = FALSE;
		   break;
	   case 1:
		   seq_access = TRUE;
		   fast_access = FALSE;
		   break;
	   case 2:
		   seq_access = FALSE;
		   fast_access = TRUE;
		   break;
   }

   B = line_size;
   if ((B < 1)) {
       printf("Block size must >=1\n");
       return endresult;
	   //exit(1);
   }

   if ((B*8 < bits_output)) {
       printf("Block size must be at least %d\n", bits_output/8);
       return endresult;
	   //exit(1);
   }
   

   tech = tech_node;
   if ((tech <= 0)) {
       printf("Feature size must be > 0\n");
       return endresult;
	   //exit(1);
   }
   if ((tech > 0.8)) {
       printf("Feature size must be <= 0.80 (um)\n");
       return endresult;
	   //exit(1);
   }

   if (nr_args ==9)
     {
       RWP = rw_ports;
       ERP = excl_read_ports;
       EWP = excl_write_ports;
       NSER = single_ended_read_ports;


       if ((RWP < 0) || (EWP < 0) || (ERP < 0)) {
			printf("Ports must >=0\n");
			return endresult;
			//exit(1);
       }
       if (RWP > 2) {
			printf("Maximum of 2 read/write ports\n");
			return endresult;
			//exit(1);
       }
       if ((RWP+ERP+EWP) < 1) {
       	 printf("Must have at least one port\n");
       	 return endresult;
		 //exit(1);
       }

       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);
      
       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
   else if (nr_args==10)
     {
       RWP = rw_ports;
       ERP = excl_read_ports;
       EWP = excl_write_ports;
       NSER = single_ended_read_ports;

	   seq_access = 1;

       if ((RWP < 0) || (EWP < 0) || (ERP < 0)) {
			printf("Ports must >=0\n");
			return endresult;
			//exit(1);
       }
       if (RWP > 2) {
			printf("Maximum of 2 read/write ports\n");
			return endresult;
			//exit(1);
       }
       if ((RWP+ERP+EWP) < 1) {
       	 printf("Must have at least one port\n");
       	 return endresult;
		 //exit(1);
       }
       

       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);
      
       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
   else if(nr_args==6)
     {
       RWP=1;
       ERP=0;
       EWP=0;
       NSER=0;

       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult; 
		 //exit(1);
       }
       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);

       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
	 else if(nr_args==8)
     {
       RWP=1;
       ERP=0;
       EWP=0;
       NSER=0;

	   bits_output = output_width;
	   seq_access = 1;

       NSubbanks = banks;
       if (NSubbanks < 1 ) {
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }
       logbanks = logtwo((double)(NSubbanks));
       logbanksfloor = floor(logbanks);

       if(logbanks > logbanksfloor){
         printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         return endresult;
		 //exit(1);
       }

     }
 
   C = cache_size/((int) (NSubbanks));
   if ((C < 64)) {
       printf("Cache size must >=64\n");
       return endresult;
	   //exit(1);
   }
 
   if (associativity == 0)
     {
       A=C/B;
       parameters.fully_assoc = 1;
     }
   else
     {
       if (associativity == 1)
         {
           A=1;
           parameters.fully_assoc = 0;
         }
       else
         {
           parameters.fully_assoc = 0;
           A = associativity;
           if ((A < 1)) {
             printf("Associativity must >= 1\n");
             return endresult;
			 //exit(1);
           }
           assoc = logtwo((double)(A));
		   assocfloor = floor(assoc);
		   
           if(assoc > assocfloor){
             printf("Associativity should be a power of 2\n");
             return endresult;
			 //exit(1);
           }

           if ((A > 32)) {
             printf("Associativity must <= 32\n or try FA (fully associative)\n");
             return endresult;
			 //exit(1);
           }
         }
     }

   if (C/(B*A)<=1 && !parameters.fully_assoc) {
     printf("Number of sets is too small:\n  Need to either increase cache size, or decrease associativity or block size\n  (or use fully associative cache)\n");
     return endresult;
	//exit(1); 
   }
 
   parameters.cache_size = C;
   parameters.block_size = B;

   parameters.nr_bits_out = bits_output;
   /*dt: testing sequential access mode*/
   if(seq_access) {
	parameters.tag_associativity = A;
	parameters.data_associativity = 1;
	parameters.sequential_access = 1;
   }
   else {
	parameters.tag_associativity = parameters.data_associativity = A;
	parameters.sequential_access = 0;
   }
   if(fast_access) {
	   parameters.fast_access = 1;
   }
   else {
	   parameters.fast_access = 0;
   }
   parameters.num_readwrite_ports = RWP;
   parameters.num_read_ports = ERP;
   parameters.num_write_ports = EWP;
   parameters.num_single_ended_read_ports =NSER;
   parameters.number_of_sets = C/(B*A);
   parameters.fudgefactor = .8/tech;   
   parameters.tech_size=(double) tech;
   parameters.pure_sram = pure_sram;

   //If multiple banks and multiple ports are specified, then if number of banks/total number 
   //of ports > 1 then assume that the multiple ports are implemented via the multiple banks.
   //Also assume that each bank has only 1 RWP port. There are some problems with this logic that 
   //will be fixed in v5.0
   ratioofbankstoports = NSubbanks/(RWP + ERP + EWP);
   if(ratioofbankstoports >= 1.0){
	   //We assume that each bank has 1 RWP port.
	   parameters.num_readwrite_ports = 1;
       parameters.num_read_ports = 0;
       parameters.num_write_ports = 0;
       parameters.num_single_ended_read_ports = 0;
	}

   if (parameters.number_of_sets < 1) {
      printf("Less than one set...\n");
      return endresult;
	  //exit(1);
   }   
 
   init_tech_params_default_process();//v4.1: First initialize all tech variables
   //to 0.8 micron values. init_tech_params function below then reinitializes tech variables to
   //given process values
   init_tech_params(parameters.tech_size);
   calculate_time(&result,&arearesult,&arearesult_subbanked,&parameters,&NSubbanks, Ndwl, Ndbl, Ntwl, Ntbl, Ntspd, Nspd, voltage);

   //v4.1: No longer using calculate_area function as area has already been
   //computed for the given tech node

   /*arearesult.dataarray_area.scaled_area = calculate_area(arearesult.dataarray_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datapredecode_area.scaled_area = calculate_area(arearesult.datapredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpredecode_area.scaled_area = calculate_area(arearesult.datacolmuxpredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpostdecode_area.scaled_area = calculate_area(arearesult.datacolmuxpostdecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.datawritesig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)*calculate_area(arearesult.datawritesig_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;

   arearesult.tagarray_area.scaled_area = calculate_area(arearesult.tagarray_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagpredecode_area.scaled_area = calculate_area(arearesult.tagpredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpredecode_area.scaled_area = calculate_area(arearesult.tagcolmuxpredecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpostdecode_area.scaled_area = calculate_area(arearesult.tagcolmuxpostdecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvdecode_area.scaled_area = calculate_area(arearesult.tagoutdrvdecode_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvsig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)*
											   calculate_area(arearesult.tagoutdrvsig_area,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;

   arearesult.perc_data = 100*area_all_dataramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_tag  = 100*area_all_tagramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_cont = 100*(arearesult.totalarea*CONVERT_TO_MMSQUARE-area_all_dataramcells-area_all_tagramcells)/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.sub_eff   = (area_all_dataramcells+area_all_tagramcells)*100/(arearesult.totalarea/100000000.0);
   arearesult.total_eff = (NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/
							(calculate_area(arearesult_subbanked,parameters.fudgefactor)*CONVERT_TO_MMSQUARE);
   arearesult.totalarea *= CONVERT_TO_MMSQUARE;
   arearesult.subbankarea = calculate_area(arearesult_subbanked,parameters.fudgefactor)*CONVERT_TO_MMSQUARE;*/
   
   arearesult.dataarray_area.scaled_area = arearesult.dataarray_area.height * arearesult.dataarray_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datapredecode_area.scaled_area = arearesult.datapredecode_area.height * arearesult.datapredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpredecode_area.scaled_area = arearesult.datacolmuxpredecode_area.height * arearesult.datacolmuxpredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datacolmuxpostdecode_area.scaled_area = arearesult.datacolmuxpostdecode_area.height * arearesult.datacolmuxpostdecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.datawritesig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)* arearesult.datawritesig_area.height * arearesult.datawritesig_area.width * CONVERT_TO_MMSQUARE;

   arearesult.tagarray_area.scaled_area = arearesult.tagarray_area.height * arearesult.tagarray_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagpredecode_area.scaled_area = arearesult.tagpredecode_area.height * arearesult.tagpredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpredecode_area.scaled_area = arearesult.tagcolmuxpredecode_area.height * arearesult.tagcolmuxpredecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagcolmuxpostdecode_area.scaled_area = arearesult.tagcolmuxpostdecode_area.height* arearesult.tagcolmuxpostdecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvdecode_area.scaled_area = arearesult.tagoutdrvdecode_area.height * arearesult.tagoutdrvdecode_area.width * CONVERT_TO_MMSQUARE;
   arearesult.tagoutdrvsig_area.scaled_area = (parameters.num_readwrite_ports+parameters.num_read_ports+parameters.num_write_ports)*
											 arearesult.tagoutdrvsig_area.height * arearesult.tagoutdrvsig_area.width * CONVERT_TO_MMSQUARE;

   arearesult.perc_data = 100*area_all_dataramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_tag  = 100*area_all_tagramcells/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.perc_cont = 100*(arearesult.totalarea*CONVERT_TO_MMSQUARE-area_all_dataramcells-area_all_tagramcells)/(arearesult.totalarea*CONVERT_TO_MMSQUARE);
   arearesult.sub_eff   = (area_all_dataramcells+area_all_tagramcells)*100/(arearesult.totalarea/100000000.0);
   arearesult.total_eff = (NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/
							(arearesult_subbanked.height * arearesult_subbanked.width * CONVERT_TO_MMSQUARE);
   arearesult.totalarea *= CONVERT_TO_MMSQUARE;
   arearesult.subbankarea = arearesult_subbanked.height * arearesult_subbanked.width * CONVERT_TO_MMSQUARE; 

  
   if(result.bitline_delay_data < 0.0) {
	   result.bitline_delay_data = 10^-12;
   }
   if(result.bitline_delay_tag < 0.0) {
	   result.bitline_delay_tag = 10^-13;
   }
   endresult.result = result;
   endresult.result.subbanks = banks;
   endresult.area = arearesult;
   endresult.params = parameters;

   return endresult;
}

int Cacti4_2::main(int argc,char *argv[])
{
    total_result_type result2;
	
	//Note that the following command line options are permitted:
	//C B A TECH Nbanks
    //C B A TECH RWP ERP EWP Nbanks
	//C B A RWP ERP EWP NSER Nbanks TECH OUTPUTWIDTH CUSTOMTAG TAGWIDTH ACCESSMODE PURESRAM
	//(CUSTOMTAG should be set to 1 or 0 depending on whether the tag width needs to be
	//changed (1) or not(0). TAGWIDTH IS the desired tag width if the tag width is being 
	//changed. ACCESSMODE = 0 for normal access, ACCESSMODE = 1 for sequential access,
	//ACCESSMODE = 2 for fast access. PURESRAM = 1 if the memory is a scratchpad memory,
	//PURESRAM = 0 if the memory is a cache

    if (input_data(argc,argv) == ERROR) exit(1);
	else{
		if (argc == 6){
			if (strcmp(argv[3],"FA") == 0) {
				result2 = cacti_interface(atoi(argv[1]), atoi(argv[2]), 0, 1, 0, 0, 0, atoi(argv[5]), atof(argv[4]), 64, 0, 0, 0, 0);
			} 
			else if (strcmp(argv[3],"DM") == 0){
					result2 = cacti_interface(atoi(argv[1]), atoi(argv[2]), 1, 1, 0, 0, 0, atoi(argv[5]), atof(argv[4]), 64, 0, 0, 0, 0);
				 }
				 else{
					 result2 = cacti_interface(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), 1, 0, 0, 0, atoi(argv[5]), atof(argv[4]), 64, 0, 0, 0, 0);
				 }
		}
		else if (argc == 9){
				if (strcmp(argv[3],"FA") == 0) {
					result2 = cacti_interface(atoi(argv[1]), atoi(argv[2]), 0, 1, 0, 0, 0, atoi(argv[5]), atof(argv[4]), 64, 0, 0, 0, 0);
				} 
				else if (strcmp(argv[3],"DM") == 0){
						result2 = cacti_interface(atoi(argv[1]), atoi(argv[2]), 1, 1, 0, 0, 0, atoi(argv[5]), atof(argv[4]), 64, 0, 0, 0, 0);
					 }
					 else{
						result2 = cacti_interface(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), 0, atoi(argv[8]), atof(argv[4]), 64, 0, 0, 0, 0);
						 }
			   }
			  else {
					result2 = cacti_interface(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), atof(argv[9]), atoi(argv[10]), atoi(argv[11]), atoi(argv[12]), atoi(argv[13]), atoi(argv[14]));
			  }
	}

	output_data(&result2.result,&result2.area,&result2.params);
	return 0;
}


Cacti4_2::Cacti4_2()
{
}

Cacti4_2::~Cacti4_2()
{
}

} // end of namespace power
} // end of namespace service
} // end of namespace unisim

#if 0
int main(int argc,char *argv[])
{
	unisim::service::power::Cacti4_2 cacti;

	return cacti.main(argc, argv);
}
#endif
