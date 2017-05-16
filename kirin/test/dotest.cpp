#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <string>
#include <stdlib.h>
//#include "kirin/common/log.h"
//#include "kirin/test/test.h"
#include "kirin/common/common.h"


using namespace std;
using namespace CppUnit;

int main( int argc, char **argv)
{
    //KIRIN_LOG_CONFIG(KIRIN_DOTEST_LOGGER_CONF);
    bool ok = false;
    {
        TextUi::TestRunner runner;
        runner.setOutputter(new CompilerOutputter(&runner.result(), std::cerr));
        TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();
        runner.addTest( registry.makeTest() );
        ok = runner.run("", false);
    }
    //KIRIN_LOG_SHUTDOWN();
    return ok ? 0 : 1;
}
