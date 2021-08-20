# Added some signals to view
set my_sig_list [list]
#lappend my_sig_list "HARDWARE.BP_FREEZE"
set num_added [ gtkwave::addSignalsFromList $my_sig_list ]
# Adjust zoom
gtkwave::setZoomFactor -34
