AM_CPPFLAGS = $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS)

common_source = abstract_user.hh owner_user.hh owner_user.cc \
    noop_user.hh brute_force_user.hh brute_force_user.cc \
	market_emulator.hh market_emulator.cc market.hh market.cc \
	slot.hh slot.cc market_event.hh shortest_remaining_time_first.hh

bin_PROGRAMS = sin
sin_SOURCES = sin.cc $(common_source)
