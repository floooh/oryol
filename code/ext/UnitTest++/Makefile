CXX = g++
CXXFLAGS ?= -g -Wall -W -ansi # -pedantic
LDFLAGS ?= 
SED = sed
MV = mv
RM = rm

.SUFFIXES: .o .cpp

lib = libUnitTest++.a
test = TestUnitTest++

src = src/AssertException.cpp \
	src/Test.cpp \
	src/Checks.cpp \
	src/TestRunner.cpp \
	src/TestResults.cpp \
	src/TestReporter.cpp \
	src/TestReporterStdout.cpp \
	src/ReportAssert.cpp \
	src/TestList.cpp \
	src/TimeConstraint.cpp \
	src/TestDetails.cpp \
	src/MemoryOutStream.cpp \
	src/DeferredTestReporter.cpp \
	src/DeferredTestResult.cpp \
	src/XmlTestReporter.cpp \
	src/CurrentTest.cpp
	
ifeq ($(MSYSTEM), MINGW32)
  src += src/Win32/TimeHelpers.cpp
else
  src += src/Posix/SignalTranslator.cpp \
	src/Posix/TimeHelpers.cpp
endif

test_src = src/tests/Main.cpp \
	src/tests/TestAssertHandler.cpp \
	src/tests/TestChecks.cpp \
	src/tests/TestUnitTest++.cpp \
	src/tests/TestTest.cpp \
	src/tests/TestTestResults.cpp \
	src/tests/TestTestRunner.cpp \
	src/tests/TestCheckMacros.cpp \
	src/tests/TestTestList.cpp \
	src/tests/TestTestMacros.cpp \
	src/tests/TestTimeConstraint.cpp \
	src/tests/TestTimeConstraintMacro.cpp \
	src/tests/TestMemoryOutStream.cpp \
	src/tests/TestDeferredTestReporter.cpp \
	src/tests/TestXmlTestReporter.cpp \
	src/tests/TestCurrentTest.cpp

objects = $(patsubst %.cpp, %.o, $(src))
test_objects = $(patsubst %.cpp, %.o, $(test_src))
dependencies = $(subst .o,.d,$(objects))
test_dependencies = $(subst .o,.d,$(test_objects))

define make-depend
  $(CXX) $(CXXFLAGS) -M $1 | \
  $(SED) -e 's,\($(notdir $2)\) *:,$(dir $2)\1: ,' > $3.tmp
  $(SED) -e 's/#.*//' \
      -e 's/^[^:]*: *//' \
      -e 's/ *\\$$//' \
      -e '/^$$/ d' \
      -e 's/$$/ :/' $3.tmp >> $3.tmp
  $(MV) $3.tmp $3
endef


all: $(test)


$(lib): $(objects) 
	@echo Creating $(lib) library...
	@ar cr $(lib) $(objects)
    
$(test): $(lib) $(test_objects)
	@echo Linking $(test)...
	@$(CXX) $(LDFLAGS) -o $(test) $(test_objects) $(lib)
	@echo Running unit tests...
	@./$(test)

clean:
	-@$(RM) $(objects) $(test_objects) $(dependencies) $(test_dependencies) $(test) $(lib) 2> /dev/null

%.o : %.cpp
	@echo $<
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(CXX) $(CXXFLAGS) -c $< -o $(patsubst %.cpp, %.o, $<)


ifneq "$(MAKECMDGOALS)" "clean"
-include $(dependencies)
-include $(test_dependencies)
endif
