#include "PROG/Arguments.hpp"

namespace CTL::util {

Arguments::Arguments(int argc, char* argv[], std::string appName)
    : argc(argc)
    , argv(argv)
{
    cliApp = std::make_shared<CLI::App>(appName);
}

int Arguments::parse(bool helpOnError)
{
    this->defineArguments();
    try
    {
        int preVal, postVal;
        preVal = preParse();
        if(preVal != 0)
        {
            return preVal;
        }
        cliApp->parse(argc, argv);
        postVal = postParse();
        if(postVal != 0)
        {
            return postVal;
        }
    } catch(const CLI::CallForHelp& e)
    {
        cliApp->exit(e); // Prints help message
        return 1;
    } catch(const CLI::ParseError& e)
    {
        int exitcode = cliApp->exit(e);
        LOGE << io::xprintf("There was perse error with exit code %d catched.", exitcode);
        if(helpOnError)
        {
            std::cout << cliApp->help();
        }
        return -1;
    } catch(...)
    {
        LOGE << "Unknown exception catched";
        return -1;
    }
    return 0;
}

std::shared_ptr<CLI::App> Arguments::getCliApp() { return cliApp; }
} // namespace CTL::util
