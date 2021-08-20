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

#ifndef __FS_PLUGINS_POWER_CACTI_4_1_HH__
#define __FS_PLUGINS_POWER_CACTI_4_1_HH__


namespace unisim {
namespace service {
namespace power {

class Cacti4_2
{
public:
	/* from cacti_interface.h */
	typedef struct {
		double dynamic;
		double leakage;
	} powerComponents;
	
	typedef struct {
		powerComponents readOp;
		powerComponents writeOp;
	} powerDef;
	
	/* Used to pass values around the program */
	
	struct cache_params_t
	{
		unsigned int nsets;
		unsigned int assoc;
		unsigned int dbits;
		unsigned int tbits;
		
		unsigned int nbanks;
		unsigned int rport;
		unsigned int wport;
		unsigned int rwport;
		unsigned int serport; // single-ended bitline read ports 
		
		unsigned int obits;
		unsigned int abits;
		
		double dweight;
		double pweight;
		double aweight;
	};
	
	struct subarray_params_t
	{
		unsigned int Ndwl;
		unsigned int Ndbl;
		unsigned int Nspd;
		unsigned int Ntwl;
		unsigned int Ntbl;
		unsigned int Ntspd;
		unsigned int muxover;
	};
	
	
	struct tech_params_t 
	{
		double tech_size;
		double crossover;
		double standby;
		double VddPow;
		double scaling_factor;
	};
	
	
	typedef struct {
		double height;
		double width;
		double scaled_area;
	} area_type;
	
	typedef struct {
		Cacti4_2::area_type dataarray_area,datapredecode_area;
		Cacti4_2::area_type datacolmuxpredecode_area,datacolmuxpostdecode_area;
		Cacti4_2::area_type datawritesig_area;
		Cacti4_2::area_type tagarray_area,tagpredecode_area;
		Cacti4_2::area_type tagcolmuxpredecode_area,tagcolmuxpostdecode_area;
		Cacti4_2::area_type tagoutdrvdecode_area;
		Cacti4_2::area_type tagoutdrvsig_area;
		double totalarea, subbankarea;
		double total_dataarea;
		double total_tagarea;
		double max_efficiency, efficiency;
		double max_aspect_ratio_total, aspect_ratio_total;
		double perc_data, perc_tag, perc_cont, sub_eff, total_eff;
	}arearesult_type;
	
	typedef struct {
		int cache_size;
		int number_of_sets;
		//int associativity;
		int tag_associativity, data_associativity;
		int block_size;
		int num_write_ports;
		int num_readwrite_ports;
		int num_read_ports;
		int num_single_ended_read_ports;
		char fully_assoc;
		double fudgefactor;
		double tech_size;
		double VddPow;
		int sequential_access;
		int fast_access;
		int force_tag;
		int tag_size;
		int nr_bits_out;
		int pure_sram;
	} parameter_type;
	
	typedef struct {
		int subbanks;
		double access_time,cycle_time;
		double senseext_scale;
		powerDef total_power;
		int best_Ndwl,best_Ndbl;
		double max_power, max_access_time;
		double best_Nspd;
		int best_Ntwl,best_Ntbl;
		int best_Ntspd;
		int best_muxover;
		powerDef total_routing_power;
		powerDef total_power_without_routing, total_power_allbanks;
		double subbank_address_routing_delay;
		powerDef subbank_address_routing_power;
		double decoder_delay_data,decoder_delay_tag;
		powerDef decoder_power_data,decoder_power_tag;
		double dec_data_driver,dec_data_3to8,dec_data_inv;
		double dec_tag_driver,dec_tag_3to8,dec_tag_inv;
		double wordline_delay_data,wordline_delay_tag;
		powerDef wordline_power_data,wordline_power_tag;
		double bitline_delay_data,bitline_delay_tag;
		powerDef bitline_power_data,bitline_power_tag;
		double sense_amp_delay_data,sense_amp_delay_tag;
		powerDef sense_amp_power_data,sense_amp_power_tag;
		double total_out_driver_delay_data;
		powerDef total_out_driver_power_data;
		double compare_part_delay;
		double drive_mux_delay;
		double selb_delay;
		powerDef compare_part_power, drive_mux_power, selb_power;
		double data_output_delay;
		powerDef data_output_power;
		double drive_valid_delay;
		powerDef drive_valid_power;
		double precharge_delay;
		int data_nor_inputs;
		int tag_nor_inputs;
	} result_type;
	
	typedef struct{
		result_type result;
		arearesult_type area;
		parameter_type params;
	}total_result_type;
	
	Cacti4_2();
	virtual ~Cacti4_2();
	
	int main(int argc,char *argv[]);

	void calculate_time (result_type *result,arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters, double *NSubbanks);

	void calculate_time(result_type *result,arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters, double *NSubbanks, int Ndwl, int Ndbl, int Ntwl, int Ntbl, int Ntspd, double Nspd, double voltage);

	void output_data(result_type *result,arearesult_type *arearesult, parameter_type *parameters) const;
	
	total_result_type cacti_interface(
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
			double *VddPow = 0);

	total_result_type cacti_interface(
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
			double voltage);

private:

	typedef struct{
		double delay;
		powerDef power;
	}output_tuples;
	
	typedef struct{
		output_tuples
				decoder,
				wordline,
				bitline,
				senseamp;
		double senseamp_outputrisetime;
	}cached_tag_entry;
	
	/* total results */
	total_result_type total_result;
	
	/* from cacti_interface.h */
	static const int CHUNKSIZE;

	/*  The following are things you might want to change
	*  when compiling
	*/
	
	/*
	* The output can be in 'long' format, which shows everything, or
	* 'short' format, which is just what a program like 'grep' would
	* want to see
	*/

