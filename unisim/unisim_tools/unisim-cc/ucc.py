#!/usr/bin/python

import sys

from configuration import Configuration
from optparse import OptionParser
from system import System
from simulator import Simulator
from module import Module
from library import Library
import os
import re
from os.path import join
from xml.dom import minidom
import generator

def checkFileType(file, rootdir, g_system):
	f = open(file, 'r')
	dom = minidom.parse(f)
	# check for modules
	if dom.documentElement.tagName == "Module":
		if not dom.documentElement.hasAttribute('name'):
			print "ERROR: found module without name when building the database"
			print "       filename = %s" % (file,)
			sys.exit(-1)
		module_name = dom.documentElement.getAttribute('name')
		if module_name in g_system.module_db:
			print "ERROR: module name collision"
			print "       file 1 = %s" % (g_system.module_db[module_name],)
			print "       file 2 = %s" % (file,)
			sys.exit(-1)
		g_system.module_db[module_name] = file
		return
	# TODO check for libraries
	if dom.documentElement.tagName == "Library":
		if not dom.documentElement.hasAttribute('name'):
			print "ERROR: found library without name when building the database"
			print "       filename = %s" % (file,)
			sys.exit(-1)
		lib_name = dom.documentElement.getAttribute('name')
		if lib_name in g_system.lib_db:
			print "ERROR: library name collision"
			print "       file 1 = %s" % (g_system.lib_db[lib_name],)
			print "       file 2 = %s" % (file,)
			sys.exit(-1)
		g_system.lib_db[lib_name] = file
		return

def initDatabase(g_system):
	for rootdir in g_system.configuration.dirs:
		for root, dirs, files in os.walk(rootdir):
			for file in files:
				if re.match(r".*\.xml$", file):
					file = os.path.join(root, file)
					checkFileType(file, rootdir, g_system)
	# for all the files in the database parse them
	for k,v in g_system.module_db.iteritems():
		module = Module(v, g_system)
		g_system.module_db[k] = module
	for k,v in g_system.lib_db.iteritems():
		lib = Library(v, g_system)
		g_system.lib_db[k] = lib

def main(argv):
	g_system = System()

	usage = "usage: %prog [options] <simulator.xml>"
	op = OptionParser(usage=usage, version="%prog 0.1")
	op.add_option("-c", "--config", dest="config_filename",
			help="configuration file")
	op.add_option("-v", "--verbose", action="store_true",
			help="verbose")
	(opts,args) = op.parse_args()
	if len(args) != 1:
		op.print_help()
		op.error("incorrect number of arguments" + str(args))
	
	# check if we run in verbose mode
	if opts.verbose:
		print "verbose operation"
		g_system.verbose = True
	else:
		g_system.verbose = False

	# parse the configuration file
	if opts.config_filename:
		g_system.configuration = Configuration(opts.config_filename)
	if g_system.verbose:
		print g_system.configuration.toString()
	
	# initialize the module database
	g_system.module_db = {}
	g_system.lib_db = {}
	initDatabase(g_system)

	if g_system.verbose:
		if len(g_system.module_db) != 0:
			print "Modules found:"
			for k,v in g_system.module_db.iteritems():
				print "%s" % (v.toString("  "),)
		if len(g_system.lib_db) != 0:
			print "Libraries found:"
			for k,v in g_system.lib_db.iteritems():
				print "%s" % (v.toString("  "),)


	# create the simulator
	sim = Simulator(args[0], g_system)
	if g_system.verbose:
		print sim.toString("")

	generator.generateCode(sim, g_system)

	sys.exit(-1)

	# parse files in the g_system.file_db
	file_db_finished = False
	file_db_done = []
	modules = {}
	while not file_db_finished:
		file_db_finished = True
		for file in g_system.file_db:
			if file_db_finished:
				if file not in file_db_done:
					file_db_finished = False
					file_to_parse = file
		if not file_db_finished:
			module = Module(file_to_parse, g_system)
			modules[module.name] = module
			file_db_done.append(file_to_parse)

	if g_system.verbose:
		for k, module in modules.iteritems():
			print module.toString("")

if __name__ == "__main__":
	main(sys.argv)
