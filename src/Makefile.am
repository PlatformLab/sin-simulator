AM_CPPFLAGS = $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS)

common_source = abstract_user.hh noop_user.hh owner_user.hh owner_user.cc basic_user.hh basic_user.cc \
	market_emulator.hh market_emulator.cc market.hh market.cc \
	slot.hh slot.cc market_event.hh

bin_PROGRAMS = sin
sin_SOURCES = sin.cc $(common_source)