	static const int LONG;
	static const int SHORT;

	static const int OUTPUTTYPE;

	/*
	* Address bits in a word, and number of output bits from the cache 
	*/
	
	/*
	was: #define ADDRESS_BITS 32
	now: I'm using 42 bits as in the Power4, 
	since that's bigger then the 36 bits on the Pentium 4 
	and 40 bits on the Opteron
	*/
	static const int ADDRESS_BITS;
	/*
	was: #define BITOUT 64
	now: making it a commandline parameter
	*/
	int BITOUT;

	/*dt: In addition to the tag bits, the tags also include 1 valid bit, 1 dirty bit, 2 bits for a 4-state 
	cache coherency protocoll (MESI), 1 bit for MRU (change this to log(ways) for full LRU). 
	So in total we have 1 + 1 + 2 + 1 = 5 */
	static const int EXTRA_TAG_BITS;


	/* limits on the various N parameters */
	
	static const int MAXDATAN;          /* Maximum for Ndwl,Ndbl */
	static const int MAXTAGN;          /* Maximum for Ndwl,Ndbl */
	static const int MAXSUBARRAYS;    /* Maximum subarrays for data and tag arrays */
	static const int MAXDATASPD;         /* Maximum for Nspd */
	static const int MAXTAGSPD;         /* Maximum for Ntspd */



	/*
	* The following scale factor can be used to scale between technologies.
	* To convert from 0.8um to 0.5um, make FUDGEFACTOR = 1.6
	*/
	
	double FUDGEFACTOR;
	double FEATURESIZE;

	/*===================================================================*/
	
	/*
	* Cache layout parameters and process parameters 
	*/
	
	
	/*
	* CMOS 0.8um model parameters
	*   - directly from Appendix II of tech report
	*/
	
	/*#define WORDWIRELENGTH (8+2*WIREPITCH*(EXTRAWRITEPORTS)+2*WIREPITCH*(EXTRAREADPORTS))*/
	/*#define BITWIRELENGTH (16+2*WIREPITCH*(EXTRAWRITEPORTS+EXTRAREADPORTS))*/
	/*
	was: 
#define WIRESPACING (2*FEATURESIZE)
#define WIREWIDTH (3*FEATURESIZE)
	is: width and pitch are taken from the Intel IEDM 2004 paper on their 65nm process.
	*/

	double WIRESPACING;
	double WIREWIDTH;
	/*dt: I've taken the aspect ratio from the Intel paper on their 65nm process */
	static const double WIREHEIGTHRATIO;

	double WIREPITCH;
	/*
#define Default_Cmetal 275e-18
	*/
	/*dt: The old Cmetal was calculated using SiO2 as dielectric (k = 3.9). Going by the Intel 65nm paper, 
	low-k dielectics are not at k=2.9. This is a very simple adjustment, as lots of other factors also go into the average
	capacitance, but I don't have any better/more up to date data on wire distribution, etc. than what's been done for cacti 1.0.
	So I'm doing a simple adjustment by 2.9/3.9 */
	//#define Default_Cmetal (2.9/3.9*275e-18)
	/*dt: changing this to reflect newer data */
	/* 2.9: is the k value for the low-k dielectric used in 65nm Intel process 
	3.9: is the k value of normal SiO2
	--> multiply by 2.9/3.9 to get new C values
	the Intel 130nm process paper mentioned 230fF/mm for M1 through M5 with k = 3.6
	So we get
	230*10^-15/mm * 10^-3 mm/1um * 3.9/3.6
	*/
	static const double Default_Cmetal;
	static const double Default_Rmetal;
	/* dt: I'm assuming that even with all the process improvements, 
	copper will 'only' have 2/3 the sheet resistance of aluminum. */
	static const double Default_CopperSheetResistancePerMicroM;

	/*dt: this number is calculated from the 2004 ITRS tables (RC delay and Wire sizes)*/
	static const double CRatiolocal_to_interm;
	/*dt: from ITRS 2004 using wire sizes, aspect ratios and effective resistivities for local and intermediate*/
	static const double RRatiolocal_to_interm;

	static const double CRatiointerm_to_global;
	/*dt: from ITRS 2004 using wire sizes, aspect ratios and effective resistivities for local and intermediate*/
	static const double RRatiointerm_to_global;

	double WireRscaling;
	double WireCscaling;
	
	double Cwordmetal;
	double Cbitmetal;
	double Rwordmetal;
	double Rbitmetal;
	
	double TagCwordmetal;
	double TagCbitmetal;
	double TagRwordmetal;
	double TagRbitmetal;
	
	double GlobalCwordmetal;
	double GlobalCbitmetal;
	double GlobalRwordmetal;
	double GlobalRbitmetal;
	double FACwordmetal;
	double FACbitmetal;
	double FARwordmetal;
	double FARbitmetal;

	int muxover;

	/* fF/um2 at 1.5V */
	//#define Cndiffarea    0.137e-15
	
	//v4.1: Making all constants static variables. Initially these variables are based
	//off 0.8 micron process values; later on in init_tech_params function of leakage.c 
	//they are scaled to input tech node parameters 

	double Cndiffarea;

	/* fF/um2 at 1.5V */
	//#define Cpdiffarea    0.343e-15
	
	double Cpdiffarea;
	
	/* fF/um at 1.5V */
	//#define Cndiffside    0.275e-15
	
	double Cndiffside;
	
	/* fF/um at 1.5V */
	//#define Cpdiffside    0.275e-15
	double Cpdiffside;
	
	/* fF/um at 1.5V */
	//#define Cndiffovlp    0.138e-15
	double Cndiffovlp;
	
