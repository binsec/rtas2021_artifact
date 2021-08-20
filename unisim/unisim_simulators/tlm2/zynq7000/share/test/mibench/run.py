import sys
import io
import shlex
import subprocess
import os
import os.path
import shutil
import filecmp
import time
from  xml.etree import ElementTree
from optparse import OptionParser


class XUnit:
	def __init__(self, name, timestamp):
		self.root = ElementTree.Element("testsuite")
		self.root.attrib['name'] = name
		self.root.attrib['timestamp'] = str(timestamp)
		self.errors = 0
		self.failures = 0
		self.tests = 0
		self.time = 0.0
		return

	def add_testcase(self, classname, name, time, error = None):
		el = ElementTree.Element("testcase")
		el.attrib['classname'] = classname
		el.attrib['name'] = name
		el.attrib['time'] = str(time)
		if error is not None:
			el.text = error
			self.failures = self.failures + 1
		self.root.append(el)
		self.tests = self.tests + 1
	
	def write(self, time, filename="xunit.xml"):
		self.root.attrib['errors'] = str(self.errors)
		self.root.attrib['failures'] = str(self.failures)
		self.root.attrib['tests'] = str(self.tests)
		self.root.attrib['time'] = str(self.time)
		ElementTree.ElementTree(self.root).write(filename, "utf-8")

def create_run_directory(benchmark_path, output_path, config_file):
	run_directory = "run/" + output_path
	output_directory = "output/" + output_path
	# os.makedirs(run_directory)
	shutil.rmtree(output_directory, ignore_errors = True)
	shutil.rmtree(run_directory, ignore_errors = True)
	os.makedirs(output_directory)
	shutil.copytree(benchmarks_path + "/" + benchmark_path, run_directory)
	shutil.copy(config_path + "/" + benchmark_path + "/" + config_file, run_directory + "/config.xml")
	return (run_directory, output_directory)

def delete_systemc_from_output(filename):
	# remove stupid systemc line from output
	shutil.move(filename, filename + ".sed")
	command = "sed '/SystemC: simulation stopped by user./d' " + filename + ".sed"
	args = shlex.split(command)
	stdout_file = open(filename, "w")
	subprocess.call(args, stdout = stdout_file)
	os.remove(filename + ".sed")

def run_benchmark(benchmark_path, output_path, config_file):
	(run_directory, output_directory) = create_run_directory(benchmark_path, output_path, config_file)
	command = simulator + " -c config.xml"
	args = shlex.split(command)
	stdout_file = open(output_directory + "/stdout.txt", "w")
	stderr_file = open(output_directory + "/stderr.txt", "w")
	t_init = time.time()
	print("args = ", args)
	print("t_init = ", t_init)
	subprocess.call(args, cwd = run_directory, stdout = stdout_file, stderr = stderr_file)
	t_end = time.time()
	print("t_end = ", t_end)
	stdout_file.close()
	stderr_file.close()
	delete_systemc_from_output(output_directory + "/stdout.txt")
	return (run_directory, output_directory, t_end - t_init)

##################################################################
# Automotive tests                                               #
##################################################################

# basicmath
def automotive_basicmath():
	print("Launching mibench/automotive/basicmath (small).")
	(run_directory, output_directory, run_time) = run_benchmark("mibench/automotive/basicmath", "mibench/automotive/basicmath/small", "config_small.xml")
	print("--> ", run_time, " seconds")
	error = ""
	if filecmp.cmp(output_directory + "/stdout.txt", config_path + "/mibench/automotive/basicmath/output_small.txt"):
		print("Executed successfully mibench/automotive/basicmath (small). ")
		shutil.rmtree(run_directory)
	else:
		print("Error executing mibench/automotive/basicmath (small) !!!!!")
		error = "Did not work"
	
	xunit.add_testcase("automotive", "basicmath_small", run_time, error)
	
	print("Launching mibench/automotive/basicmath (large).")
	(run_directory, output_directory, run_time) = run_benchmark("mibench/automotive/basicmath", "mibench/automotive/basicmath/large", "config_large.xml")
	print("--> ", run_time, " seconds")
	error = ""
	if filecmp.cmp(output_directory + "/stdout.txt", config_path + "/mibench/automotive/basicmath/output_large.txt"):
		print("Executed successfully mibench/automotive/basicmath (large). ")
		shutil.rmtree(run_directory)
	else:
		print("Error executing mibench/automotive/basicmath (large) !!!!!")
		error = "Did not work"
	
	xunit.add_testcase("automotive", "basicmath_large", run_time, error)

# BEGIN: small trick to force flush on sys.stdout after each print
class flushfile(io.TextIOWrapper):
	def __init__(self, f):
		self.f = f

	def write(self, x):
		self.f.write(x)
		self.f.flush()

sys.stdout = flushfile(sys.stdout)
# END

usage = """usage: %prog [options]
NOTE: options -b, -s and -c must be defined"""
parser = OptionParser(usage = usage)
parser.add_option("-b", "--benchmarks", dest = "benchmarks_path",
	default = None,
	help = "Directory containing the compiled mibench benchmarks.", metavar = "PATH")
parser.add_option("-s", "--simulator", dest = "simulator",
	default = None,
	help = "The simulator to use (with the full path if required).", metavar = "FILE")
parser.add_option("-c", "--config", dest = "config_path",
	default = None,
	help = "Directory containing the simulator configuration for each benchmark.", metavar = "PATH")

(options, args) = parser.parse_args()
if not options.benchmarks_path:
	parser.print_help()
	parser.error("missing directory containing the compiled mibench benchmarks")
if not options.simulator:
	parser.print_help()
	parser.error("missing simulator to use")
if not options.config_path:
	parser.print_help()
	parser.error("missing directory containing the simulator configuration for each benchmark")

benchmarks_path = os.path.abspath(options.benchmarks_path)
simulator = os.path.abspath(options.simulator)
config_path = os.path.abspath(options.config_path)
xunit = XUnit("mibench", time.time())
t_init = time.time()
automotive_basicmath()
t_end = time.time()
xunit.write(t_end - t_init)
# automotive_basicmath("/opt/unisim/benchmarks", "/home/gracia/Developer/unisim/armemu/build/bin/unisim-armemu")
