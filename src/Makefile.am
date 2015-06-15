AM_CPPFLAGS = $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS)

common_source = abstract_user.hh owner_user.hh owner_user.cc \
    noop_user.hh \
	market_emulator.hh market_emulator.cc market.hh market.cc \
	slot.hh slot.cc market_event.hh shortest_remaining_time_first.hh \
    round_robin.hh pretty_print.hh pretty_print.cc flow_completion_time_user.hh flow_completion_time_user.cc

bin_PROGRAMS = sin
sin_SOURCES = sin.cc $(common_source)