	/* fF/um at 1.5V */
	//#define Cpdiffovlp    0.138e-15
	double Cpdiffovlp;
	
	/* fF/um assuming 25% Miller effect */
	//#define Cnoxideovlp   0.263e-15
	double Cnoxideovlp;
	
	/* fF/um assuming 25% Miller effect */
	//#define Cpoxideovlp   0.338e-15
	double Cpoxideovlp;
	
	/* um */
	//#define Leff          (0.8)
	double Leff;
	
	//#define inv_Leff	  1.25
	double inv_Leff;
	
	/* fF/um2 */
	//#define Cgate         1.95e-15
	double Cgate;	
	
	/* fF/um2 */
	//#define Cgatepass     1.45e-15
	double Cgatepass;		

	/* note that the value of Cgatepass will be different depending on 
	whether or not the source and drain are at different potentials or
	the same potential.  The two values were averaged */
	
	/* fF/um */
	//#define Cpolywire	(0.25e-15)	
	double Cpolywire;			 
	
	/* ohms*um of channel width */
	//#define Rnchannelstatic	(25800)
	double Rnchannelstatic;
	
	/* ohms*um of channel width */
	//#define Rpchannelstatic	(61200)
	double Rpchannelstatic;
	
	//#define Rnchannelon	(8751)
	double Rnchannelon;
	
	//#define Rpchannelon	(20160)
	double Rpchannelon;
	
	
	static const double Vdd;
	double VddPow;
	/* Threshold voltages (as a proportion of Vdd)
	If you don't know them, set all values to 0.5 */

	static const double SizingRatio;
	static const double VTHNAND;
	static const double VTHFA1;
	static const double VTHFA2;
	static const double VTHFA3;
	static const double VTHFA4;
	static const double VTHFA5;
	static const double VTHFA6;
	static const double VSINV;
	static const double VTHINV100x60;   /* inverter with p=100,n=60 */
	static const double VTHINV360x240;   /* inverter with p=360, n=240 */
	static const double VTHNAND60x90;   /* nand with p=60 and three n=90 */
	static const double VTHNOR12x4x1;   /* nor with p=12, n=4, 1 input */
	static const double VTHNOR12x4x2;   /* nor with p=12, n=4, 2 inputs */
	static const double VTHNOR12x4x3;   /* nor with p=12, n=4, 3 inputs */
	static const double VTHNOR12x4x4;   /* nor with p=12, n=4, 4 inputs */
	static const double VTHOUTDRINV;
	static const double VTHOUTDRNOR;
	static const double VTHOUTDRNAND;
	static const double VTHOUTDRIVE;
	static const double VTHCOMPINV;
	static const double VTHMUXNAND;
	static const double VTHMUXDRV1;
	static const double VTHMUXDRV2;
	static const double VTHMUXDRV3;
	static const double VTHEVALINV;
	static const double VTHSENSEEXTDRV;
	
	static const double VTHNAND60x120;

	/* transistor widths in um (as described in tech report, appendix 1) */
	/* incorporating changes from eCACTI. Mostly device widths are now calculated with logical effort
	and no longer static. Some changes to constants to reflect improving/changing circuit/device technology */

	//#define Wdecdrivep	(360.0)
	double Wdecdrivep;
	//#define Wdecdriven	(240.0)
	double Wdecdriven;
	/*#define Wdec3to8n     120.0
#define Wdec3to8p     60.0
#define WdecNORn       2.4
#define WdecNORp      12.0
#define Wdecinvn      20.0
#define Wdecinvp      40.0 */


	//#define Wworddrivemax 100.0
	double Wworddrivemax;
	
	double Waddrdrvn1;
	double Waddrdrvp1;
	double Waddrdrvn2;
	double Waddrdrvp2;
	
	double Wdecdrivep_second;
	double Wdecdriven_second;
	double Wdecdrivep_first;
	double Wdecdriven_first;
	double WdecdrivetreeN[10];
	double Cdectreesegments[10], Rdectreesegments[10];
	int    nr_dectreesegments;
	double Wdec3to8n ;
	double Wdec3to8p ;
	double WdecNORn  ;
	double WdecNORp  ;
	double Wdecinvn  ;
	double Wdecinvp  ;
	double WwlDrvn ;
	double WwlDrvp ;

	double Wtdecdrivep_second;
	double Wtdecdriven_second;
	double Wtdecdrivep_first;
	double Wtdecdriven_first;
	double WtdecdrivetreeN[10];
	double Ctdectreesegments[10], Rtdectreesegments[10];
	int    nr_tdectreesegments;
	double Wtdec3to8n ;
	double Wtdec3to8p ;
	double WtdecNORn  ;
	double WtdecNORp  ;
	double Wtdecinvn  ;
	double Wtdecinvp  ;
	double WtwlDrvn ;
	double WtwlDrvp ;


	/* #define Wmemcella	(2.4)
	// added by MnM
	// #define Wmemcellpmos (4.0)
	// #define Wmemcellnmos (2.0)
	*/

	//#define Wmemcella	(0.9)
	double Wmemcella;

	/* added by MnM */
	//#define Wmemcellpmos (0.65)
	double Wmemcellpmos;
	//#define Wmemcellnmos (2.0)
	double Wmemcellnmos;


	//#define Wmemcellbscale	2		/* means 2x bigger than Wmemcella */
	int Wmemcellbscale;
	/* #define Wbitpreequ	(80.0) */
	double Wbitpreequ;
	//#define Wpchmax		(25.0) /* precharge transistor sizes usually do not exceed 25 */
	double Wpchmax;


	/* #define Wbitmuxn	(10.0)
	//#define WsenseQ1to4	(4.0) */

	double Wpch;
	double Wiso;
	double WsenseEn;
	double WsenseN;
	double WsenseP;
	double WsPch;
	double WoBufN;
	double WoBufP;
	
