# -*- coding: utf-8 -*-
from optparse import OptionParser
from puscomp.config import ConfigClass as ConfigClass
from puscomp.repository import RepositoryClass as RepositoryClass
# from puscomp.simulator import SimulatorClass as SimulatorClass
from puscomp.builder import Builder as Builder
import sys

# puscomp v0.1
print("puscomp (Python UniSim Compiler) v0.1")
print("Author: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)")

usage = """usage: %prog [options] [target]
NOTE: options -c and -o must be defined"""
parser = OptionParser(usage = usage)
parser.add_option("-c", "--config", dest = "config_filename",
	default = None,
	help = "puscomp configuration file to use", metavar = "FILE")
# parser.add_option("-s", "--simulator", dest = "simul_filename",
# 	default = None,
# 	help = "simulator description file to use", metavar = "FILE")
parser.add_option("-o", "--output", dest = "output_path",
	default = None,
	help = "location of the resulting output", metavar = "PATH")
parser.add_option("-v", "--verbose", dest = "verbose",
	action = "store_true",
	default = False,
	help = "verbose operation mode")
parser.add_option("-l", "--link", dest = "do_link",
	action = "store_true",
	default = False,
	help = "instead of copying files create links")

(options, args) = parser.parse_args()
if not options.config_filename:
	parser.print_help()
	parser.error("missing configuration file")
# if not options.simul_filename:
# 	parser.print_help()
# 	parser.error("missing simulation description file")
if not options.output_path:
	parser.print_help()
	parser.error("missing output path")
if len(args) == 0:
	parser.print_help()
	parser.error("missing target to build")
if len(args) != 1:
	parser.print_help()
	parser.error("too many target defined, just one is enough")

# read configuration

print("Reading configuration file...")
config = ConfigClass(options.config_filename, options.do_link)
if config.getStatus():
	print("... DONE reading configuration file.")
else:
	print("Stopping puscomp.")
	sys.exit()
if options.verbose:
	print("============================")
	print("Configuration:")
	config.dump(" + ")
	print("============================")
print("Populating repository...")
rep = RepositoryClass(config)
if rep.getStatus():
	print("... DONE populating repository.")
else:
	print("Stopping puscomp.")
	sys.exit()
if options.verbose:
	print("============================")
	print("Repository:")
	rep.dump(" + ")
	print("============================")
print("Building ...")
build = Builder(args[0], rep)
if build.getStatus():
	print("... DONE building.")
else:
	print("Stopping puscomp.")
	sys.exit()
if options.verbose:
	print("============================")
	print("Builder:")
	build.dump(" + ")
	print("============================")
print("Generating output...")
build.generate(config, options.output_path)
if build.getStatus():
	print("... DONE generating output.")
else:
	print("Stopping puscomp.")
	sys.exit()

print("Bye.")

