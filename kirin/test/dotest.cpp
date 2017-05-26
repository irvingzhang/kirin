#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <string>
#include <stdlib.h>
#include "kirin/common/common.h"


using namespace std;
using namespace CppUnit;

int main( int argc, char **argv)
{
    bool ok = false;
    {
        TextUi::TestRunner runner;
        runner.setOutputter(new CompilerOutputter(&runner.result(), std::cerr));
        TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();
        runner.addTest( registry.makeTest() );
        ok = runner.run("", false);
    }
    return ok ? 0 : 1;
}