	double WpchDrvp, WpchDrvn;
	double WisoDrvp, WisoDrvn;
	double WspchDrvp, WspchDrvn;
	double WsenseEnDrvp, WsenseEnDrvn;
	
	double WwrtMuxSelDrvn;
	double WwrtMuxSelDrvp;
	double WtwrtMuxSelDrvn;
	double WtwrtMuxSelDrvp;
	
	double Wtbitpreequ;
	double Wtpch;
	double Wtiso;
	double WtsenseEn;
	double WtsenseN;
	double WtsenseP;
	double WtoBufN;
	double WtoBufP;
	double WtsPch;
	
	double WtpchDrvp, WtpchDrvn;
	double WtisoDrvp, WtisoDrvn;
	double WtspchDrvp, WtspchDrvn;
	double WtsenseEnDrvp, WtsenseEnDrvn;

	//#define Wcompinvp1	(10.0)
	double Wcompinvp1;
	//#define Wcompinvn1	(6.0)
	double Wcompinvn1;
	//#define Wcompinvp2	(20.0)
	double Wcompinvp2;
	//#define Wcompinvn2	(12.0)
	double Wcompinvn2;
	//#define Wcompinvp3	(40.0)
	double Wcompinvp3;
	//#define Wcompinvn3	(24.0)
	double Wcompinvn3;
	//#define Wevalinvp	(80.0)
	double Wevalinvp;
	//#define Wevalinvn	(40.0)
	double Wevalinvn;
	
	//#define Wfadriven    (50.0)
	double Wfadriven;
	//#define Wfadrivep    (100.0)
	double Wfadrivep;
	//#define Wfadrive2n    (200.0)
	double Wfadrive2n;
	//#define Wfadrive2p    (400.0)
	double Wfadrive2p;
	//#define Wfadecdrive1n    (5.0)
	double Wfadecdrive1n;
	//#define Wfadecdrive1p    (10.0)
	double Wfadecdrive1p;
	//#define Wfadecdrive2n    (20.0)
	double Wfadecdrive2n;
	//#define Wfadecdrive2p    (40.0)
	double Wfadecdrive2p;
	//#define Wfadecdriven    (50.0)
	double Wfadecdriven;
	//#define Wfadecdrivep    (100.0)
	double Wfadecdrivep;
	//#define Wfaprechn       (6.0)
	double Wfaprechn;
	//#define Wfaprechp       (10.0)
	double Wfaprechp;
	//#define Wdummyn         (10.0)
	double Wdummyn;
	//#define Wdummyinvn      (60.0)
	double Wdummyinvn;
	//#define Wdummyinvp      (80.0)
	double Wdummyinvp;
	//#define Wfainvn         (10.0)
	double Wfainvn;
	//#define Wfainvp         (20.0)
	double Wfainvp;
	//#define Waddrnandn      (50.0)
	double Waddrnandn;
	//#define Waddrnandp      (50.0)
	double Waddrnandp;
	//#define Wfanandn        (20.0)
	double Wfanandn;
	//#define Wfanandp        (30.0)
	double Wfanandp;
	//#define Wfanorn         (5.0)
	double Wfanorn;
	//#define Wfanorp         (10.0)
	double Wfanorp;
	//#define Wdecnandn       (10.0)
	double Wdecnandn;
	//#define Wdecnandp       (30.0)
	double Wdecnandp;
	
	//#define Wcompn		(10.0)
	double Wcompn;
	//#define Wcompp		(30.0)
	double Wcompp;
	//#define Wmuxdrv12n	(60.0)
	double Wmuxdrv12n;
	//#define Wmuxdrv12p	(100.0)
	double Wmuxdrv12p;
	
	/* #define WmuxdrvNANDn    (60.0)
	//#define WmuxdrvNANDp    (80.0)
	//#define WmuxdrvNORn	(40.0)
	//#define WmuxdrvNORp	(100.0)
	//#define Wmuxdrv3n	(80.0)
	//#define Wmuxdrv3p	(200.0)
	// #define Woutdrvseln	(24.0)
	// #define Woutdrvselp	(40.0)
	*/

	double Coutdrvtreesegments[20], Routdrvtreesegments[20];
	double WoutdrvtreeN[20];
	int    nr_outdrvtreesegments;
	
	double Cmuxdrvtreesegments[20], Rmuxdrvtreesegments[20];
	double WmuxdrvtreeN[20];
	int    nr_muxdrvtreesegments;
	
	double WmuxdrvNANDn    ;
	double WmuxdrvNANDp    ;
	double WmuxdrvNORn	;
	double WmuxdrvNORp	;
	double Wmuxdrv3n	;
	double Wmuxdrv3p	;
	double Woutdrvseln	;
	double Woutdrvselp	;


	/* #define Woutdrvnandn	(10.0)
	//#define Woutdrvnandp	(30.0)
	//#define Woutdrvnorn	(5.0)
	//#define Woutdrvnorp	(20.0)
	//#define Woutdrivern	(48.0)
	//#define Woutdriverp	(80.0)
	*/
	
	double Woutdrvnandn;
	double Woutdrvnandp;
	double Woutdrvnorn	;
	double Woutdrvnorp	;
	double Woutdrivern	;
	double Woutdriverp	;


	//#define Wsenseextdrv1p (80.0)
	double Wsenseextdrv1p;
	//#define Wsenseextdrv1n (40.0)
	double Wsenseextdrv1n;
	//#define Wsenseextdrv2p (240.0)
	double Wsenseextdrv2p;
	//#define Wsenseextdrv2n (160.0)
	double Wsenseextdrv2n;
	
	
	/* other stuff (from tech report, appendix 1) */
	
