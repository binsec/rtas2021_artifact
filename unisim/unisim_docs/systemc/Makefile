INTRODUCTION= \
	introduction/intro_main.tex  introduction/really.tex \
	introduction/technically.tex introduction/install.txt \
	introduction/conclusion.tex introduction/fibonacci_annotated.cc \
	introduction/fibonacci.cc \
	introduction/fibonacci_annotated.cc \
	introduction/fibonacci_annotated_presentation_1.cc \
	introduction/fibonacci_annotated_presentation_2.cc 

CYCLE= \
	systemc_cycle/main.tex \
	systemc_cycle/systemc_example.cc \
	systemc_cycle/schedule_example.pdf \
	systemc_cycle/systemc_example.pdf \
	systemc_cycle/dependency_graph.pdf \
	systemc_cycle/threads.pdf \
	systemc_cycle/method.pdf \
	systemc_cycle/connecting.cc \
	systemc_cycle/fibonacci.pdf \
	systemc_cycle/fibonacci_with_wait_states.pdf \
	systemc_cycle/basic_wait.cc \
	systemc_cycle/advanced_wait.cc \
	systemc_cycle/module_definition.cc \
	systemc_cycle/port_access.cc \
	systemc_cycle/port_signal_binding.cc \
	systemc_cycle/sc_method_definition.cc \
	systemc_cycle/sc_thread_definition.cc \
	systemc_cycle/sc_cthread_definition.cc \
	systemc_cycle/dynamic_sensitivity.pdf \
	systemc_cycle/channel_example.cc

TLM= \
	tlm/main.tex \
	tlm/introduction.tex \
	tlm/interfaces.tex \
	tlm/b_transport.cc tlm/nb_transport.cc \
	tlm/blocking_combined_interfaces.hh     tlm/tlm_generic_payload_types.hh \
	tlm/combined_interfaces.hh              tlm/tlm_response_status.hh \
	tlm/nonblocking_combined_interfaces.hh  tlm/tlm_socket_b.hh \
	tlm/quantum_keeper_example.hh           tlm/tlm_socket_initiator.hh \
	tlm/quantum_keeper.hh                   tlm/tlm_socket_nb.hh \
	tlm/tlm_command.hh                      tlm/tlm_socket_target.hh \
	tlm/tlm_dmi.hh                          tlm/tlm_socket_types.hh \
	tlm/tlm_generic_payload.hh              tlm/tlm_transport_dbg_if.hh \
	tlm/sockets.tex \
	tlm/payload.tex \
	tlm/quantum.tex \
	tlm/charts.tex \
	tlm/others.tex \
	tlm/exercise.tex

all: presentation article
	
presentation: main.beamer.pdf

article: main.article.pdf

main.article.pdf: main.article.tex main.tex \
	$(INTRODUCTION) \
	$(CYCLE) \
	$(TLM)
	rubber -d main.article

main.beamer.pdf: main.beamer.tex main.tex \
	$(INTRODUCTION) \
	$(CYCLE) \
	$(TLM)
	rubber -d main.beamer

clean:
	rubber -d --clean main.beamer
	rubber -d --clean main.article

#	rm main.beamer.out main.beamer.pdf main.article.log main.beamer.log main.beamer.nav \
#		main.article.aux main.beamer.toc main.beamer.aux main.beamer.snm main.article.pdf

