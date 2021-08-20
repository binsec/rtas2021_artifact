/* displaying an information message */
logger << DebugInfo << "This is an information message" << EndDebugInfo;
 
/* displaying an information message using   */
/* the EndDebug keyword to close the message */
logger << DebugInfo << "This is an information message" << EndDebug;
 
/* displaying a warning message written in multiple steps */
logger << DebugWarning << "This is the start of a warning message" << endl;
logger << "This is the end of the warning message." << EndDebug;
 
/* displaying an error message using variables */
unsigned int error = 25;
logger << DebugError << "This is an error message using variable \"error\" with value "
       << error << EndDebug;