	//#define krise		(0.4e-9)
	double krise;
	//#define tsensedata	(5.8e-10)
	double tsensedata;
	// #define psensedata      (0.025e-9)
	//#define psensedata      (0.02e-9)
	double psensedata;
	//#define tsensescale     0.02e-10
	double tsensescale;
	//#define tsensetag	(2.6e-10)
	double tsensetag;
	// #define psensetag       (0.01e-9)
	//#define psensetag	(0.016e-9)
	double psensetag;
	//#define tfalldata	(7e-10)
	double tfalldata;
	//#define tfalltag	(7e-10)
	double tfalltag;
	static const double Vbitpre;
	double VbitprePow;
	static const double Vt;
	/*
	was: #define Vbitsense	(0.10)
	now: 50mV seems to be the norm as of 2005
	*/
	static const double Vbitsense;
	static const double Vbitswing;

	/* bit width of RAM cell in um */
	/*
	was:
#define BitWidth	(8.0)
	*/
	//#define BitWidth	7.746*0.8
	double BitWidth;

	/* bit height of RAM cell in um */
	/*
	was:
#define BitHeight	(16.0)
	*/
	//#define BitHeight	2*7.746*0.8
	double BitHeight;

	//#define Cout		(0.5e-12)
	double Cout;
	
	int dualVt ;
	int explore;
	/*===================================================================*/

	/*
	* The following are things you probably wouldn't want to change.  
	*/


	static const double BIGNUM;
	
	static const int WAVE_PIPE;
	static const int MAX_COL_MUX;

/* Used to communicate with the horowitz model */

	static const int RISE;
	static const int FALL;
	static const int NCH;
	static const int PCH;


/* Used to pass values around the program */

/*dt: maximum numbers of entries in the 
      caching structures of the tag calculations
*/
	static const int MAX_CACHE_ENTRIES;

	int sequential_access_flag;
	int fast_cache_access_flag;
	int pure_sram_flag; //Changed from static int to just int as value wasn't getting passed through to 
//area function in area.c

	static const double EPSILON; //v4.1: This constant is being used in order to fix floating point -> integer
//conversion problems that were occuring within CACTI. Typical problem that was occuring was
//that with different compilers a floating point number like 3.0 would get represented as either 
//2.9999....or 3.00000001 and then the integer part of the floating point number (3.0) would 
//be computed differently depending on the compiler. What we are doing now is to replace 
//int (x) with (int) (x+EPSILON) where EPSILON is 0.5. This would fix such problems. Note that
//this works only when x is an integer >= 0. 

	/* from basic_circuit.h */
	int powers (int base, int n);
	double logtwo (double x) const;
	double gatecap (double width,double  wirelength);
	double gatecappass (double width,double  wirelength);
	double draincap (double width,int nchannel,int stack);
	double transresswitch (double width,int nchannel,int stack);
	double transreson (double width,int nchannel,int stack);
	double restowidth (double res,int nchannel);
	double horowitz (double inputramptime,double  tf,double  vs1,double  vs2,int rise);

	/* from areadef.h */
	double area_all_datasubarrays;
	double area_all_tagsubarrays;
	double area_all_dataramcells;
	double area_all_tagramcells;
	double faarea_all_subarrays ;
	
	double aspect_ratio_data;
	double aspect_ratio_tag;
	double aspect_ratio_subbank;
	double aspect_ratio_total;
	
	/*
	was: #define Widthptondiff 4.0
	now: is 4* feature size, taken from the Intel 65nm process paper
	*/
	
	//v4.1: Making all constants static variables. Initially these variables are based
	//off 0.8 micron process values; later on in init_tech_params function of leakage.c 
	//they are scaled to input tech node parameters
	
	//#define Widthptondiff 3.2
	double Widthptondiff;
	/* 
	was: #define Widthtrack    3.2
	now: 3.2*FEATURESIZE, i.e. 3.2*0.8
	*/
	//#define Widthtrack    (3.2*0.8)
	double Widthtrack;
	//#define Widthcontact  1.6
	double Widthcontact;
	//#define Wpoly         0.8
	double Wpoly;
	//#define ptocontact    0.4
	double ptocontact;
	//#define stitch_ramv   6.0 
	double stitch_ramv;
	//#define BitHeight16x2 33.6
	//#define BitHeight1x1 (2*7.746*0.8) /* see below */
	double BitHeight1x1;
	//#define stitch_ramh   12.0
	double stitch_ramh;
	//#define BitWidth16x2  192.8
	//#define BitWidth1x1	  (7.746*0.8) 
	double BitWidth1x1;
	/* dt: Assume that each 6-T SRAM cell is 120F^2 and has an aspect ratio of 1(width) to 2(height), than the width is 2*sqrt(60)*F */
	//#define WidthNOR1     11.6
	double WidthNOR1;
	//#define WidthNOR2     13.6
	double WidthNOR2;
	//#define WidthNOR3     20.8
	double WidthNOR3;
	//#define WidthNOR4     28.8
	double WidthNOR4;
	//#define WidthNOR5     34.4
	double WidthNOR5;
	//#define WidthNOR6     41.6
	double WidthNOR6;
	//#define Predec_height1    140.8
	double Predec_height1;
	//#define Predec_width1     270.4
	double Predec_width1;
	//#define Predec_height2    140.8
	double Predec_height2;
	//#define Predec_width2     539.2
	double Predec_width2;
	//#define Predec_height3    281.6
	double Predec_height3;    
	//#define Predec_width3     584.0
	double Predec_width3;
	//#define Predec_height4    281.6
	double Predec_height4;  
	//#define Predec_width4     628.8
	double Predec_width4; 
	//#define Predec_height5    422.4
	double Predec_height5; 
	//#define Predec_width5     673.6
	double Predec_width5;
	//#define Predec_height6    422.4
	double Predec_height6;
	//#define Predec_width6     718.4
	double Predec_width6;
	//#define Wwrite		  1.2
	double Wwrite;
	//#define SenseampHeight    152.0
	double SenseampHeight;
	//#define OutdriveHeight	  200.0
	double OutdriveHeight;
	//#define FAOutdriveHeight  229.2
	double FAOutdriveHeight;
	//#define FArowWidth	  382.8
	double FArowWidth;
	//#define CAM2x2Height_1p	  48.8
	double CAM2x2Height_1p;
	//#define CAM2x2Width_1p	  44.8
	double CAM2x2Width_1p;
	//#define CAM2x2Height_2p   80.8 
	double CAM2x2Height_2p;   
	//#define CAM2x2Width_2p    76.8
	double CAM2x2Width_2p;
	//#define DatainvHeight     25.6
	double DatainvHeight;
	//#define Wbitdropv 	  30.0
	double Wbitdropv;
	//#define decNandWidth      34.4
	double decNandWidth;
	//#define FArowNANDWidth    71.2
	double FArowNANDWidth;
	//#define FArowNOR_INVWidth 28.0  
	double FArowNOR_INVWidth;
	
