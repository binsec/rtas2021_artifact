
#include <csignal>
#include <unisim/kernel/clm/engine/unisim.h>
//#include <unisim/kernel/clm/engine/unisim_lse.h>

///////////// For Emulator ... //////////////////

#include <stdio.h>
//#include <getopt.h>

#include "parameters_d32.hh"
#include "simulator_ideal.hh"

int main(int argc, char **argv, char **envp)
{ 
  command_line.add_flag("display-signals", "display signals at each cycle");



  command_line.add_option("max:inst", "<count>", "execute <count> instructions then exit");
  //  command_line.add_option("fastforward", "number of instructions to skip before starting the simulation");
  command_line.add_option("dump-latex", "<file>", "Draw the architecture");
  command_line.add_option("dump-machine-description", "<file>", "Dump the machine description in <file> then exits.");
  command_line.add_option("dump-statistics", "<file>", "Dump the statistics at the end of the simulation to <file>.");
  command_line.add_option("load-checkpoint", "<file>", "Load a checkpoint from <file>");
  //redirection
  command_line.add_option("stdin", "<file>", "Use <file> as stdin for the simulated program");
  command_line.add_option("stdout", "<file>", "Use <file> as stdout for the simulated program");
  command_line.add_option("stderr", "<file>", "Use <file> as stderr for the simulated program");
  //eoredirection
  command_line.add_extra("program", "ELF32 statically linked Linux binary");
  command_line.add_extra_opt("program arguments", "ELF32 binary parameters");
  command_line.parse(argc,argv);
  
  /* Variables from Powerpc tlm main.cc */
  /*
    int c;


	bool estimate_power = false;


	uint64_t maxinst = 0; // maximum number of instruction to simulate
	char *logger_filename = 0;
	bool logger_zip = false;
	bool logger_error = false;
	bool logger_out = false;
	bool logger_on = false;
	bool logger_messages = false;
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	uint32_t tech_node = 130; // in nm
	double cpu_ipc = 1.0; // in instructions per cycle
	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	uint32_t mem_cycle_time = fsb_cycle_time;
  */



  ///////////////////////////////////////// "From Emulator" Start ///////////////////////
  int c;


  bool kernel_mode = false;
  uint32_t video_refresh_period = 1000;

  const char *device_tree_filename = "device_tree.xml";

  uint64_t maxinst = 0;
  bool check_sig=false;
  
  if(command_line["max:inst"])
    { maxinst = strtoull(command_line["max:inst"], 0, 0);
    }

  /*
  const char *filename = command_line[0];
  unsigned int sim_argc = command_line.count();
  */
  
  GeneratedSimulator s;
  
  if(command_line["dump-machine-description"])
    { s.dump_machine_description(command_line["dump-machine-description"]);
    exit(0);
    }
  
  if(command_line.count()==0)
    { command_line.help();
    return 0;
    }
  
  if(command_line["dump-latex"])
  { s.dump_latex(command_line["dump-latex"]);
    exit(0);
  }




  ///////////////////////////////////////// "From Emulator"  End ///////////////////////
  //  GeneratedSimulator s;
  unisim_port::check_connections();
  //#include "cmp-nosnoop.uni.inc.init.cxx"
  

  if(kernel_mode)
    {
      cerr << "<<< Error: kernel mode is no more supported, until it will be implement by none TLM modules. >>>" << endl;
      //      help(argv[0]);
      return 0;
    }
  else
    {
      
      s.ServiceConnection(
			  /*
		      filename,
		      argc,


		      kernel_mode,
		      video_refresh_period,

		      device_tree_filename,

		      maxinst
			  */
		      );
    }

  signal(SIGINT,GeneratedSimulator::at_ctrlc);
#if !defined(WIN32) && !defined(WIN64)
  signal(SIGTSTP,GeneratedSimulator::at_ctrlz);
#endif


  while(!unisim_terminated)
  { 
    fsc_phase();
    fsc_phase();
    //#include "cmp-nosnoop.uni.inc.eoc.cxx"
#ifdef DD_UNSAFE_OPTIMIZATION
    ;
#else
    if(s._unknown_display) unisim_port::check_knowness(s.stream_knowness(),s._unknown_fatal);
    //if(s._signal_display) unisim_port::check_signals(s.stream_knowness(),s._unknown_fatal);
#endif

#ifdef DD_DISPLAY_SIGNALS    
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      if (command_line["display-signals"])
      { cerr << "---------------------------- DISPLAY 1 -----------------------------------" << endl;
	unisim_port::check_signals(s.stream_knowness(),s._unknown_fatal);
      }
    }
#endif
    if(s.is_terminated())
      {
	//	cerr << "Simulation ended at cycle : " << timestamp() << endl;
	/*
	for(int cfg=0; cfg<nConfig; cfg++)
	  {
	    cerr << "       Cpu[" << cfg << "] ended at cycle : " << check_emulator[cfg]->end_at_cycle << endl;
	  }
	*/
	s.printend();
	cerr << "Simulation ended at cycle : " << timestamp() << endl;
	terminate_now();
      }
  }
  ServiceManager::DumpStatistics(cerr);

  /* 
  if(command_line["dump-statistics"])
  { string filename = command_line["dump-statistics"];
    if(filename=="-")
    { StatisticService::dump_statistics(cerr);
    }
    else
    { ofstream stats(filename.c_str(),ios_base::trunc);
      StatisticService::dump_statistics(stats);
      stats.close();
    }
  }
  */


 return 0;
} //main()
