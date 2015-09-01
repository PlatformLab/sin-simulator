AM_CPPFLAGS = $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS)

common_source = abstract_user.hh owner_user.hh \
    schedule.hh interval.hh pretty_print.hh \
    link.hh flow.hh \
	market_simulator.hh market_simulator.cc market.hh market.cc \
    flow_completion_time_user.hh 

bin_PROGRAMS = sin
sin_SOURCES = sin.cc $(common_source)