	//#define FAHeightIncrPer_first_rw_or_w_port 16.0
	double FAHeightIncrPer_first_rw_or_w_port;
	//#define FAHeightIncrPer_later_rw_or_w_port 16.0
	double FAHeightIncrPer_later_rw_or_w_port;
	//#define FAHeightIncrPer_first_r_port       12.0
	double FAHeightIncrPer_first_r_port;
	//#define FAHeightIncrPer_later_r_port       12.0
	double FAHeightIncrPer_later_r_port;
	//#define FAWidthIncrPer_first_rw_or_w_port  16.0 
	double FAWidthIncrPer_first_rw_or_w_port;
	//#define FAWidthIncrPer_later_rw_or_w_port  9.6
	double FAWidthIncrPer_later_rw_or_w_port;
	//#define FAWidthIncrPer_first_r_port        12.0
	double FAWidthIncrPer_first_r_port;
	//#define FAWidthIncrPer_later_r_port        9.6
	double FAWidthIncrPer_later_r_port;
	
	static const int tracks_precharge_p;
	static const int tracks_precharge_nx2;
	static const int tracks_outdrvselinv_p;
	static const int tracks_outdrvfanand_p;
	
	static const double CONVERT_TO_MMSQUARE;
	
	int data_organizational_parameters_valid(int B,int A,int C,int Ndwl,int Ndbl,double Nspd,char assoc,double NSubbanks);
	int tag_organizational_parameters_valid(int B,int A,int C,int Ntwl,int Ntbl,int Ntspd,char assoc,double NSubbanks);
	void area_subbanked (int baddr,int b0,int RWP,int ERP,int EWP,int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,
			double NSubbanks,parameter_type *parameters,Cacti4_2::area_type *result_subbanked,arearesult_type *result);


	/* from area.c */
	double
	logtwo_area (double x) const;
	Cacti4_2::area_type
	inverter_area (double Widthp,double Widthn);
	Cacti4_2::area_type
	subarraymem_area (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int RWP,int ERP,int EWP,int NSER,double techscaling_factor);	/* returns area of subarray */
	Cacti4_2::area_type
	decodemem_row (int C,int B,int A,int Ndbl,double Nspd,int Ndwl,int RWP,int ERP,int EWP);	/* returns area of post decode */
	Cacti4_2::area_type
	predecode_area (double noof_rows,int RWP,int ERP,int EWP);
	Cacti4_2::area_type
	postdecode_area (int noof_rows,int RWP,int ERP,int EWP);
	Cacti4_2::area_type
	colmux (int Ndbl,double Nspd,int RWP,int ERP,int EWP,int NSER);	/* gives the height of the colmux */
	Cacti4_2::area_type
	precharge (int Ndbl,double Nspd,int RWP,int ERP,int EWP,int NSER);
	Cacti4_2::area_type
	senseamp (int Ndbl,double Nspd,int RWP,int ERP,int EWP,int NSER);
	Cacti4_2::area_type
	subarraytag_area (int baddr,int C,int B,int A,int Ntdbl,int Ntdwl,int Ntspd,double NSubbanks,int RWP,int ERP,
			int EWP,int NSER,double techscaling_factor);	/* returns area of subarray */
	Cacti4_2::area_type
	decodetag_row (int baddr,int C,int B,int A,int Ntdbl,int Ntspd,int Ntdwl,double NSubbanks,int RWP,int ERP,int EWP);	/* returns area of post decode */
	Cacti4_2::area_type
	comparatorbit (int RWP,int ERP,int EWP);
	Cacti4_2::area_type
	muxdriverdecode (int B,int b0,int RWP,int ERP,int EWP);
	Cacti4_2::area_type
	muxdrvsig (int A,int B,int b0);		/* generates the 8B/b0*A signals */
	Cacti4_2::area_type
	datasubarray (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int RWP,int ERP,int EWP,int NSER,
			double techscaling_factor);
	Cacti4_2::area_type
	datasubblock (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int SB,int b0,int RWP,int ERP,int EWP,int NSER,
			double techscaling_factor);
	Cacti4_2::area_type
	dataarray (int C,int B,int A,int Ndbl,int Ndwl,double Nspd,int b0,int RWP,int ERP,int EWP,int NSER,
		double techscaling_factor);
	Cacti4_2::area_type
	tagsubarray (int baddr,int C,int B,int A,int Ndbl,int Ndwl,double Nspd,double NSubbanks,int RWP,int ERP,int EWP,int NSER,
			double techscaling_factor);
	Cacti4_2::area_type
	tagsubblock (int baddr,int C,int B,int A,int Ndbl,int Ndwl,double Nspd,double NSubbanks,int SB,int RWP,int ERP,int EWP,
			int NSER,double techscaling_factor);
	Cacti4_2::area_type
	tagarray (int baddr,int C,int B,int A,int Ndbl,int Ndwl,double Nspd,double NSubbanks,int RWP,int ERP,int EWP,int NSER,
		double techscaling_factor);
	void
	area (int baddr,int b0,int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,double NSubbanks,parameter_type *parameters,
		arearesult_type *result);
	Cacti4_2::area_type
	fadecode_row (int C,int B,int Ndbl,int RWP,int ERP,int EWP);	/*returns area of post decode */
	Cacti4_2::area_type
	fasubarray (int baddr,int C,int B,int Ndbl,int RWP,int ERP,int EWP,int NSER,double techscaling_factor);	/* returns area of subarray */
	Cacti4_2::area_type
	faarea (int baddr,int b0,int C,int B,int Ndbl,int RWP,int ERP,int EWP,int NSER,double techscaling_factor);
	void
	fatotalarea (int baddr,int b0,int Ndbl,parameter_type *parameters,arearesult_type *faresult);

	/* from leakage.h */

	/* .18 technology */
	
	/* Common for all the Technology */
	static const double Bk;  /* Boltzman Constant */
	static const double Qparam;    /* FIXME     */
	static const double Eox;
	
	static const int No_of_Samples;
	static const double Tox_Std;
	static const double Tech_Std;
	static const double Vdd_Std;
	static const double Vthn_Std;
	static const double Vthp_Std;
	
	double Tkelvin;
	double process_tech;
	double tech_length0;
	double M0n ;      /* Zero Bias Mobility for N-Type */
	double M0p  ;     /* Zero Bias Mobility for P-Type */
	double Tox ;
	double Cox ;      /* Gate Oxide Capacitance per unit area */
	double Vnoff0  ;  /* Empirically Determined Model Parameter for N-Type */
							/* FIX ME */
	double Vpoff0  ;  /* Empirically Determined Model Parameter for P-Type */
	double Nfix ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
	double Pfix ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
	double Vthn  ;    /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
	double Vthp  ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
	double Vnthx  ;   /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
	double Vpthx ;    /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
	double Vdd_init ; /* Default Vdd. Can be Changed in leakage.c */
	double Volt0  ;
	double Na  ;      /* Empirical param for the Vdd fit */
	double Nb ;       /* Empirical param for the Vdd fit */
	double Pa  ;      /* Empirical param for the Vdd fit */
	double Pb  ;      /* Empirical param for the Vdd fit */
	double NEta  ;    /* Sub-threshold Swing Co-efficient N-Type */
	double PEta   ;   /* Sub-threshold Swing Co-efficient P-Type */
	
	double L_nmos_d  ;     /* Adjusting Factor for Length */
	double Tox_nmos_e  ;   /* Adjusting Factor for Tox */
	double L_pmos_d ;    /* Adjusting Factor for Length */
	double Tox_pmos_e ;  /* Adjusting Factor for Tox */
	
	/* gate Vss */
	double Vth0_gate_vss ;
	double aspect_gate_vss;
	
	/*drowsy cache*/
	double Vdd_low ;
	
	/*RBB*/
	double k1_body_n ;
	double k1_body_p ;
	double vfi ;
	
	double VSB_NMOS ;
	double VSB_PMOS ;
	/* dual VT*/
	double Vt_cell_nmos_high ;
	double Vt_cell_pmos_high ;
	double Vt_bit_nmos_low ;
	double Vt_bit_pmos_low ;
	
	/* Gate lekage for 70nm */
	double  nmos_unit_leakage ;
	double a_nmos_vdd ;
	double b_nmos_t;
	double c_nmos_tox;
	
	double pmos_unit_leakage;
	double a_pmos_vdd ;
	double b_pmos_t ;
	double c_pmos_tox ;
	
	
	/* Precalculated Values for leakage */
	double precalc_Vnthx, precalc_Vpthx;
	double precalc_Vthermal,precalc_inv_nVthermal,precalc_inv_pVthermal;
	
	double precalc_nparam2,precalc_pparam2;
	
	double precalc_nparamf, precalc_pparamf;
	double precalc_nparaml, precalc_pparaml;
	
	int have_leakage_params;
	
	/* Technology Length */
	
	double nmos_ileakage(double aspect_ratio,double Volt,double Vth0,double Tkelvin,double tox0);
	
	double pmos_ileakage(double aspect_ratio,double Volt,double Vth0,double Tkelvin,double tox0);
	
	double nmos_ileakage_var(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0, double tech_length);
	
	double pmos_ileakage_var(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0, double tech_length);
	
	double box_mueller(double std_var, double value);
	
	double simplified_cmos_leakage(double naspect_ratio,double paspect_ratio, double nVth0, double pVth0,
								double *norm_nleak, double *norm_pleak);

	double optimized_simplified_cmos_leakage(double naspect_ratio,double paspect_ratio, double nVth0, double pVth0,
							   double * nleak, double * pleak);
	
	double simplified_nmos_leakage(double naspect_ratio, double nVth0);
	double simplified_pmos_leakage(double paspect_ratio, double pVth0);
	
	void precalc_leakage_params(double Volt,double Tkelvin,double tox0, double tech_length);
	
	void init_tech_params(double tech);
	
	void init_tech_params_default_process();//v4.1


	/* from time.c */

	int force_tag, force_tag_size;


	void reset_tag_device_widths();
	void reset_data_device_widths();
	void compute_device_widths(int C,int B,int A,int fullyassoc, int Ndwl,int Ndbl,double Nspd);
	void compute_tag_device_widths(int C,int B,int A,int Ntspd,int Ntwl,int Ntbl,double NSubbanks);
	double cmos_ileakage(double nWidth, double pWidth, double nVthresh_dual, double nVthreshold, double pVthresh_dual, double pVthreshold);
	void reset_powerDef(powerDef *power);
	void mult_powerDef(powerDef *power, int val);
	void mac_powerDef(powerDef *sum,powerDef *mult, int val);
	void copy_powerDef(powerDef *dest, powerDef source);
	void copy_and_div_powerDef(powerDef *dest, powerDef source, double val);
	void add_powerDef(powerDef *sum, powerDef a, powerDef b);
	double objective_function(double delay_weight, double area_weight, double power_weight,
						  double delay,double area,double power);
	void subbank_routing_length (int C,int B,int A,
						char fullyassoc,
						int Ndbl,double Nspd,int Ndwl,int Ntbl,int Ntwl,int Ntspd,
						double NSubbanks,
						double *subbank_v,double *subbank_h);
	void subbank_dim (int C,int B,int A,
			 char fullyassoc,
			 int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,
			 double NSubbanks,
			 double *subbank_h,double *subbank_v);
	void subbanks_routing_power (char fullyassoc,
							int A,
							double NSubbanks,
							double *subbank_h,double *subbank_v,powerDef *power);
	double address_routing_delay (int C,int B,int A,
						char fullyassoc,
						int Ndwl,int Ndbl,double Nspd,int Ntwl,int Ntbl,int Ntspd,
						double *NSubbanks,double *outrisetime,powerDef *power);
	double decoder_delay(int C, int B,int A,int Ndwl,int Ndbl,double Nspd,double NSubbanks,
				double *Tdecdrive,double *Tdecoder1,double *Tdecoder2,double inrisetime,double *outrisetime, int *nor_inputs,powerDef *power);
	double decoder_tag_delay(int C, int B,int A,int Ntwl,int Ntbl, int Ntspd,double NSubbanks,
				double *Tdecdrive, double *Tdecoder1, double *Tdecoder2,double inrisetime,double *outrisetime, int *nor_inputs,powerDef *power);
	double fa_tag_delay (int C,int B,int Ntwl,int Ntbl,int Ntspd,
				double *Tagdrive, double *Tag1, double *Tag2, double *Tag3, double *Tag4, double *Tag5, double *outrisetime,
				int *nor_inputs,
				powerDef *power);
	double wordline_delay (int C, int B,int A,int Ndwl, int Ndbl, double Nspd,
					double inrisetime,
					double *outrisetime,powerDef *power);
	double wordline_tag_delay (int C,int B,int A,
							int Ntspd,int Ntwl,int Ntbl,double NSubbanks,
							double inrisetime,double *outrisetime,powerDef *power);
	double bitline_delay (int C,int A,int B,int Ndwl,int Ndbl,double Nspd,
				double inrisetime,
				double *outrisetime,powerDef *power,double Tpre);
	double bitline_tag_delay (int C,int A,int B,int Ntwl,int Ntbl,int Ntspd,
					double NSubbanks,double inrisetime,
					double *outrisetime,powerDef *power,double Tpre);
	double sense_amp_delay (int C,int B,int A,int Ndwl,int Ndbl,double Nspd, double inrisetime,double *outrisetime, powerDef *power);
	double sense_amp_tag_delay (int C,int B,int A,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,double inrisetime, double *outrisetime, powerDef *power);
	double compare_time (int C,int A,int Ntbl,int Ntspd,double NSubbanks,double inputtime,double *outputtime,powerDef *power);
	double mux_driver_delay (int Ntbl,int Ntspd,double inputtime,
		  double *outputtime,double wirelength);
	void precalc_muxdrv_widths(int C,int B,int A,int Ndwl,int Ndbl,double Nspd,double * wirelength_v,double * wirelength_h);
	double mux_driver_delay_dualin (int C,int B,int A,int Ntbl,int Ntspd,double inputtime1,
			 double *outputtime,double wirelength_v,double wirelength_h,powerDef *power);
	double senseext_driver_delay(int A,char fullyassoc,
					double inputtime,double *outputtime, double wirelength_sense_v,double wirelength_sense_h, powerDef *power);
	double total_out_driver_delay (int C,int B,int A,char fullyassoc,int Ndbl,double Nspd,int Ndwl,int Ntbl,int Ntwl,
			int Ntspd,double NSubbanks,double inputtime,double *outputtime,powerDef *power);
	double valid_driver_delay (int C,int B,int A,char fullyassoc,int Ndbl,int Ndwl,double Nspd,int Ntbl,int Ntwl,int Ntspd,
		    double *NSubbanks,double inputtime,powerDef *power);
	double half_compare_delay(int C,int B,int A,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,double inputtime,double *outputtime,powerDef *power);
	double dataoutput_delay (int C,int B,int A,char fullyassoc,int Ndbl,double Nspd,int Ndwl,
		  double inrisetime,double *outrisetime,powerDef *power);
	double selb_delay_tag_path (double inrisetime,double *outrisetime,powerDef *power);
	double precharge_delay (double worddata);
	void calc_wire_parameters(parameter_type *parameters);

	/* from io.h */

	int input_data(int argc,char *argv[]) const;
	void output_time_components(result_type *result,parameter_type *parameters) const;
	void output_area_components(arearesult_type *arearesult, parameter_type *parameters) const;
};

} // end of namespace power
} // end of namespace service
} // end of namespace unisim

#endif
